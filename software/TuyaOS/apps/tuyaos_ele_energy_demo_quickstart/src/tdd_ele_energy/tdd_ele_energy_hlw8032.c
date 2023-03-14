/**
 * @file tdd_ele_energy_hlw8032.c
 * @author www.tuya.com
 * @brief tdd_ele_energy_hlw8032 module is used to
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
#include "uni_time_queue.h"
#include "tkl_timer.h"
#include "sys_timer.h"

#include "tdd_ele_energy_hlw8032.h"
#include "tdl_ele_energy_ops.h"

/***********************************************************
*************************micro define***********************
***********************************************************/
#define RX_BUF_DEBUG         0 /* 打印一包数据帧 */
#define SAMPLE_RESISTANCE_MR 1 //使用的采样锰铜电阻mR值
#define UART_IND_HD          0
#define UART_IND_5A          1 //第二个寄存器的校验在上层帧校验已经完成
#define UART_IND_VK          2
#define UART_IND_VT          5
#define UART_IND_IK          8
#define UART_IND_IT          11
#define UART_IND_PK          14
#define UART_IND_PT          17
#define UART_IND_FG          20 //状态寄存器是data[20]，手册上的data[21]是错误的
#define UART_IND_EN          21
#define UART_IND_SM          23
#define ARRAY_LEN            3 //平滑滤波buf长度
#define COUNT_NUM            1 //超时更新数据次数
#define K_V                  (1.88)
#define K_I                  1
#define K_P                  (K_V * K_I)
#define ENERGY_FLOW_NUM      65536 //电量采集，电能溢出时的脉冲计数值
#define TAR_RANGE_V          5
#define TAR_RANGE_I          10
#define TAR_RANGE_P          10
#define MAX_ENERGY_CNT       15
#define MAX_DATE_LEN         96
#define RES_VAL              1 // 定时1ms固定硬件定时ms

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef struct runing_info_s {
    ULONG_T voltage;    //当前电压值，单位为0.1V
    ULONG_T current;    //当前电流值,单位为0.01A
    ULONG_T power;      //当前功率值,单位为0.1W
    ULONG_T add_energy; //增加的电量

    ULONG_T energy;          //当前消耗电能值对应的脉冲个数
    ULONG_T energy_current;  //电能脉冲当前统计值
    ULONG_T energy_last;     //电能脉冲上次统计值
    ULONG_T energy_flowflag; //电能脉冲溢出标致

    ULONG_T energy_unit; // 0.001度点对应的脉冲个数
    ULONG_T vk;
    ULONG_T ik;
    ULONG_T pk;
} RUNING_INFO_T;

typedef enum {
    FLAME_HEAD1 = 0,
    FLAME_HEAD2,
    RECVING,
} UART_RECV_STAT_E;

typedef struct {
    UCHAR_T err_percent;
    BOOL_T is_start_test;
    BOOL_T test_finished;
    BOOL_T sample_finished;
    TIMER_ID timer_id;
    TDL_ELE_ENERGY_TEST_RESULT_E test_result;
} TDD_HLW8012_FAC_TEST_T;

/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC RUNING_INFO_T sg_runing_params;
STATIC USHORT_T sg_p_ref = 0; //参考功率值,如以1000W校正。1000.0W
STATIC USHORT_T sg_v_ref = 0; //参考电压  220.0V
STATIC USHORT_T sg_i_ref = 0; //参考电流  1000W,220V条件下是4.545A
/*产测换算 电压电流功率寄存器*/
STATIC ULONG_T sg_voltage_out = 0;
STATIC ULONG_T sg_elec_out = 0;
STATIC ULONG_T sg_power_out = 0;
/*产测换算 电量系数使用*/
STATIC UCHAR_T sg_calc_accl = 0;
STATIC UART_RECV_STAT_E sg_uart_recvstat = FLAME_HEAD1;
STATIC UCHAR_T sg_rev_buf[MAX_DATE_LEN] = {0};
STATIC UINT_T sg_addplus = 0;
STATIC UINT_T sg_timecnt = 0;
STATIC UINT_T sg_time_start = 0;
STATIC UINT_T sg_time_end = 0;

