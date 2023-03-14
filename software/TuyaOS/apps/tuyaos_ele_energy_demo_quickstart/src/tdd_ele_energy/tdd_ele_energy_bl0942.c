/**
 * @file tdd_ele_energy_bl0942.c
 * @author www.tuya.com
 * @brief tdd_ele_energy_bl0942 module is used to
 * @version 1.0.0
 * @date 2022-05-12
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include <string.h>
#include "tuya_cloud_types.h"
#include "tuya_uart.h"
#include "uni_log.h"
#include "tuya_hal_mutex.h"
#include "tuya_hal_system.h"
#include "uni_time_queue.h"
#include "sys_timer.h"

#include "tdd_ele_energy_bl0942.h"
#include "tdl_ele_energy_ops.h"

/***********************************************************
*************************micro define***********************
***********************************************************/
#define ADDR_I_WAVE     0x01 //电流通道波形
#define ADDR_V_WAVE     0x02 //电压通道波形*
#define ADDR_I_RMS      0x03 //电流有效值
#define ADDR_V_RMS      0x04 //电压有效值
#define ADDR_I_FAST_RMS 0x05 //电流快速有效值*
#define ADDR_WATT       0x06 //有功功率
#define ADDR_CF_CNT     0x07 //有功电能脉冲计数
#define ADDR_FREQ       0x08 //工频频率
#define ADDR_STATUS     0x09 //状态
#define ADDR_VERSION    0x0F //版本
/*用户操作寄存器*/
#define ADDR_I_CHOS       0x11 //电流通道直流偏置校正
#define ADDR_I_RMSOS      0x12 //电流通道有效值小信号校正
#define ADDR_WA_CREEP     0x14 //有功功率防潜阈值
#define ADDR_FAST_RMS_TH  0x15 //
#define ADDR_FAST_RMS_CYC 0x16
#define ADDR_FREQ_CYC     0x17
#define ADDR_FUNX         0x18
#define ADDR_MODE         0x19
#define ADDR_GAIN_CR      0x1A
#define ADDR_SOFT_RESET   0x1C //软复位
#define ADDR_WRPROT       0x1D //用户写保护设置
/*用户写保护设置*/
/*注意 BL0940的读命令字节固定为0x58+ICADDR，写命令字节固定为0xA8+ICADDR；
SOP10封装芯片的IC_ADDR地址固定为0,BL0942 TSSOP14封装带地址选择管脚，
需根据A1~A2地址选择管脚的电平配置命令字节，可以进行多机并联通信*/
#define BL0942_ADDR_R  0x58
#define BL0942_ADDR_w  0xA8
#define TAR_RANGE_V    3
#define TAR_RANGE_I    5
#define TAR_RANGE_P    6
#define MAX_ENERGY_CNT 15
#define MAX_DATE_LEN   96

#define CAL_CNT              8
#define ERR_CNT              4
#define TDD_ELE_BL0942_P_REF 6378
#define TDD_ELE_BL0942_I_REF 25699
#define TDD_ELE_BL0942_V_REF 15883
#define TDD_ELE_BL0942_E_REF 5474

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef union {
    UCHAR_T single_byte[4];
    USHORT_T short_byte[2];
    UINT_T int_byte;
} RECV_DATA_U;

typedef struct {
    UCHAR_T head;
    UCHAR_T i_rms[3];
    UCHAR_T v_rms[3];
    UCHAR_T i_fast_rms[3];
    UCHAR_T wait[3];
    UCHAR_T cf_cnt[3];
    UCHAR_T freq[3];
    UCHAR_T status[3];
    UCHAR_T checksum;
} UART_FLAME_DATA_T;

typedef struct {
    UINT_T energy_kwh;    //实际用电量，千瓦小时 XXXXXX,1度电/LSB，最大65535度电
    UINT_T fir_cf_cnt;    //上一次的CF_CNT寄存器读数
    UINT_T mid_cf_cnt;    //小于1度电的脉冲底数
    UINT_T power_value;   // unit:	0.1W	XXXX.X
    UINT_T current_value; // unit:	0.001A	XX.XXX
    UINT_T voltage_value; // unit:0.1V		XXX.X
    UINT_T freq;          // unit:0.1Hz	XX.X
    UINT_T f_current_value;
} ELEC_DATA_T;