STATIC TDD_HLW8032_DRV_RESOURCE_T sg_hw_cfg;
STATIC TDD_HLW8012_FAC_TEST_T sg_factory_test;
STATIC TDL_ELE_ENERGY_COE_T sg_coe_params;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief 产测校准硬件定时器回调
 *
 * @return none
 */
STATIC VOID_T __hw_timer_cb(VOID_T)
{
    sg_timecnt++;
}

/**
 * @brief 硬件定时器初始化
 *
 * @param[in] timer_no 定时器号
 * @return OPRT_OK 代表成功。如果是其他错误码，请参考 tuya_error_code.h
 */
STATIC OPERATE_RET __drv_hw_timer_init(IN UCHAR_T timer_no)
{
    TUYA_TIMER_BASE_CFG_E timer_cfg;

    memset(&timer_cfg, 0, SIZEOF(timer_cfg));
    timer_cfg.mode = TUYA_TIMER_MODE_PERIOD;
    timer_cfg.cb = __hw_timer_cb;
    timer_cfg.args = NULL;

    return tkl_timer_init(timer_no, &timer_cfg);
}

/**
 * @brief 硬件定时器开关控制
 *
 * @param[in] delay_times 定时周期
 * @param[in] timer_no 定时ID
 * @param[in] stat 定时器开关状态 true：开 false: 关
 * @return VOID_T
 */
STATIC VOID_T __hw_timer_ctrl(IN UINT_T delay_times, IN UCHAR_T timer_no, IN BOOL_T stat)
{
    if (stat) {
        tkl_timer_start(timer_no, delay_times * 1000);
    } else {
        tkl_timer_stop(timer_no);
    }
    PR_DEBUG("hlw timer[%d] is %s, times:%d", timer_no, ((stat == TRUE) ? "open" : "closed"), delay_times);

    return;
}

/**
 * @brief 获取PVI滤波数据
 *
 * @param[in] arr 源数据
 * @return 滤波结果值
 */
STATIC ULONG_T __get_pvi_fiter_value(IN ULONG_T *arr)
{
    INT_T maxIndex = 0;
    INT_T minIndex = 0;
    INT_T j = 0;

    if (NULL == arr) {
        return 0;
    }

    /*找最大最小值位置,避免所有数据一样时minIndex等于maxIndex*/
    for (j = 1; j < ARRAY_LEN; j++) {
        if (arr[maxIndex] <= arr[j]) {
            maxIndex = j;
        }
        if (arr[minIndex] > arr[j]) {
            minIndex = j;
        }
    }
    /*去除最大最小值的最新值返回*/
    for (j = 0; j < ARRAY_LEN; j++) {
        if ((maxIndex == j) || (minIndex == j)) {
            continue;
        } else {
            return arr[j];
        }
    }

    return 0;
}

/**
 * @brief 是否需要更新电压,电流数据
 *
 * @param[in] arr 源数据
 * @param[in] data 目标数据
 * @return TRUE 需要更新 FALSE 不需要更新
 */
STATIC BOOL_T __is_need_updata_data(IN ULONG_T *arr, IN ULONG_T data)
{
    if (NULL == arr) {
        return FALSE;
    }

    if (arr[0] == data) {
        return FALSE;
    }

    return TRUE;
}

/**
 * @brief 更新电压、电流、功率的列表
 *
 * @param[in] arr 源数据
 * @param[in] data 目标数据
 * @return VOID_T
 */
STATIC VOID_T __updata_pvi_value(IN ULONG_T *arr, IN ULONG_T data)
{
    if (NULL == arr) {
        return;
    }
    INT_T i = ARRAY_LEN - 1;

    for (i = ARRAY_LEN - 1; i > 0; i--) {
        arr[i] = arr[i - 1];
    }
    arr[0] = data;

    return;
}

/**
 * @brief 复位电压、电流、功率的列表
 *
 * @param[in] arr 源数据
 * @param[in] data 复位值
 * @return VOID_T
 */
STATIC VOID_T __reset_pvi_value(IN ULONG_T *arr, IN ULONG_T data)
{
    if (NULL == arr) {
        return;
    }
    INT_T i = ARRAY_LEN - 1;

    for (i = ARRAY_LEN - 1; i >= 0; i--) {
        arr[i] = data;
    }

    return;
}

/**
 * @brief 串口数据帧校验
 *
 * @param[in] src 待校验数据
 * @param[in] offset 偏移量
 * @return TRUE 校验成功 FALSE 校验失败
 */
STATIC BOOL_T __frame_head_check(IN UCHAR_T *src, IN UCHAR_T offset)
{
    if (NULL == src) {
        return FALSE;
    }

    switch (offset) {
    case 0: {
        if ((src[offset] == 0xAA) || (src[offset] == 0x55) || (src[offset] & 0xF0) == 0xF0) {
            return TRUE;
        } else {
            return FALSE;
        }
    } break;

    case 1: {
        if (src[offset] == 0x5A) {
            return TRUE;
        } else {
            return FALSE;
        }
    } break;

    default:
        return FALSE;
    }

    return FALSE;
}

/**
 * @brief 计算帧校验和
 *
 * @param[in] src 帧数据
 * @param[in] len 数据长度
 * @return 校验和
 */
STATIC INT_T __frame_check_sum_cal(IN UCHAR_T *src, IN UCHAR_T len)
{
    INT_T sum = 0;
    UCHAR_T i = 0;

    for (i = 0; i < len; i++) {
        sum += src[i];
    }
    return (sum & 0xff);
}

/**
 * @brief 帧数据解析
 *
 * @param[in] rev_buf 接收数据buf首地址
 * @param[in] rev_len 接收数据长度
 * @return
 */
STATIC INT_T __frame_data_analysis(IN UCHAR_T *rev_buf, IN INT_T rev_len)
{
    UCHAR_T startFlag = 0;

    ULONG_T voltage_k = 0;
    ULONG_T voltage_t = 0;
    ULONG_T voltage = 0;
    STATIC ULONG_T voltage_a[ARRAY_LEN] = {0};
    STATIC UINT_T voltageCnt = 0;

    ULONG_T electricity_k = 0;
    ULONG_T electricity_t = 0;
    ULONG_T electricity = 0;
    STATIC ULONG_T electricity_a[ARRAY_LEN] = {0};
    STATIC UINT_T electricityCnt = 0;

    ULONG_T power_k = 0;
    ULONG_T power_t = 0;
    ULONG_T power = 0;
    STATIC ULONG_T power_a[ARRAY_LEN] = {0};
    STATIC UINT_T powerCnt = 0;

    UINT_T energy_cnt = 0;
    UCHAR_T energy_flowflag = 0;

    STATIC UCHAR_T first_right_time = 0; //前几个数据包的电量统计不对

    UCHAR_T pivFlag[3] = {0, 0, 0};

#if RX_BUF_DEBUG
    INT_T i = 0;
    for (i = 0; i < 24; i++) {
        PR_DEBUG("%02x ", rev_buf[i]);
    }
    // PR_DEBUG("\n");
#endif
    startFlag = rev_buf[UART_IND_HD];
    switch (startFlag) {
    case 0xAA:
        //芯片未校准
        PR_ERR("hlw8032 not fac test");
        break;
        /* 芯片手册中首位寄存器为0x55才允许更新电压电流功率值，但是实际使用芯片时发现当功率溢出(为0)时，
            首位寄存器始终为0xf2。这会导致即使电压改变了，也无法更新电压的值，因此在首位寄存器为0x55和
            PVI溢出时都进行PVI计算，为0xaa或者0xf1时才不予处理。
        */
    default:
        if ((startFlag & 0xF1) == 0xF1) { //存储区异常，芯片坏了
            //芯片坏掉，反馈服务器
            PR_ERR("hlw8032 flash error");
            break;
        }
        /* 计算电压值 */
        if ((startFlag & 0xF8) == 0xF8) { //电压溢出
            voltage = 0;
            pivFlag[2] = 1;
        } else if ((rev_buf[UART_IND_FG] & 0x40) == 0x40) { //获取当前电压标致，为1时说明电压检测OK
            voltage_k = ((rev_buf[UART_IND_VK] << 16) | (rev_buf[UART_IND_VK + 1] << 8) |
                         (rev_buf[UART_IND_VK + 2])); //电压系数
            voltage_t = ((rev_buf[UART_IND_VT] << 16) | (rev_buf[UART_IND_VT + 1] << 8) |
                         (rev_buf[UART_IND_VT + 2])); //电压周期

            if (__is_need_updata_data(voltage_a, voltage_t)) {
                __updata_pvi_value(voltage_a, voltage_t);
                voltageCnt = 0;
            } else {
                voltageCnt++;
                if (voltageCnt >= COUNT_NUM) {
                    voltageCnt = 0;
                    __updata_pvi_value(voltage_a, voltage_t);
                }
            }
            voltage_t = __get_pvi_fiter_value(voltage_a);
            sg_voltage_out = voltage_t;

            if (voltage_t == 0) {
                voltage = 0;
            } else {
                if ((TDL_ELE_TEST_PASS == sg_factory_test.test_result) && (FALSE == sg_factory_test.is_start_test)) {
                    voltage = (sg_coe_params.v_coe / voltage_t) * 100;
                } else {
                    voltage = voltage_k * 100 / voltage_t; //电压10mV值，避免溢出
                    voltage = voltage * 10 * K_V;          //电压mV值
                }
            }
            pivFlag[2] = 1;
        } else {
            pivFlag[2] = 0;
        }

        /* 计算电流值 */
        if ((startFlag & 0xF4) == 0xF4) { //电流溢出
            electricity = 0;
            pivFlag[1] = 1;
        } else if ((rev_buf[UART_IND_FG] & 0x20) == 0x20) {
            electricity_k = ((rev_buf[UART_IND_IK] << 16) | (rev_buf[UART_IND_IK + 1] << 8) |
                             (rev_buf[UART_IND_IK + 2])); //电流系数
            electricity_t = ((rev_buf[UART_IND_IT] << 16) | (rev_buf[UART_IND_IT + 1] << 8) |
                             (rev_buf[UART_IND_IT + 2])); //电流周期

            if (__is_need_updata_data(electricity_a, electricity_t)) {
                __updata_pvi_value(electricity_a, electricity_t);
                electricityCnt = 0;
            } else {
                electricityCnt++;
                if (electricityCnt >= COUNT_NUM) {
                    electricityCnt = 0;
                    __updata_pvi_value(electricity_a, electricity_t);
                }
            }
            electricity_t = __get_pvi_fiter_value(electricity_a);
            sg_elec_out = electricity_t;

            if (electricity_t == 0) {
                electricity = 0;
            } else {
                if ((TDL_ELE_TEST_PASS == sg_factory_test.test_result) && (FALSE == sg_factory_test.is_start_test)) {
                    electricity = sg_coe_params.i_coe / electricity_t;
                } else {
                    if (electricity_k < 0xffffffff / 1000) {
                        electricity = electricity_k * 1000 / electricity_t; //增加精确度
                        electricity = electricity * 1 * K_I;                //电流mA值
                    } else {
                        electricity = electricity_k * 100 / electricity_t; //电流10mA值，避免溢出
                        electricity = electricity * 10 * K_I;              //电流mA值
                    }
                }

#if (SAMPLE_RESISTANCE_MR == 1)
//由于使用1mR的电阻，电流和功率需要不除以2
#elif (SAMPLE_RESISTANCE_MR == 2)
                //由于使用2mR的电阻，电流和功率需要除以2
                electricity >>= 1;
#endif
            }
            pivFlag[1] = 1;
        } else {
            pivFlag[1] = 0;
        }

        /* 计算功率值 */
        if ((startFlag & 0xF2) == 0xF2) { //功率溢出
            power = 0;
            pivFlag[0] = 1;
        } else if ((rev_buf[UART_IND_FG] & 0x10) == 0x10) {
            power_t = ((rev_buf[UART_IND_PT] << 16) | (rev_buf[UART_IND_PT + 1] << 8) |
                       (rev_buf[UART_IND_PT + 2])); //功率周期
            power_k = ((rev_buf[UART_IND_PK] << 16) | (rev_buf[UART_IND_PK + 1] << 8) |
                       (rev_buf[UART_IND_PK + 2])); //功率系数
            if (__is_need_updata_data(power_a, power_t)) {
                __updata_pvi_value(power_a, power_t);
                powerCnt = 0;
            } else {
                powerCnt++;
                if (powerCnt >= COUNT_NUM) {
                    powerCnt = 0;
                    __updata_pvi_value(power_a, power_t);
                }
            }
            power_t = __get_pvi_fiter_value(power_a);
            sg_power_out = power_t;

            if (power_t == 0) {
                power = 0;
            } else {
                if ((TDL_ELE_TEST_PASS == sg_factory_test.test_result) && (FALSE == sg_factory_test.is_start_test)) {
                    power = (sg_coe_params.p_coe / power_t) * 100;
                } else {
                    power = power_k * 100 / power_t; //功率10mw值，避免溢出
                    power = power * 10 * K_P;        //功率mw值
                }
#if (SAMPLE_RESISTANCE_MR == 1)
//由于使用1mR的电阻，电流和功率需要不除以2
#elif (SAMPLE_RESISTANCE_MR == 2)
                //由于使用2mR的电阻，电流和功率需要除以2
                power >>= 1;
#endif
            }
            pivFlag[0] = 1;
        } else {
            pivFlag[0] = 0;
        }
        /*计算电能值*/
        if (startFlag == 0x55) {
            power_k = ((rev_buf[UART_IND_PK] << 16) | (rev_buf[UART_IND_PK + 1] << 8) |
                       (rev_buf[UART_IND_PK + 2]));        //功率系数
            energy_flowflag = (rev_buf[UART_IND_FG] >> 7); //获取当前电能计数溢出标致
            sg_runing_params.energy_current =
                ((rev_buf[UART_IND_EN] << 8) | (rev_buf[UART_IND_EN + 1])); //更新当前的脉冲计数值
            if (sg_runing_params.energy_flowflag != energy_flowflag) { //每次计数溢出时更新当前脉冲计数值
                sg_runing_params.energy_flowflag = energy_flowflag;
                if (sg_runing_params.energy_current > sg_runing_params.energy_last) { //
                    sg_runing_params.energy_current = 0;
                    sg_runing_params.energy_last = 0; // my add
                    energy_cnt = 0;                   // my add
                } else {
                    energy_cnt = sg_runing_params.energy_current + ENERGY_FLOW_NUM - sg_runing_params.energy_last;
                }
            } else {
                energy_cnt = sg_runing_params.energy_current - sg_runing_params.energy_last;
            }
            sg_runing_params.energy_last = sg_runing_params.energy_current;

            if (TRUE == sg_factory_test.is_start_test) {
                if (0 == sg_calc_accl) {
                    sg_time_start = sg_timecnt;
                } else if (MAX_ENERGY_CNT == sg_calc_accl) {
                    sg_time_end = sg_timecnt;
                }

                if (sg_calc_accl <= MAX_ENERGY_CNT) {
                    if (sg_calc_accl) {
                        sg_addplus += energy_cnt;
                        if (sg_calc_accl == MAX_ENERGY_CNT) {
                            sg_factory_test.sample_finished = TRUE;
                        }
                    }
                    sg_calc_accl++;
                }
            }

            /*2022.3.4:去除e_coe校准系数的使用*/
            if ((TDL_ELE_TEST_PASS == sg_factory_test.test_result) && (FALSE == sg_factory_test.is_start_test)) {
                sg_runing_params.energy_unit = sg_coe_params.e_coe * 10;
            } else {
                sg_runing_params.energy_unit = 1800000000;
                if (power_k >> 1 != 0) {
                    sg_runing_params.energy_unit /= (power_k >> 1); // 1mR采样电阻0.001度电对应的脉冲个数
                } else {
                    sg_runing_params.energy_unit = 0;
                    PR_DEBUG("! hlw8032 power k = 0!");
                }
    #if (SAMPLE_RESISTANCE_MR == 1)
    // 1mR锰铜电阻对应的脉冲个数
    #elif (SAMPLE_RESISTANCE_MR == 2)
                // 2mR锰铜电阻对应的脉冲个数
                sg_runing_params.energy_unit = (sg_runing_params.energy_unit << 1); // 2mR采样电阻0.001度电对应的脉冲个数
    #endif
                sg_runing_params.energy_unit = sg_runing_params.energy_unit *
                                            10; // 0.001度电对应的脉冲个数(计算个数时放大了10倍，所以在这里也要放大10倍)
                sg_runing_params.energy_unit /= K_P;
            }

            if (first_right_time < 4) {
                energy_cnt = 0;
                first_right_time++;
            }
            //电能使用量
            sg_runing_params.energy += (energy_cnt * 10); //电能个数累加时扩大10倍，计算电能是除数扩大10倍，保证计算精度
            if (sg_runing_params.energy_unit > 0) {
                sg_runing_params.add_energy += sg_runing_params.energy / sg_runing_params.energy_unit; //单位是0.001度
                sg_runing_params.energy = sg_runing_params.energy % sg_runing_params.energy_unit;
            }
        }
        break;
    }
    if (1 == pivFlag[0])
        sg_runing_params.power = power / 100; //获取到的功率是以0.1W为单位
    if (1 == pivFlag[1])
        sg_runing_params.current = electricity; //电流单位为1mA
    if (1 == pivFlag[2])
        sg_runing_params.voltage = voltage / 100; //获取到的电压是以0.1V为单位
    /* 下面这个计算公式是hlw技术人员建议的 */
    if (sg_runing_params.power < 20) {
        sg_runing_params.power = 0;
        sg_runing_params.current = 0;
    } else if (sg_runing_params.power < 70) {
        if (sg_runing_params.voltage) {
            sg_runing_params.current = 1000 * sg_runing_params.power / sg_runing_params.voltage;
        }
    }
    return 1;
}