// tuya developer
typedef struct {
    UCHAR_T err_percent;
    BOOL_T is_start_test;
    BOOL_T test_finished;
    BOOL_T sample_finished;
    TIMER_ID timer_id;
    TDL_ELE_ENERGY_TEST_RESULT_E test_result;
} TDD_BL0942_FAC_TEST_T;

typedef enum {
    TDD_BL0942_SEND = 0,
    TDD_BL0942_RECV,
    TDD_BL0942_END,
} TDD_BL0942_MEASURE_STATUS_E;

/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC ELEC_DATA_T sg_bl0942_elec;

STATIC MUTEX_HANDLE sg_uart_mutex;
STATIC TDD_BL0942_FAC_TEST_T sg_factory_test;
STATIC TDD_BL0942_MEASURE_STATUS_E sg_uart_state;
STATIC TDL_ELE_ENERGY_COE_T sg_coe_params;
STATIC TDL_ELE_ENERGY_COE_T sg_def_coe_params;
STATIC TDL_ELE_ENERGY_COE_T sg_ref_coe_params;
STATIC TDD_BL0942_DRV_RESOURCE_T sg_hw_cfg;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief 获取pvie的值
 *
 * @param[out] pvie_val pvie值
 * @return VOID_T
 */
STATIC VOID_T __bl0942_read_pvie(OUT TDL_ELE_ENERGY_PVIE_T *pvie_val)
{
    if (NULL == pvie_val) {
        return;
    }

    pvie_val->power_val = sg_bl0942_elec.power_value;
    pvie_val->volt_val = sg_bl0942_elec.voltage_value;
    pvie_val->curr_val = sg_bl0942_elec.current_value;
    pvie_val->energy_val = sg_bl0942_elec.energy_kwh;
    sg_bl0942_elec.energy_kwh = 0;

    return;
}

/**
 * @brief 数据处理
 *
 * @param[in] src 源数据
 * @return VOID_T
 */
STATIC VOID_T __data_analysis_proc(IN CONST UCHAR_T *src)
{
    if (NULL == src) {
        return;
    }

    RECV_DATA_U f_revdate;
    UART_FLAME_DATA_T dest;

    memcpy(&dest, src, 23);
    tuya_hal_mutex_lock(sg_uart_mutex);

    // 电流
    f_revdate.int_byte = 0;
    memcpy(f_revdate.single_byte, dest.i_rms, 3);
    f_revdate.int_byte = f_revdate.int_byte * 100 / sg_coe_params.i_coe;
    sg_bl0942_elec.current_value = f_revdate.int_byte;

    // 电压
    f_revdate.int_byte = 0;
    memcpy(f_revdate.single_byte, dest.v_rms, 3);
    f_revdate.int_byte = f_revdate.int_byte * 10 / sg_coe_params.v_coe;
    sg_bl0942_elec.voltage_value = f_revdate.int_byte;

    // 频率
    f_revdate.int_byte = 0;
    memcpy(f_revdate.single_byte, dest.freq, 3);
    if (f_revdate.int_byte) {
        f_revdate.int_byte = 10000000 / f_revdate.int_byte;
    } else {
        f_revdate.int_byte = 500;
    }
    sg_bl0942_elec.freq = f_revdate.int_byte;

    // 功率
    f_revdate.int_byte = 0;
    memcpy(f_revdate.single_byte, dest.wait, 3);
    if (dest.wait[2] & 0x80) {
        f_revdate.single_byte[1] = ~dest.wait[1];
        f_revdate.single_byte[2] = ~dest.wait[2];
        f_revdate.single_byte[0] = ~dest.wait[0] + 1;
    }

    f_revdate.int_byte = f_revdate.int_byte * 100 / sg_coe_params.p_coe;
    sg_bl0942_elec.power_value = f_revdate.int_byte;

    // 电量增量
    f_revdate.int_byte = 0;
    memcpy(f_revdate.single_byte, dest.cf_cnt, 3);
    if (f_revdate.int_byte >= sg_bl0942_elec.fir_cf_cnt) {
        sg_bl0942_elec.mid_cf_cnt += (f_revdate.int_byte - sg_bl0942_elec.fir_cf_cnt) * 1000;
    } else {
        sg_bl0942_elec.mid_cf_cnt += (f_revdate.int_byte + (0xFFFFFF - sg_bl0942_elec.fir_cf_cnt)) * 1000;
    }

    sg_bl0942_elec.energy_kwh += sg_bl0942_elec.mid_cf_cnt / sg_coe_params.e_coe;
    sg_bl0942_elec.mid_cf_cnt = sg_bl0942_elec.mid_cf_cnt % sg_coe_params.e_coe;
    sg_bl0942_elec.fir_cf_cnt = f_revdate.int_byte;

    tuya_hal_mutex_unlock(sg_uart_mutex);

    return;
}