/**
 * @brief 帧数据接收
 *
 * @return TRUE 接收完成 FALSE 接收失败
 */
STATIC BOOL_T __frame_data_receive(VOID_T)
{
    BOOL_T analyze = FALSE;

    switch (sg_uart_recvstat) {
    case FLAME_HEAD1: {
        while (1) {
            if (1 == ty_uart_read_data(sg_hw_cfg.uart_no, sg_rev_buf, 1)) {
                if (__frame_head_check(sg_rev_buf, 0)) { // 校验head
                    if (1 == ty_uart_read_data(sg_hw_cfg.uart_no, &sg_rev_buf[1], 1)) {
                        if (__frame_head_check(sg_rev_buf, 1)) {
                            sg_uart_recvstat = RECVING;
                            break;
                        }
                    }
                }
            } else {
                goto __DAT_ERR;
            }
        }
    }
    case RECVING: {
        if (RECVING != sg_uart_recvstat)
            break;
        if (22 == ty_uart_read_data(sg_hw_cfg.uart_no, &sg_rev_buf[2], 22)) {
            if (sg_rev_buf[23] == __frame_check_sum_cal(&sg_rev_buf[2], 21)) {
                __frame_data_analysis(sg_rev_buf, 24);
                analyze = TRUE;
            }
            sg_uart_recvstat = FLAME_HEAD1;
        } else {
            goto __DAT_ERR;
        }
    } break;

    default:
        sg_uart_recvstat = FLAME_HEAD1;
        break;
    }

__DAT_ERR:
    sg_uart_recvstat = FLAME_HEAD1;
    ty_uart_read_data(sg_hw_cfg.uart_no, &sg_rev_buf[0], MAX_DATE_LEN);
    memset(sg_rev_buf, 0, MAX_DATE_LEN);

    return analyze;
}

/**
 * @brief hlw8012清运行参数
 *
 * @return VOID_T
 */
STATIC VOID_T hlw8032_run_params_init(VOID_T)
{
    memset(&sg_runing_params, 0x00, SIZEOF(sg_runing_params));

    return;
}

/**
 * @brief 8032读pvie数据
 *
 * @param[out] pvie_val pvie数据
 * @return VOID_T
 */
STATIC VOID_T __hlw8032_read_pvie_value(OUT TDL_ELE_ENERGY_PVIE_T *pvie_val)
{
    if (NULL == pvie_val) {
        return;
    }

    if (FALSE == __frame_data_receive()) {
        return;
    }

    pvie_val->power_val = sg_runing_params.power;
    pvie_val->volt_val = sg_runing_params.voltage;
    pvie_val->curr_val = sg_runing_params.current;
    pvie_val->energy_val = sg_runing_params.add_energy;
    sg_runing_params.add_energy = 0;

    return;
}

/**
 * @brief hlw8032控制
 *
 * @param[in] state TRUE 使能 FALSE 失败
 * @return VOID_T
 */
STATIC VOID_T __hlw8032_ctrl(BOOL_T state)
{
    if (state) {
        ty_uart_init(sg_hw_cfg.uart_no, TYU_RATE_4800, TYWL_8B, TYP_NONE, TYS_STOPBIT1, MAX_DATE_LEN, TRUE);
        hlw8032_run_params_init();
        sg_uart_recvstat = FLAME_HEAD1;
    } else {
        ty_uart_free(sg_hw_cfg.uart_no);
    }

    return;
}