/**
 * @brief 写bl0942寄存器
 *
 * @param[in] reg_addr 寄存器地址
 * @param[in] cmd 命令
 * @param[in] buf 数据
 * @param[in] len 数据长度
 * @return VOID_T
 */
STATIC VOID_T __write_data_to_reg(IN UCHAR_T reg_addr, IN UCHAR_T cmd, IN CONST UCHAR_T *buf, IN UCHAR_T len)
{
    UCHAR_T i = 0;
    UCHAR_T write_buf[10] = {0};
    memset(write_buf, 0, SIZEOF(write_buf));

    if (len > 7) {
        return;
    }

    write_buf[0] = reg_addr;
    write_buf[1] = cmd;
    memcpy(write_buf + 2, buf, len);
    write_buf[2 + len] = 0;
    for (i = 0; i < 2 + len; i++) {
        write_buf[2 + len] += write_buf[i];
    }
    write_buf[2 + len] = ~write_buf[2 + len];
    ty_uart_send_data(sg_hw_cfg.uart_no, write_buf, 3 + len);

    return;
}

/**
 * @brief 读数据
 *
 * @param[out] pvie_val 读出的PVIE数据
 * @return TRUE 读成功 FALSE 读失败
 */
STATIC BOOL_T __read_data_form_reg(OUT TDL_ELE_ENERGY_PVIE_T *pvie_val)
{
    UCHAR_T sum = 0x58;
    UCHAR_T i = 0;
    UCHAR_T read_buf[23];

    memset(read_buf, 0, SIZEOF(read_buf));
    if (23 == ty_uart_read_data(sg_hw_cfg.uart_no, read_buf, 23)) {
        if (0x55 != read_buf[0]) {
            PR_ERR("data head err");
            goto _EXT_ERR;
        }
        for (i = 0; i < 22; i++) {
            sum += read_buf[i];
        }
        sum = ~sum;
        if (sum != read_buf[22]) {
            PR_ERR("check sum err");
            goto _EXT_ERR;
        }
        __data_analysis_proc(read_buf);
        __bl0942_read_pvie(pvie_val);
        return TRUE;
    } else {
        goto _EXT_ERR;
    }

_EXT_ERR:
    ty_uart_read_data(sg_hw_cfg.uart_no, read_buf, 23);
    return FALSE;
}

/**
 * @brief 初始化运行参数
 *
 * @return VOID_T
 */
STATIC VOID_T __bl0942_run_params_init(VOID_T)
{
    memset(&sg_bl0942_elec, 0, SIZEOF(ELEC_DATA_T));
    sg_uart_state = TDD_BL0942_SEND;

    return;
}

/**
 * @brief 零点检测控制
 *
 * @param[in] state TRUE 使能零点检测 FALSE 关闭零点检测
 * @return VOID_T
 */
STATIC VOID_T __bl0942_zero_detection_ctrl(IN BOOL_T state)
{
    UCHAR_T buf[3] = {0};

    ty_uart_init(sg_hw_cfg.uart_no, TYU_RATE_4800, TYWL_8B, TYP_NONE, TYS_STOPBIT1, MAX_DATE_LEN, TRUE);

    memset(buf, 0, 3);
    buf[0] = 0x55;
    __write_data_to_reg(BL0942_ADDR_w, ADDR_WRPROT, buf, 3);

    memset(buf, 0, 3);
    if (state) {
        buf[0] = 0x26;
    } else {
        buf[0] = 0x24;
    }
    __write_data_to_reg(BL0942_ADDR_w, ADDR_FUNX, buf, 3);

    memset(buf, 0, 3);
    __write_data_to_reg(BL0942_ADDR_w, ADDR_WRPROT, buf, 3);

    return;
}

/**
 * @brief 获取市电频率
 *
 * @return 检测到的频率
 */