/**
 * @brief 初始化校准参数
 *
 * @param[in] res 采样电阻 1mR/2mR
 * @param[in] vin 参考电压
 * @param[in] iin 参考电流
 * @param[in] pin 参考功率
 * @return
 */
STATIC VOID_T __hlw8032_coe_params_init(IN UCHAR_T res, IN UINT_T vin, IN UINT_T iin, IN UINT_T pin)
{
    if ((!vin) || (!iin) || (!pin) || (!res)) {
        return;
    }

    if (!res) {
        res = 1; // 不设置时, 默认1mR
        PR_ERR("sample resistance err, set resistance to the default value:%d", res);
    }

    sg_p_ref = pin;
    sg_v_ref = vin;
    sg_i_ref = iin;

    return;
}

/**
 * @brief 产测数据判断
 *
 * @return TDL_ELE_TEST_PASS 产测成功 TDL_ELE_TEST_FAIL 产测失败
 */
STATIC TDL_ELE_ENERGY_TEST_RESULT_E __hlw8032_fac_test_cal(VOID_T)
{
    PR_NOTICE("actual V:%d I:%d P:%d", sg_runing_params.voltage, sg_runing_params.current, sg_runing_params.power);

    PR_NOTICE("default V:%d I:%d P:%d ", sg_v_ref, sg_i_ref, sg_p_ref);

    if ((tdl_ele_energy_rang_judge(sg_runing_params.voltage, sg_v_ref, sg_factory_test.err_percent)) &&
        (tdl_ele_energy_rang_judge(sg_runing_params.power, sg_p_ref, sg_factory_test.err_percent))) {
        sg_coe_params.v_coe = sg_v_ref * sg_voltage_out;
        sg_coe_params.i_coe = sg_i_ref * sg_elec_out;
        sg_coe_params.p_coe = sg_p_ref * sg_power_out;
        sg_coe_params.e_coe = (36000000 / sg_p_ref / (sg_time_end - sg_time_start)) * sg_addplus;
        return TDL_ELE_TEST_PASS;
    }

    return TDL_ELE_TEST_FAIL;
}