STATIC UINT_T __bl0942_get_elec_freq(VOID_T)
{
    UINT_T tmp = sg_bl0942_elec.freq;

    if ((tmp > 650) || (tmp < 450)) {
        return 500;
    }

    return tmp;
}

/**
 * @brief 初始化校准参数
 *
 * @param[in] res 采样电阻 1mR/2mR
 * @param[in] vin 参考电压
 * @param[in] iin 参考电流
 * @param[in] pin 参考功率
 * @return VOID_T
 */
STATIC VOID_T __bl0942_coe_params_init(IN UCHAR_T res, IN UINT_T vin, IN UINT_T iin, IN UINT_T pin)
{
    if ((!vin) || (!iin) || (!pin)) {
        return;
    }

    if (!res) {
        res = 1; // 不设置时, 默认1mR
        PR_ERR("sample resistance err, set resistance to the default value:%d", res);
    }
    memset(&sg_def_coe_params, 0x00, SIZEOF(sg_def_coe_params));
    sg_def_coe_params.v_coe = TDD_ELE_BL0942_V_REF;
    sg_def_coe_params.i_coe = TDD_ELE_BL0942_I_REF * res;
    sg_def_coe_params.p_coe = TDD_ELE_BL0942_P_REF * res;
    sg_def_coe_params.e_coe = TDD_ELE_BL0942_E_REF * res;

    sg_ref_coe_params.p_coe = pin;
    sg_ref_coe_params.v_coe = vin;
    sg_ref_coe_params.i_coe = iin;

    return;
}

/**
 * @brief bl0942出厂校准测试
 *
 * @return TDL_ELE_TEST_PASS 产测成功 TDL_ELE_TEST_FAIL 产测失败
 */
STATIC TDL_ELE_ENERGY_TEST_RESULT_E __bl0942_fac_test_cal(VOID_T)
{
    TDL_ELE_ENERGY_TEST_RESULT_E test_ret = TDL_ELE_TEST_FAIL;

    PR_NOTICE("actual V:%d I:%d P:%d", sg_bl0942_elec.voltage_value, sg_bl0942_elec.current_value,
              sg_bl0942_elec.power_value);

    PR_NOTICE("default V:%d I:%d P:%d", sg_ref_coe_params.v_coe, sg_ref_coe_params.i_coe, sg_ref_coe_params.p_coe);

    tuya_hal_mutex_lock(sg_uart_mutex);
    if ((tdl_ele_energy_rang_judge(sg_bl0942_elec.voltage_value, sg_ref_coe_params.v_coe,
                                   sg_factory_test.err_percent)) &&
        (tdl_ele_energy_rang_judge(sg_bl0942_elec.power_value, sg_ref_coe_params.p_coe, sg_factory_test.err_percent))) {
        sg_coe_params.v_coe = sg_coe_params.v_coe * sg_bl0942_elec.voltage_value / sg_ref_coe_params.v_coe;
        sg_coe_params.i_coe = sg_coe_params.i_coe * sg_bl0942_elec.current_value / sg_ref_coe_params.i_coe;
        sg_coe_params.p_coe = sg_coe_params.p_coe * sg_bl0942_elec.power_value / sg_ref_coe_params.p_coe;
        sg_coe_params.e_coe = ((FLOAT_T)3600000 / 16384) * sg_coe_params.p_coe / 256; // 3600000*Power_K/16384/256
        test_ret = TDL_ELE_TEST_PASS;
    } else {
        test_ret = TDL_ELE_TEST_FAIL;
    }
    tuya_hal_mutex_unlock(sg_uart_mutex);

    return test_ret;
}

/**
 * @brief 产测判断定时回调
 *
 * @param[in] timerID 定时器ID
 * @param[in] pTimerArg arg 参数
 * @return VOID_T
 */
STATIC VOID_T __bl0942_fac_test_timer_cb(UINT_T timerID, PVOID_T pTimerArg)
{
    if (sg_factory_test.sample_finished) {
        sg_factory_test.test_result = __bl0942_fac_test_cal();
        sg_factory_test.test_finished = TRUE;
        sys_stop_timer(sg_factory_test.timer_id);
    }

    return;
}

/**
 * @brief bl0942芯片初始化
 *
 * @param[in] drv_cfg 芯片配置参数
 * @param[in] platform_resource 平台外设资源
 * @return OPRT_OK 代表成功。如果是其他错误码，请参考 tuya_error_code.h
 */