/**
 * @brief 产测状态检测
 *
 * @param[in] timerID
 * @param[in] pTimerArg
 * @return VOID_T
 */
STATIC VOID_T __hlw8032_fac_test_timer_cb(UINT_T timerID, PVOID_T pTimerArg)
{
    if (sg_factory_test.sample_finished) {
        sg_factory_test.test_result = __hlw8032_fac_test_cal();
        sg_factory_test.test_finished = TRUE;
        sys_stop_timer(sg_factory_test.timer_id);
    }

    return;
}

/**
 * @brief hlw8032芯片初始化
 *
 * @param[in] drv_cfg 芯片配置参数
 * @param[in] platform_resource 硬件资源
 * @return OPRT_OK 代表成功。如果是其他错误码，请参考 tuya_error_code.h
 */
STATIC OPERATE_RET __hlw8032_chip_init(IN CONST TDL_ELE_ENERGY_CFG_T *drv_cfg, IN CONST VOID_T *platform_resource)
{
    if ((NULL == drv_cfg) || (NULL == platform_resource)) {
        PR_DEBUG("init info is null ptr");
        return OPRT_INVALID_PARM;
    }
    OPERATE_RET ret = OPRT_OK;

    /*全局数据初始化*/
    memset(&sg_coe_params, 0x00, SIZEOF(sg_coe_params));
    memset(&sg_hw_cfg, 0, SIZEOF(sg_hw_cfg));
    memcpy(&sg_hw_cfg, (TDD_HLW8032_DRV_RESOURCE_T *)platform_resource, SIZEOF(sg_hw_cfg));

    /*硬件资源初始化*/
    ty_uart_init(sg_hw_cfg.uart_no, TYU_RATE_4800, TYWL_8B, TYP_NONE, TYS_STOPBIT1, MAX_DATE_LEN, TRUE);

    /*运行参数初始化*/
    if (TDL_ELE_VOLT_120V == drv_cfg->params.volt_type) {
        __hlw8032_coe_params_init(drv_cfg->params.resval, TDL_ELE_V_REF_120, TDL_ELE_I_REF_120, TDL_ELE_P_REF_120);
    } else {
        __hlw8032_coe_params_init(drv_cfg->params.resval, TDL_ELE_V_REF_220, TDL_ELE_I_REF_220, TDL_ELE_P_REF_220);
    }
    hlw8032_run_params_init();

    if (TDL_ELE_TEST_MODE == drv_cfg->mode) {
        sg_factory_test.is_start_test = TRUE;
        sg_factory_test.test_result = TDL_ELE_TEST_FAIL;
        sg_factory_test.err_percent = drv_cfg->params.err_percent;
        if ((sg_factory_test.err_percent > 100) || (sg_factory_test.err_percent < 1)) {
            sg_factory_test.err_percent = 5; // 默认产测误差
        }
        PR_NOTICE("-->hlw8032 test err percent:%d", sg_factory_test.err_percent);
        ret = __drv_hw_timer_init(sg_hw_cfg.timer_no);
        if (OPRT_OK != ret) {
            PR_ERR("hlw8032 hw timer init err:%d", ret);
            return ret;
        }
        __hw_timer_ctrl(1, sg_hw_cfg.timer_no, TRUE);//1ms

        ret = sys_add_timer(__hlw8032_fac_test_timer_cb, NULL, &sg_factory_test.timer_id);
        if (OPRT_OK != ret) {
            PR_ERR("sg_factory_test timer init err:%d", ret);
            return ret;
        }
        sys_start_timer(sg_factory_test.timer_id, 1000, TIMER_CYCLE);
    } else {
        sg_factory_test.is_start_test = FALSE;
        sg_factory_test.test_result = drv_cfg->fac_test.test_result;
        if (TDL_ELE_TEST_PASS == sg_factory_test.test_result) {
            memcpy(&sg_coe_params, &drv_cfg->fac_test.coe_data, SIZEOF(sg_coe_params));
        } // 默认校准系数0
    }

    return OPRT_OK;
}