STATIC OPERATE_RET __bl0942_chip_init(IN CONST TDL_ELE_ENERGY_CFG_T *drv_cfg, IN CONST VOID_T *platform_resource)
{
    if ((NULL == drv_cfg) || (NULL == platform_resource)) {
        PR_DEBUG("init info is null ptr");
        return OPRT_INVALID_PARM;
    }

    /*全局数据初始化*/
    memset(&sg_coe_params, 0x00, SIZEOF(sg_coe_params));
    memset(&sg_ref_coe_params, 0x00, SIZEOF(sg_ref_coe_params));
    memset(&sg_factory_test, 0x00, SIZEOF(sg_factory_test));
    memset(&sg_hw_cfg, 0x00, SIZEOF(sg_hw_cfg));
    memcpy(&sg_hw_cfg, (TDD_BL0942_DRV_RESOURCE_T *)platform_resource, SIZEOF(sg_hw_cfg));

    /*硬件资源初始化*/
    ty_uart_init(sg_hw_cfg.uart_no, TYU_RATE_4800, TYWL_8B, TYP_NONE, TYS_STOPBIT1, MAX_DATE_LEN, TRUE);
    OPERATE_RET ret = tuya_hal_mutex_create_init(&sg_uart_mutex);
    if (OPRT_OK != ret) {
        PR_ERR("uart mutex init err");
        return ret;
    }

    /*运行参数初始化*/
    if (TDL_ELE_VOLT_120V == drv_cfg->params.volt_type) {
        __bl0942_coe_params_init(drv_cfg->params.resval, TDL_ELE_V_REF_120, TDL_ELE_I_REF_120, TDL_ELE_P_REF_120);
    } else {
        __bl0942_coe_params_init(drv_cfg->params.resval, TDL_ELE_V_REF_220, TDL_ELE_I_REF_220, TDL_ELE_P_REF_220);
    }
    __bl0942_run_params_init();

    if (TDL_ELE_TEST_MODE == drv_cfg->mode) {
        sg_factory_test.is_start_test = TRUE;
        sg_factory_test.test_result = TDL_ELE_TEST_FAIL;
        sg_factory_test.err_percent = drv_cfg->params.err_percent;
        if ((sg_factory_test.err_percent > 100) || (sg_factory_test.err_percent < 1)) {
            sg_factory_test.err_percent = 5; // 默认产测误差
        }
        memcpy(&sg_coe_params, &sg_def_coe_params, SIZEOF(sg_coe_params));
        PR_NOTICE("-->bl0942 test err percent:%d", sg_factory_test.err_percent);
        ret = sys_add_timer(__bl0942_fac_test_timer_cb, NULL, &sg_factory_test.timer_id);
        if (OPRT_OK != ret) {
            PR_ERR("bl0937_fac_test timer init err:%d", ret);
            return ret;
        }
        sys_start_timer(sg_factory_test.timer_id, 1000, TIMER_CYCLE);
    } else {
        sg_factory_test.is_start_test = FALSE;
        sg_factory_test.test_result = drv_cfg->fac_test.test_result;
        if (TDL_ELE_TEST_PASS == sg_factory_test.test_result) {
            memcpy(&sg_coe_params, &drv_cfg->fac_test.coe_data, SIZEOF(sg_coe_params));
        } else {
            memcpy(&sg_coe_params, &sg_def_coe_params, SIZEOF(sg_coe_params));
        }
    }

    return OPRT_OK;
}

/**
 * @brief 获取pvie的值
 *
 * @param[out] pvie_val pvie数据
 * @return VOID_T
 */
STATIC VOID_T __bl0942_read_pvie_value(OUT TDL_ELE_ENERGY_PVIE_T *pvie_val)
{
    if (NULL == pvie_val) {
        return;
    }

    STATIC UCHAR_T i = 0;
    STATIC UCHAR_T errcnt = 0; //产测时读错次数持续超过x次
    UCHAR_T send_buf[2] = {0x58, 0xaa};

    switch (sg_uart_state) {
    case TDD_BL0942_SEND: {
        ty_uart_send_data(sg_hw_cfg.uart_no, send_buf, SIZEOF(send_buf));
        if (sg_factory_test.is_start_test) {
            if (i < CAL_CNT) {
                i++;
            }
        }
        sg_uart_state = TDD_BL0942_RECV;
    } break;

    case TDD_BL0942_RECV: {
        if (sg_factory_test.is_start_test) {
            if (__read_data_form_reg(pvie_val)) {
                if (i == CAL_CNT) {
                    sg_factory_test.sample_finished = TRUE;
                    sg_uart_state = TDD_BL0942_END;
                    return;
                }
                errcnt = 0;
            } else {
                if (sg_factory_test.sample_finished == FALSE) {
                    errcnt++;
                    if (errcnt >= ERR_CNT) {
                        sg_factory_test.sample_finished = TRUE;
                        sg_uart_state = TDD_BL0942_END;
                        return;
                    } else {
                        i--;
                    }
                }
            }
        } else {
            __read_data_form_reg(pvie_val);
        }
        sg_uart_state = TDD_BL0942_SEND;
    } break;

    default:
        break;
    }

    return;
}

/**
 * @brief 读芯片出厂校准参数
 *
 * @param[out] fac_test_data 校准数据
 * @return VOID_T
 */
STATIC VOID_T __bl0942_read_fac_cal_data(OUT TDL_ELE_ENERGY_FAC_TEST_T *fac_test_data)
{
    if (NULL == fac_test_data) {
        PR_ERR("fac cal data ptr null");
        return;
    }

    fac_test_data->test_result = sg_factory_test.test_result;
    fac_test_data->coe_data.v_coe = sg_coe_params.v_coe;
    fac_test_data->coe_data.i_coe = sg_coe_params.i_coe;
    fac_test_data->coe_data.p_coe = sg_coe_params.p_coe;
    fac_test_data->coe_data.e_coe = sg_coe_params.e_coe;

    return;
}

/**
 * @brief 读产测状态
 *
 * @return TRUE 产测结束 FALSE 产测未开始或产测未结束
 */
STATIC BOOL_T __bl0942_read_fac_test_state(VOID_T)
{
    return sg_factory_test.test_finished;
}

/**
 * @brief bl0942开关控制
 *
 * @param[in] state TRUE 使能芯片采样 FALSE 关闭芯片采样
 * @return VOID_T
 */
STATIC VOID_T __bl0942_ctrl(IN BOOL_T state)
{
    if (state) {
        ty_uart_init(sg_hw_cfg.uart_no, TYU_RATE_4800, TYWL_8B, TYP_NONE, TYS_STOPBIT1, MAX_DATE_LEN, TRUE);
        __bl0942_run_params_init();
    } else {
        ty_uart_free(sg_hw_cfg.uart_no);
        sg_uart_state = TDD_BL0942_END;
    }

    return;
}

/*******************************************drv init && ops
 * register*************************************************************/
/**
 * @brief bl0942 ops 注册
 **/
STATIC CONST TDL_ELE_ENERGY_DRV_OPS_T __bl0942_drv_ops = {
    .drv_init_cb = __bl0942_chip_init,
    .drv_ctrl_cb = __bl0942_ctrl,
    .drv_read_pvie_cb = __bl0942_read_pvie_value,
    .drv_read_fac_test_data_cb = __bl0942_read_fac_cal_data,
    .drv_read_fac_test_state_cb = __bl0942_read_fac_test_state,
};

/**
 * @brief bl0942初始化
 *
 * @param[in] drv_cfg 芯片配置参数
 * @param[in] platform_resource 驱动芯片所需的外设资源
 * @return OPRT_OK 代表成功。如果是其他错误码，请参考 tuya_error_code.h
 */
OPERATE_RET tdd_ele_energy_bl0942_init(IN CONST TDL_ELE_ENERGY_CFG_T *drv_cfg,
                                       IN CONST TDD_BL0942_DRV_RESOURCE_T *platform_resource)
{
    if ((NULL == drv_cfg) || (NULL == platform_resource)) {
        return OPRT_INVALID_PARM;
    }
    TDL_ELE_ENERGY_DRIVER_T bl0942_drv;
    memset(&bl0942_drv, 0x00, SIZEOF(bl0942_drv));

    bl0942_drv.name = "bl0942";
    bl0942_drv.cfg = drv_cfg;
    bl0942_drv.ops = &__bl0942_drv_ops;
    bl0942_drv.platform_resource = (CONST VOID_T *)platform_resource;

    return tdl_ele_energy_driver_register(&bl0942_drv);
}