/**
 * @brief 读产测结果
 *
 * @return TRUE 产测结束 FALSE 产测未开始或产测未结束
 */
STATIC BOOL_T __hlw8032_read_fac_test_state(VOID_T)
{
    return sg_factory_test.test_finished;
}

/**
 * @brief 读出厂校准数据
 *
 * @param[out] fac_test_data 校准数据
 * @return VOID_T
 */
STATIC VOID_T __hlw8032_read_fac_cal_data(OUT TDL_ELE_ENERGY_FAC_TEST_T *fac_test_data)
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

STATIC CONST TDL_ELE_ENERGY_DRV_OPS_T __hlw8032_drv_ops = {
    .drv_init_cb = __hlw8032_chip_init,
    .drv_ctrl_cb = __hlw8032_ctrl,
    .drv_read_pvie_cb = __hlw8032_read_pvie_value,
    .drv_read_fac_test_data_cb = __hlw8032_read_fac_cal_data,
    .drv_read_fac_test_state_cb = __hlw8032_read_fac_test_state,
};

/**
 * @brief hlw8032驱动初始化
 *
 * @param[in] drv_cfg 芯片配置参数
 * @param[in] dev_info 平台资源信息
 * @return OPRT_OK 代表成功。如果是其他错误码，请参考 tuya_error_code.h
 */
OPERATE_RET tdd_ele_energy_hlw8032_init(IN CONST TDL_ELE_ENERGY_CFG_T *drv_cfg,
                                        IN CONST TDD_HLW8032_DRV_RESOURCE_T *dev_info)
{
    if ((NULL == drv_cfg) || (NULL == dev_info)) {
        return OPRT_INVALID_PARM;
    }

    TDL_ELE_ENERGY_DRIVER_T hlw8032_drv;
    memset(&hlw8032_drv, 0x00, SIZEOF(hlw8032_drv));

    hlw8032_drv.name = "hlw8032";
    hlw8032_drv.cfg = drv_cfg;
    hlw8032_drv.ops = &__hlw8032_drv_ops;
    hlw8032_drv.platform_resource = (CONST VOID_T *)dev_info;

    return tdl_ele_energy_driver_register(&hlw8032_drv);
}