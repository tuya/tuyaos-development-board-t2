/**
 * @file tdd_ele_energy_bl0937_hlw8012.c
 * @author www.tuya.com
 * @brief tdd_ele_energy_bl0937_hlw8012 module is used to
 * @version 1.0.0
 * @date 2022-05-12
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include <string.h>
#include "tuya_cloud_types.h"
#include "tuya_pin.h"
#include "tkl_timer.h"
#include "tkl_gpio.h"
#include "uni_log.h"
#include "uni_time_queue.h"
#include "sys_timer.h"

#include "tdd_ele_energy_bl0937_hlw8012.h"
#include "tdl_ele_energy_ops.h"

/***********************************************************
*************************micro define***********************
***********************************************************/
#define RES_VAL             1              // 定时1ms固定硬件定时ms
#define VI_CHANGE           TRUE           // 是否需要电压电流切换
#define SMALL_POWER_LIMIT   30             // 小功率屏蔽
#define ENERGY_ABNORMAL     10             // 单次采集周期电量飞走限制
#define E_1_KWH             (3600000 * 10) // 1度电量代表的能量,10是当前精度为0.1度
#define E_ENERGY_RESOLUTION 1000           // 1度电的千分之1，为应用层最小分辨率

// 产测万分之1度 大约（3.5秒-4.16秒），后面调整产测就是调整这个参数，最大放大2倍，可以缩小，放大只能在无切换功能下做
#define E_PROD_RESOLUTION 10000

// 这个是对产测数据的放大，（1-10），将来可满足应用层精度的变化
#define E_001E_GAIN (E_PROD_RESOLUTION / E_ENERGY_RESOLUTION)
//校正时间，记录在此时间内的脉冲数0.0001
//产测需要多少秒 多少ms 这个时间要大于2.5秒
//#define D_TIME1_CAL_TIME            ((E_1_KWH/E_PROD_RESOLUTION)/get_standard_pow()/RES_VAL)*1000)
#define D_TIME1_CAL_TIME   (((E_1_KWH / E_PROD_RESOLUTION) * 1000) / sg_p_ref / RES_VAL)
#define R_MEASUREIC_TYPE   0x01
#define R_MEASUREIC_U_RMS  (0 | (R_MEASUREIC_TYPE << 24))
#define R_MEASUREIC_I_RMS  (1 | (R_MEASUREIC_TYPE << 24))
#define R_MEASUREIC_PQ_RMS (2 | (R_MEASUREIC_TYPE << 24))
#define R_MEASUREIC_ENERGY (3 | (R_MEASUREIC_TYPE << 24))
#define R_MEASUREIC_ALL    (4 | (R_MEASUREIC_TYPE << 24))
// Time1定时器定时,时间基数 = 1ms
#define D_TIME1_20MS  20 / RES_VAL
#define D_TIME1_100MS 100 / RES_VAL
#define D_TIME1_150MS 150 / RES_VAL
#define D_TIME1_200MS 200 / RES_VAL
#define D_TIME1_300MS 300 / RES_VAL
#define D_TIME1_400MS 400 / RES_VAL
#define D_TIME1_500MS 500 / RES_VAL
#define D_TIME1_1S    1000 / RES_VAL // Time1定时器定时1S时间常数
#define D_TIME1_2S    2000 / RES_VAL
#define D_TIME1_3S    3000 / RES_VAL
#define D_TIME1_4S    4000 / RES_VAL
#define D_TIME1_5S    5000 / RES_VAL
#define D_TIME1_6S    6000 / RES_VAL
#define D_TIME1_8S    8000 / RES_VAL
#define D_TIME1_9S    9000 / RES_VAL
#define D_TIME1_10S   10000 / RES_VAL
#define D_TIME1_11S   11000 / RES_VAL
#define D_TIME1_20S   20000 / RES_VAL
//若不需要切换VI的管脚实际，检测周期V和P可以同步，同时V_CAL_VIR_TIME这个变量是为了开500ms的窗口，给应用层去计算
//且产测时间必须大于V_CAL_VIR_TIME，因为产测总时间是确定电能量换算系数，其他几个参变量是周期测试测试脉冲个数应用层放到
//校准系数中，产测总时间到的时候必须要以完成一轮获取P V I检测的内容
#if (VI_CHANGE == FALSE)
#define V_CAL_TIME       (D_TIME1_1S)                 //电压检测时间
#define P_CAL_CYCLE_TIME (V_CAL_TIME)                 //功率周期按照和和电压一致
#define V_CAL_VIR_TIME   (V_CAL_TIME + D_TIME1_500MS) //电压测试时间+应用调用掉换算系数时间
#else
#define I_CAL_TIME       D_TIME1_10S //电流检测时间，SEL拉低的时间
#define V_CAL_TIME       D_TIME1_1S  //电压检测时间，SEL拉高的时间
#define I_CYCLE_TIME     (D_TIME1_2S + D_TIME1_500MS)
#define P_CAL_CYCLE_TIME (D_TIME1_2S) //功率周期按照电压周期+1秒
#endif
#define CAL_SINGLE_TIME    D_TIME1_100MS
#define D_TIME1_V_OVERFLOW 500 / RES_VAL // Time1定时器,电压溢出常数设定为500mS,溢出说明脉宽周期大于500mS
#define D_TIME1_I_OVERFLOW 5000 / RES_VAL // Time1定时器,电流溢出常数设定为10S,溢出说明脉宽周期大于10S
#define D_TIME1_P_OVERFLOW 5000 / RES_VAL // Time1定时器,功率溢出常数设定为10S(约0.5W最小值),溢出说明脉宽周期大于10S

/*bl0937默认校准参数*/
#define TDD_ELE_BL0937_V_REF 586
#define TDD_ELE_BL0937_I_REF 28928
#define TDD_ELE_BL0937_P_REF 16929
#define TDD_ELE_BL0937_E_REF 2820

/*hlw8012默认校准参数*/
#define TDD_ELE_HLW8012_V_REF 1680
#define TDD_ELE_HLW8012_I_REF 34586
#define TDD_ELE_HLW8012_P_REF 98363
#define TDD_ELE_HLW8012_E_REF 424

#define my_abs(x, y) ((x) > (y) ? (x) - (y) : (y) - (x))
/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef enum {
    TDD_ELE_CHIP_BL0937 = 0,
    TDD_ELE_CHIP_HLW8012,
} TDD_BL0937_HW8012_CHIP_E;

typedef enum {
    TDD_ELE_MEASURE_VOLT = 0,
    TDD_ELE_MEASURE_CURR,
} TDD_BL0937_HW8012_MEASURE_E;

typedef struct {
    UCHAR_T err_percent;
    BOOL_T is_start_test;
    BOOL_T test_finished;
    BOOL_T sample_finished;
    TIMER_ID timer_id;
    TDL_ELE_ENERGY_TEST_RESULT_E test_result;
} TDD_BL0937_HW8012_FAC_TEST_T;

/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC USHORT_T sg_p_totaltimes;         //当前脉冲 功率测量总时间
STATIC USHORT_T sg_v_totaltimes;         //当前脉冲 电压测量总时间
STATIC USHORT_T sg_i_totaltimes;         //当前脉冲 电流测量总时间
STATIC USHORT_T sg_p_onecycle_times;     //功率测量时间参数
STATIC USHORT_T sg_v_onecycle_times;     //电压测量时间参数
STATIC USHORT_T sg_i_onecycle_times;     //电流测量时间参数
STATIC USHORT_T sg_p_lastonecycle_times; //功率测量时间参数，上一次数量值
STATIC USHORT_T sg_v_lastonecycle_times; //电压测量时间参数，上一次数量值
STATIC USHORT_T sg_i_lastonecycle_times; //电流测量时间参数，上一次数量值
STATIC USHORT_T sg_p_counts;             //功率测量脉冲数量
STATIC USHORT_T sg_v_counts;             //电压测量脉冲数量
STATIC USHORT_T sg_i_counts;             //电流测量脉冲数量
STATIC USHORT_T sg_p_lastcounts;         //功率测量脉冲数量，上一次数量值
STATIC USHORT_T sg_v_lastcounts;         //电压测量脉冲数量，上一次数量值
STATIC USHORT_T sg_i_lastcounts;         //电流测量脉冲数量，上一次数量值
STATIC BOOL_T sg_p_onecycle_mode;        //功率测量模式 1:单周期测量，0:1S定时测量
STATIC BOOL_T sg_v_onecycle_mode;
STATIC BOOL_T sg_i_onecycle_mode;
STATIC BOOL_T sg_p_onecycle_lastmode;
STATIC BOOL_T sg_v_onecycle_lastmode;
STATIC BOOL_T sg_i_onecycle_lastmode;
STATIC BOOL_T sg_p_overflow;     // 功率脉冲周期 溢出标志位
STATIC BOOL_T sg_v_overflow;     // 电压脉冲周期 溢出标志位
STATIC BOOL_T sg_i_overflow;     // 电流脉冲周期 溢出标志位
STATIC BOOL_T sg_p_lastoverflow; // 功率脉冲周期 溢出标志位
STATIC BOOL_T sg_v_lastoverflow; // 电压脉冲周期 溢出标志位
STATIC BOOL_T sg_i_lastoverflow; // 电流脉冲周期 溢出标志位
STATIC BOOL_T sg_vi_testmode;    // 1:电压测量模式;0:电流测量模式
STATIC USHORT_T sg_vi_testtimes;
//这部分采样值是换算后的值，需要通过对应接口传递给应用层
STATIC UINT_T sg_p_samplevalue;        //功率值 1000.0W
STATIC UINT_T sg_v_samplevalue;        //电压值 220.0V
STATIC UINT_T sg_i_samplevalue;        //电流值 4.545A
STATIC UINT_T sg_e_samplevalue;        //用电量   0.001度
STATIC UINT_T sg_isr_add_energy;       //用于中断中累计的0.001度
STATIC UINT_T sg_pluse_cnt;            //脉冲个数寄存器
STATIC UINT_T sg_calc_times;           //校正时间
STATIC UINT_T sg_p_current_plusewidth; //当前功率 脉冲周期
STATIC UINT_T sg_v_current_plusewidth; //当前电压 脉冲周期
STATIC UINT_T sg_i_current_plusewidth; //当前电流 脉冲周期
STATIC USHORT_T sg_p_ref;              //参考功率值,如以1000W校正。1000.0W
STATIC USHORT_T sg_v_ref;              //参考电压  220.0V
STATIC USHORT_T sg_i_ref;              //参考电流  1000W,220V条件下是4.545A

STATIC BOOL_T sg_is_sample_finish = FALSE;
STATIC TDD_BL0937_HLW8012_DRV_RESOURCE_T sg_hw_cfg;
STATIC TDL_ELE_ENERGY_COE_T sg_coe_params;
STATIC TDL_ELE_ENERGY_COE_T sg_def_coe_params;
STATIC TDD_BL0937_HW8012_CHIP_E sg_chipid;
STATIC TDD_BL0937_HW8012_FAC_TEST_T sg_factorytest;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief 电压电流测量类型切换
 *
 * @param[in] masure_type 测量类型
 * @return OPRT_OK 代表成功。如果是其他错误码，请参考 tuya_error_code.h
 */
STATIC OPERATE_RET __voltage_curr_measure_ctrl(IN TDD_BL0937_HW8012_MEASURE_E masure_type)
{
    OPERATE_RET ret = OPRT_OK;
    TUYA_GPIO_LEVEL_E stat = TUYA_GPIO_LEVEL_HIGH;

    switch (masure_type) {
    case TDD_ELE_MEASURE_VOLT: {
        stat = ((sg_hw_cfg.ivcpin.type == TDL_ELE_IO_DRV_HIGH) ? TUYA_GPIO_LEVEL_HIGH : TUYA_GPIO_LEVEL_LOW);
    } break;

    case TDD_ELE_MEASURE_CURR: {
        stat = ((sg_hw_cfg.ivcpin.type == TDL_ELE_IO_DRV_HIGH) ? TUYA_GPIO_LEVEL_LOW : TUYA_GPIO_LEVEL_HIGH);
    } break;

    default:
        break;
    }

    ret = tkl_gpio_write(sg_hw_cfg.ivcpin.pin, stat);
    if (OPRT_OK != ret) {
        PR_ERR("vi sel gpio write err:%d", ret);
        return ret;
    }

    return OPRT_OK;
}

/**
 * @brief 硬件定时回调
 *
 * @return VOID_T
 */
STATIC VOID_T __hw_timer_cb(VOID_T *args)
{
    if (sg_factorytest.is_start_test) {
        sg_calc_times++;
        if (sg_calc_times == D_TIME1_CAL_TIME) {
            sg_coe_params.e_coe = E_001E_GAIN * sg_pluse_cnt; //记录校正时间内的脉冲数，此脉冲数表示0.0005度用电量
            sg_factorytest.sample_finished = TRUE;
        }
    }

    //功率测量
    if (sg_p_counts != 0) {
        sg_p_onecycle_times++;
        sg_p_totaltimes++;
    }

    if (sg_p_totaltimes >= D_TIME1_P_OVERFLOW) {
        sg_p_overflow = TRUE; //溢出，
        sg_p_lastoverflow = sg_p_overflow;
        //清状态参数,重新开始测试
        sg_p_totaltimes = 0; //清溢出寄存器
        sg_p_onecycle_times = 0;
        sg_p_counts = 0;        //等待下一次中断开始计数
        sg_p_onecycle_mode = 0; //初始化为计数脉冲测量模式
    } else if (sg_p_onecycle_times == CAL_SINGLE_TIME) {
        if (sg_p_counts < 2) {
            // 100ms内只有一次中断，说明周期>100ms,采用单周期测量模式
            sg_p_onecycle_mode = 1;
        } else {
            // 100ms内有2次或以上数量脉冲，说明周期<100ms，采用计数脉冲测量模式
            sg_p_onecycle_mode = 0;
        }
    }

    //电压、电流测量
    if (sg_vi_testmode == 1) {
        //电压测量
        if (sg_v_counts != 0) {
            sg_v_onecycle_times++;
            sg_v_totaltimes++;
        }

        if (sg_v_totaltimes >= D_TIME1_V_OVERFLOW) {
            sg_v_overflow = TRUE;
            sg_v_lastoverflow = sg_v_overflow;
            //清状态参数,重新开始测试
            sg_v_totaltimes = 0; //清溢出寄存器
            sg_v_onecycle_times = 0;
            sg_v_counts = 0;
            sg_v_onecycle_mode = 0; //初始化为计数脉冲测量模式
        } else if (sg_v_onecycle_times == CAL_SINGLE_TIME) {
            if (sg_v_counts < 2) {
                // 100ms内只有一次中断，说明周期>100ms,采用单周期测量模式
                sg_v_onecycle_mode = 1;
            } else {
                // 100ms内有2次或以上数量脉冲，说明周期<100ms，采用计数脉冲测量模式
                sg_v_onecycle_mode = 0;
            }
        }
    } else {
        //电流测量
        if (sg_i_counts != 0) {
            sg_i_onecycle_times++;
            sg_i_totaltimes++;
        }

        if (sg_i_totaltimes >= D_TIME1_I_OVERFLOW) {
            sg_i_overflow = TRUE;
            sg_i_lastoverflow = sg_i_overflow;
            //清状态参数,重新开始测试
            sg_i_totaltimes = 0; //清溢出寄存器
            sg_i_onecycle_times = 0;
            sg_i_counts = 0;
            sg_i_onecycle_mode = 0; //初始化为计数脉冲测量模式
        } else if (sg_i_onecycle_times == CAL_SINGLE_TIME) {
            if (sg_i_counts < 2) {
                // 100ms内只有一次中断，说明周期>100ms,采用单周期测量模式
                sg_i_onecycle_mode = 1;
            } else {
                // 100ms内有2次或以上数量脉冲，说明周期<100ms，采用计数脉冲测量模式
                sg_i_onecycle_mode = 0;
            }
        }
    }

#if (VI_CHANGE == TRUE)
    //电压、电流测量模式切换  B_VI_Test_Mode:(1:电压测量模式) (0:电流测试模式)
    sg_vi_testtimes--;
    if (sg_vi_testmode == 1) {
        if (sg_vi_testtimes == V_CAL_TIME) { //此条件不会触发,电流电压功率数据同时更新
            sg_is_sample_finish = TRUE;
        }
    } else {
        if (sg_vi_testtimes == I_CAL_TIME - I_CYCLE_TIME) {
            sg_is_sample_finish = TRUE;
        }
    }

    if (sg_vi_testtimes == 0) {
        sg_is_sample_finish = FALSE;
        if (sg_vi_testmode == 1) {
            //转为电流测量模式
            sg_vi_testmode = 0;
            __voltage_curr_measure_ctrl(TDD_ELE_MEASURE_CURR);
            sg_vi_testtimes = I_CAL_TIME;
            //清状态参数
            sg_i_totaltimes = 0;
            sg_i_onecycle_times = 0;
            sg_i_counts = 0;
            sg_i_overflow = FALSE;
        } else {
            //转为电压测量模式
            sg_vi_testmode = 1;

            __voltage_curr_measure_ctrl(TDD_ELE_MEASURE_VOLT);

            sg_vi_testtimes = V_CAL_TIME;
            //清状态参数
            sg_v_totaltimes = 0;
            sg_v_onecycle_times = 0;
            sg_v_counts = 0;
            sg_v_overflow = FALSE;
        }
    }
#else
    sg_vi_testtimes--;
    if (sg_vi_testmode == 1) {
        if (sg_vi_testtimes == (V_CAL_VIR_TIME - V_CAL_TIME)) { //此条件不会触发,电流电压功率数据同时更新
            sg_is_sample_finish = TRUE;
        }
    }

    if (sg_vi_testtimes == 0) {
        sg_is_sample_finish = FALSE;

        sg_vi_testtimes = V_CAL_VIR_TIME;
        //清状态参数
        sg_v_totaltimes = 0;
        sg_v_onecycle_times = 0;
        sg_v_counts = 0;
        sg_v_overflow = FALSE;
    }
#endif

    return;
}

/**
 * @brief 功率gpio外部中断回调
 *
 * @param[in] args
 * @return VOID_T
 */
STATIC VOID_T __power_gpio_isr_cb(VOID_T *args)
{
    sg_p_totaltimes = 0; //完成一次有效的测量，溢出寄存器清零
    sg_p_counts++;
    if (sg_p_overflow == TRUE) {
        //从溢出模式转入,开始测量
        sg_p_onecycle_mode = 0;  //初始化为计数脉冲测量模式
        sg_p_totaltimes = 0;     //清溢出寄存器清零
        sg_p_onecycle_times = 0; //清测量寄存器
        sg_p_counts = 1;
        sg_p_overflow = FALSE; //清溢出标志位
    } else {
        if (sg_p_onecycle_mode == 1) {
            if (sg_p_onecycle_times >= CAL_SINGLE_TIME) {
                //单周期测量模式
                sg_p_lastonecycle_times = sg_p_onecycle_times;
                sg_p_onecycle_lastmode = sg_p_onecycle_mode;
                sg_p_overflow = FALSE; //溢出标志位清零
                sg_p_lastoverflow = sg_p_overflow;
                //清状态参数,重新开始测试
                sg_p_onecycle_mode = 0;  //初始化为计数脉冲测量模式
                sg_p_totaltimes = 0;     //完成一次有效的测量，溢出寄存器清零
                sg_p_onecycle_times = 0; //清测量寄存器
                sg_p_counts = 1;
            }
        } else {
            if (sg_p_onecycle_times >= P_CAL_CYCLE_TIME) {
                sg_p_lastonecycle_times = sg_p_onecycle_times;
                sg_p_lastcounts = sg_p_counts;
                sg_p_onecycle_lastmode = sg_p_onecycle_mode;
                sg_p_overflow = FALSE; //溢出标志位清零
                sg_p_lastoverflow = sg_p_overflow;
                //清状态参数,重新开始测试
                sg_p_onecycle_mode = 0;  //初始化为计数脉冲测量模式
                sg_p_totaltimes = 0;     //完成一次有效的测量，溢出寄存器清零
                sg_p_onecycle_times = 0; //清测量寄存器
                sg_p_counts = 1;
            }
        }
    }
    //记录单位时间内的用电量
    sg_pluse_cnt++;

    //用电量计量，每0.1度电，用电量寄存器增加0.1度
    if (FALSE == sg_factorytest.is_start_test) {
        if (sg_pluse_cnt == sg_coe_params.e_coe) {
            sg_pluse_cnt = 0;
            sg_isr_add_energy++;
        }
    }

    return;
}

/**
 * @brief 电压、电流gpio外部中断回调
 *
 * @param[in] args
 * @return VOID_T
 */
STATIC VOID_T __vol_curr_gpio_isr_cb(VOID_T *args)
{
    //电压测试模式
    if (sg_vi_testmode == 1) {
        sg_v_totaltimes = 0;
        sg_v_counts++;
        if (sg_v_overflow == TRUE) {
            //从溢出模式转入,开始测量
            sg_v_onecycle_mode = 0;  //初始化为计数脉冲测量模式
            sg_v_totaltimes = 0;     //清溢出寄存器清零
            sg_v_onecycle_times = 0; //清测量寄存器
            sg_v_counts = 1;
            sg_v_overflow = FALSE; //清溢出标志位
        } else {
            if (sg_v_onecycle_mode == 1) {
                if (sg_v_onecycle_times >= CAL_SINGLE_TIME) {
                    //单周期测量模式
                    sg_v_lastonecycle_times = sg_v_onecycle_times;
                    sg_v_onecycle_lastmode = sg_v_onecycle_mode;
                    sg_v_overflow = FALSE; //溢出标志位清零
                    sg_v_lastoverflow = sg_v_overflow;
                    //清状态参数,重新开始测试
                    sg_v_onecycle_mode = 0;  //初始化为计数脉冲测量模式
                    sg_v_totaltimes = 0;     //完成一次有效的测量，溢出寄存器清零
                    sg_v_onecycle_times = 0; //清测量寄存器
                    sg_v_counts = 1;
                }
            } else {
                if (sg_v_onecycle_times >= D_TIME1_300MS) {
                    sg_v_lastonecycle_times = sg_v_onecycle_times;
                    sg_v_lastcounts = sg_v_counts;
                    sg_v_onecycle_lastmode = sg_v_onecycle_mode;
                    sg_v_overflow = FALSE; //溢出标志位清零
                    sg_v_lastoverflow = sg_v_overflow;
                    //清状态参数,重新开始测试
                    sg_v_onecycle_mode = 0;  //初始化为计数脉冲测量模式
                    sg_v_totaltimes = 0;     //完成一次有效的测量，溢出寄存器清零
                    sg_v_onecycle_times = 0; //清测量寄存器
                    sg_v_counts = 1;
                    sg_v_overflow = FALSE; //溢出标志位清零
                }
            }
        }
    }

    //电流测试模式
    if (sg_vi_testmode == 0) {
        sg_i_totaltimes = 0;
        sg_i_counts++;
        if (sg_i_overflow == TRUE) {
            //从溢出模式转入,开始测量
            sg_i_onecycle_mode = 0;  //初始化为计数脉冲测量模式
            sg_i_totaltimes = 0;     //清溢出寄存器清零
            sg_i_onecycle_times = 0; //清测量寄存器
            sg_i_counts = 1;
            sg_i_overflow = FALSE; //清溢出标志位
        } else {
            if (sg_i_onecycle_mode == 1) {
                if (sg_i_onecycle_times >= D_TIME1_100MS) {
                    //单周期测量模式
                    sg_i_lastonecycle_times = sg_i_onecycle_times;
                    sg_i_onecycle_lastmode = sg_i_onecycle_mode;
                    sg_i_overflow = FALSE; //溢出标志位清零
                    sg_i_lastoverflow = sg_i_overflow;
                    //清状态参数,重新开始测试
                    sg_i_onecycle_mode = 0;  //初始化为计数脉冲测量模式
                    sg_i_totaltimes = 0;     //完成一次有效的测量，溢出寄存器清零
                    sg_i_onecycle_times = 0; //清测量寄存器
                    sg_i_counts = 1;
                }
            } else {
                if (sg_i_onecycle_times >= D_TIME1_1S) {
                    sg_i_lastonecycle_times = sg_i_onecycle_times;
                    sg_i_lastcounts = sg_i_counts;
                    sg_i_onecycle_lastmode = sg_i_onecycle_mode;
                    sg_i_overflow = FALSE; //溢出标志位清零
                    sg_i_lastoverflow = sg_i_overflow;
                    //清状态参数,重新开始测试
                    sg_i_onecycle_mode = 0;  //初始化为计数脉冲测量模式
                    sg_i_totaltimes = 0;     //完成一次有效的测量，溢出寄存器清零
                    sg_i_onecycle_times = 0; //清测量寄存器
                    sg_i_counts = 1;
                }
            }
        }
    }

    return;
}

/**
 * @brief 输出gpio初始化
 *
 * @param[in] out_pin gpio配置信息
 * @return OPRT_OK 代表成功。如果是其他错误码，请参考 tuya_error_code.h
 */
STATIC OPERATE_RET __drv_out_pin_init(IN CONST TDL_ELE_ENERGY_IO_CFG_T *out_pin)
{
    if (NULL == out_pin) {
        PR_ERR("out_pin is null ptr");
        return OPRT_INVALID_PARM;
    }

    if (TDL_ELE_IO_DRV_ERR == out_pin->type) {
        PR_ERR("invalid io type");
        return OPRT_INVALID_PARM;
    }
    TUYA_GPIO_BASE_CFG_T gpio_cfg;
    memset(&gpio_cfg, 0, SIZEOF(gpio_cfg));

    gpio_cfg.mode = TUYA_GPIO_PUSH_PULL;
    gpio_cfg.direct = TUYA_GPIO_OUTPUT;

    /*默认先采样电压*/
    if (TDL_ELE_IO_DRV_HIGH == out_pin->type) {
        gpio_cfg.level = TUYA_GPIO_LEVEL_HIGH;
    } else if (TDL_ELE_IO_DRV_LOW == out_pin->type) {
        gpio_cfg.level = TUYA_GPIO_LEVEL_LOW;
    } else {
        PR_ERR("gpio pin type err:%d", out_pin->type);
        return OPRT_INVALID_PARM;
    }

    /*初始化电压电流切换引脚*/
    OPERATE_RET ret = tkl_gpio_init(out_pin->pin, &gpio_cfg);
    if (OPRT_OK != ret) {
        PR_ERR("vi sel gpio inout set err:%d", ret);
        return ret;
    }
    /*默认先采样电压*/
    /*ret = tuya_gpio_write(out_pin->pin, ((out_pin->type == TDL_ELE_IO_DRV_HIGH) ? TRUE : FALSE));
    if (OPRT_OK != ret) {
        PR_ERR("vi sel gpio write err:%d", ret);
        return ret;
    }*/

    return OPRT_OK;
}

/**
 * @brief gpio外部中断初始化
 *
 * @param[in] epin 功率采样引脚
 * @param[in] ivpin 电压电流采样引脚
 * @return OPRT_OK 代表成功。如果是其他错误码，请参考 tuya_error_code.h
 */
STATIC OPERATE_RET __drv_gpio_isr_init(IN UCHAR_T epin, IN UCHAR_T ivpin)
{
    OPERATE_RET ret = OPRT_OK;
    TUYA_GPIO_IRQ_T gpio_isr;

    memset(&gpio_isr, 0, SIZEOF(gpio_isr));
    gpio_isr.mode = TUYA_GPIO_IRQ_FALL;
    gpio_isr.arg = NULL;

    gpio_isr.cb = __vol_curr_gpio_isr_cb;
    ret = tkl_gpio_irq_init(ivpin, &gpio_isr);
    if (OPRT_OK != ret) {
        return ret;
    }

    gpio_isr.cb = __power_gpio_isr_cb;
    ret = tkl_gpio_irq_init(epin, &gpio_isr);
    if (OPRT_OK != ret) {
        return ret;
    }
    PR_DEBUG("pwr[%d], vi[%d] isr init success", epin, ivpin);

    return OPRT_OK;
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
 * @brief gpio外部中断控制
 *
 * @param[in] gpio_pin gpio引脚
 * @param[in] isr_stat gpio中断状态，true：开 false: 关
 * @return VOID_T
 */
STATIC VOID_T __gpio_isr_ctrl(IN UCHAR_T gpio_pin, IN BOOL_T isr_stat)
{
    if (isr_stat) {
        tkl_gpio_irq_enable(gpio_pin);
    } else {
        tkl_gpio_irq_disable(gpio_pin);
    }
    PR_DEBUG("gpio_pin:%d isr %s", gpio_pin, ((isr_stat == TRUE) ? "open" : "closed"));

    return;
}

/**
 * @brief 读原始采样数据
 *
 * @param[out] src 读到的数据
 * @param[in] addr 读数据地址
 * @param[in] len 读数据长度
 * @return OPRT_OK 代表成功。如果是其他错误码，请参考 tuya_error_code.h
 */
STATIC OPERATE_RET __read_measure_origin_data(OUT UINT_T *src, IN UINT_T addr, IN UINT_T len)
{
    if (NULL == src) {
        return OPRT_INVALID_PARM;
    }

    switch (addr) {
    case R_MEASUREIC_U_RMS: {
        if (sg_v_onecycle_lastmode == 1) {
            *src = sg_v_lastonecycle_times * 1000;
        } else {
            if (sg_v_lastcounts < 2) {
                return OPRT_COM_ERROR;
            }
            *src = sg_v_lastonecycle_times * 1000 / (sg_v_lastcounts - 1);
        }
        if (sg_v_lastoverflow == TRUE) {
            *src = 0;
        }
    } break;

    case R_MEASUREIC_I_RMS: {
        if (sg_i_onecycle_lastmode == 1) {
            *src = sg_i_lastonecycle_times * 1000;
        } else {
            if (sg_i_lastcounts < 2) {
                return OPRT_COM_ERROR;
            }
            *src = sg_i_lastonecycle_times * 1000 / (sg_i_lastcounts - 1);
        }
        if (sg_i_lastoverflow == TRUE) {
            *src = 0;
        }
    } break;

    case R_MEASUREIC_PQ_RMS: {
        if (sg_p_onecycle_lastmode == 1) {
            *src = sg_p_lastonecycle_times * 1000;
        } else {
            if (sg_p_lastcounts < 2) {
                return OPRT_COM_ERROR;
            }
            *src = sg_p_lastonecycle_times * 1000 / (sg_p_lastcounts - 1);
        }
        if (sg_p_lastoverflow == TRUE) {
            *src = 0;
        }
    } break;

    case R_MEASUREIC_ENERGY: {
        *src = sg_isr_add_energy;
        sg_isr_add_energy = 0;
    } break;

    default:
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
}

/**
 * @brief 读当前电量
 *
 * @return VOID_T
 */
STATIC VOID_T __read_energy_measure_value(VOID_T)
{
    __read_measure_origin_data(&sg_e_samplevalue, R_MEASUREIC_ENERGY, 4);

    return;
}

/**
 * @brief 读当前功率值
 *
 * @return VOID_T
 */
STATIC VOID_T __read_power_measure_value(VOID_T)
{
    __read_measure_origin_data(&sg_p_current_plusewidth, R_MEASUREIC_PQ_RMS, 4);

    if (sg_factorytest.is_start_test) {
        sg_coe_params.p_coe = sg_p_current_plusewidth; // 校正时取U32_P_CURRENT_PLUSEWIDTH_TIME参数作为参考值
        return;
    }
    if (sg_p_current_plusewidth == 0) {
        sg_p_samplevalue = 0;
    } else {
        sg_p_samplevalue = sg_p_ref * sg_coe_params.p_coe / sg_p_current_plusewidth;
    }

    /*开机时U32_P_CURRENT_PLUSEWIDTH_TIME = 0，计算溢出*/
    if (0xffffffff == sg_p_samplevalue) {
        sg_p_samplevalue = 0;
    }

    return;
}

/**
 * @brief 读当前电流值
 *
 * @return VOID_T
 */
STATIC VOID_T __read_current_measure_value(VOID_T)
{
    __read_measure_origin_data(&sg_i_current_plusewidth, R_MEASUREIC_I_RMS, 4);

    if (sg_factorytest.is_start_test) {
        sg_coe_params.i_coe = sg_i_current_plusewidth; // 校正时取U32_V_CURRENT_PLUSEWIDTH_TIME参数作为参考值
        return;
    }
    if (sg_i_current_plusewidth == 0) {
        sg_i_samplevalue = 0;
    } else {
        sg_i_samplevalue = sg_i_ref * sg_coe_params.i_coe / sg_i_current_plusewidth;
    }

    if (0 == sg_p_samplevalue) {
        sg_i_samplevalue = 0;
    }

    return;
}

/**
 * @brief 读当前电压值
 *
 * @return VOID_T
 */
STATIC VOID_T __read_voltage_measure_value(VOID_T)
{
    __read_measure_origin_data(&sg_v_current_plusewidth, R_MEASUREIC_U_RMS, 4);

    if (sg_factorytest.is_start_test) {
        sg_coe_params.v_coe = sg_v_current_plusewidth; // 校正时取UINT_T_V_Period参数作为参考值
        return;
    }
    if (sg_v_current_plusewidth == 0) {
        sg_v_samplevalue = 0;
    } else {
        sg_v_samplevalue = sg_v_ref * sg_coe_params.v_coe / sg_v_current_plusewidth;
    }

    /*开机时U32_V_CURRENT_PLUSEWIDTH_TIME = 0，计算溢出*/
    if (0xffffffff == sg_v_samplevalue) {
        sg_v_samplevalue = 0;
    }

    return;
}

/**
 * @brief 测量数据处理
 *
 * @return VOID_T
 */
STATIC VOID_T __deal_measure_value(VOID_T)
{
    //潜动判断
    if (sg_p_samplevalue < SMALL_POWER_LIMIT) {
        sg_p_samplevalue = 0;
        sg_i_samplevalue = 0;
    }

    //防飞走，电量新增很快
    if (sg_e_samplevalue > ENERGY_ABNORMAL) {
        sg_e_samplevalue = 0;
    }

#if (VI_CHANGE == FALSE)
    sg_i_samplevalue = 1000 * sg_p_samplevalue / sg_v_samplevalue;
#endif

    return;
}

/**
 * @brief bl0937_hlw8012出厂校准测试
 *
 * @return TDL_ELE_TEST_PASS 出厂校准成功 TDL_ELE_TEST_FAIL 出厂校准失败
 */
STATIC TDL_ELE_ENERGY_TEST_RESULT_E __bl0937_hlw8012_fac_test_cal(VOID_T)
{
    TDL_ELE_ENERGY_TEST_RESULT_E test_ret = TDL_ELE_TEST_PASS;

    PR_NOTICE("actual V:%d I:%d P:%d E:%d", sg_coe_params.v_coe, sg_coe_params.i_coe, sg_coe_params.p_coe,
              sg_coe_params.e_coe);

    PR_NOTICE("default V:%d I:%d P:%d E:%d", sg_def_coe_params.v_coe, sg_def_coe_params.i_coe, sg_def_coe_params.p_coe,
              sg_def_coe_params.e_coe);

    if (my_abs(sg_coe_params.v_coe, sg_def_coe_params.v_coe) >
        (sg_def_coe_params.v_coe * sg_factorytest.err_percent / 100)) {
        test_ret = TDL_ELE_TEST_FAIL;
    } else if (my_abs(sg_coe_params.p_coe, sg_def_coe_params.p_coe) >
               (sg_def_coe_params.p_coe * sg_factorytest.err_percent / 100)) {
        test_ret = TDL_ELE_TEST_FAIL;
    } else if (my_abs(sg_coe_params.e_coe, sg_def_coe_params.e_coe) >
               (sg_def_coe_params.e_coe * sg_factorytest.err_percent / 100)) {
        test_ret = TDL_ELE_TEST_FAIL;
    } else {
#if (VI_CHANGE == TRUE)
        if (my_abs(sg_coe_params.i_coe, sg_def_coe_params.i_coe) >
            (sg_def_coe_params.i_coe * sg_factorytest.err_percent / 100)) {
            test_ret = TDL_ELE_TEST_FAIL;
        }
#endif
    }

    return test_ret;
}

/**
 * @brief 定时检测产测状态
 *
 * @param[in] timerID
 * @param[in] pTimerArg
 * @return VOID_T
 */
STATIC VOID_T __bl0937_hlw8012_fac_test_timer_cb(IN UINT_T timerID, IN PVOID_T pTimerArg)
{
    if (sg_factorytest.sample_finished) {
        sg_factorytest.test_result = __bl0937_hlw8012_fac_test_cal();
        sg_factorytest.test_finished = TRUE;
        sys_stop_timer(sg_factorytest.timer_id);
    }

    return;
}

/**
 * @brief 读出厂校准数据
 *
 * @param[out] fac_test_data 校准数据
 * @return VOID_T
 */
STATIC VOID_T __bl0937_hlw8012_read_fac_test_data(OUT TDL_ELE_ENERGY_FAC_TEST_T *fac_test_data)
{
    if (NULL == fac_test_data) {
        PR_ERR("fac test data ptr null");
        return;
    }

    fac_test_data->test_result = sg_factorytest.test_result;
    fac_test_data->coe_data.v_coe = sg_coe_params.v_coe;
    fac_test_data->coe_data.i_coe = sg_coe_params.i_coe;
    fac_test_data->coe_data.p_coe = sg_coe_params.p_coe;
    fac_test_data->coe_data.e_coe = sg_coe_params.e_coe;

    return;
}

/**
 * @brief 读产测进行状态
 *
 * @return TRUE 产测结束 FALSE 产测失败
 */
STATIC BOOL_T __bl0937_hlw8012_read_fac_test_state(VOID_T)
{
    return sg_factorytest.test_finished;
}

/**
 * @brief 初始化校准参数
 *
 * @param[in] res 采样电阻阻值,1mR或2mR
 * @param[in] vin 校准电压参考值
 * @param[in] iin 校准电流参考值
 * @param[in] pin 校准功率参考值
 * @return
 */
STATIC VOID_T __bl0937_hlw8012_coe_value_init(IN UCHAR_T res, IN UINT_T vin, IN UINT_T iin, IN UINT_T pin)
{
    //入参判断
    if ((!vin) || (!iin) || (!pin)) {
        return;
    }

    if ((!res)) {
        res = 1;
        PR_ERR("sample resistance err, set resistance to the default value:%d", res);
    }

    if (TDD_ELE_CHIP_BL0937 == sg_chipid) {
        sg_def_coe_params.v_coe = TDD_ELE_BL0937_V_REF;
        sg_def_coe_params.i_coe = TDD_ELE_BL0937_I_REF;
        sg_def_coe_params.p_coe = TDD_ELE_BL0937_P_REF;
        sg_def_coe_params.e_coe = TDD_ELE_BL0937_E_REF;
    } else {
        sg_def_coe_params.v_coe = TDD_ELE_HLW8012_V_REF;
        sg_def_coe_params.i_coe = TDD_ELE_HLW8012_I_REF;
        sg_def_coe_params.p_coe = TDD_ELE_HLW8012_P_REF;
        sg_def_coe_params.e_coe = TDD_ELE_HLW8012_E_REF;
    }

    sg_def_coe_params.v_coe = (sg_def_coe_params.v_coe) * TDL_ELE_V_REF_220 / vin;
    sg_def_coe_params.i_coe = (sg_def_coe_params.i_coe) * TDL_ELE_I_REF_220 / iin / res;
    sg_def_coe_params.p_coe = (sg_def_coe_params.p_coe) * TDL_ELE_P_REF_220 / pin / res;
    sg_def_coe_params.e_coe = (sg_def_coe_params.e_coe) * res;

    sg_p_ref = pin;
    sg_v_ref = vin;
    sg_i_ref = iin;

    return;
}

/**
 * @brief 初始化运行参数
 *
 * @return VOID_T
 */
STATIC VOID_T __bl0937_hlw8012_run_params_init(VOID_T)
{
    sg_pluse_cnt = 0; //高频脉冲个数

    sg_isr_add_energy = 0; //中断中计算出的0.001读个数

    sg_p_samplevalue = 0;
    sg_v_samplevalue = 0;
    sg_i_samplevalue = 0;
    sg_e_samplevalue = 0; //经过滤波处理的0.001度电

    sg_p_totaltimes = 0;
    sg_v_totaltimes = 0;
    sg_i_totaltimes = 0;

    sg_p_onecycle_times = 0;
    sg_v_onecycle_times = 0;
    sg_i_onecycle_times = 0;
    sg_p_lastonecycle_times = 0;
    sg_v_lastonecycle_times = 0;
    sg_i_lastonecycle_times = 0;

    sg_p_counts = 0;
    sg_v_counts = 0;
    sg_i_counts = 0;

    sg_p_lastcounts = 0;
    sg_v_lastcounts = 0;
    sg_i_lastcounts = 0;

    //初始化单周期测量模式
    sg_p_onecycle_mode = 1;
    sg_v_onecycle_mode = 1;
    sg_i_onecycle_mode = 1;
    sg_p_onecycle_lastmode = 1;
    sg_v_onecycle_lastmode = 1;
    sg_i_onecycle_lastmode = 1;

    //开始测量，置溢出标志位为1
    sg_p_overflow = 1;
    sg_v_overflow = 1;
    sg_i_overflow = 1;

    sg_p_lastoverflow = 1;
    sg_v_lastoverflow = 1;
    sg_i_lastoverflow = 1;

    //上电初始化为电压测试模式
    sg_vi_testmode = 1;

    sg_vi_testtimes = V_CAL_TIME;

    //设置默认值
    sg_calc_times = 0;

    sg_p_current_plusewidth = 0; //当前功率 脉冲周期
    sg_v_current_plusewidth = 0; //当前电压 脉冲周期
    sg_i_current_plusewidth = 0; //当前电流 脉冲周期

    sg_is_sample_finish = FALSE;
    PR_DEBUG("prod time is %d", D_TIME1_CAL_TIME);

    return;
}

/**
 * @brief 计量芯片开关控制
 *
 * @param[in] state TRUE 使能芯片采样 FALSE 关闭芯片采样
 * @return VOID_T
 */
STATIC VOID_T __bl0937_hlw8012_ctrl(IN BOOL_T state)
{
    __hw_timer_ctrl(RES_VAL, sg_hw_cfg.timer_no, state);
    if (state) {
        __drv_gpio_isr_init(sg_hw_cfg.epin, sg_hw_cfg.ivpin);
    }
    __gpio_isr_ctrl(sg_hw_cfg.ivpin, state);
    __gpio_isr_ctrl(sg_hw_cfg.epin, state);

    if (state) {
        __bl0937_hlw8012_run_params_init();
    }

    return;
}

/**
 * @brief 获取pvie的值
 *
 * @param[out] pvie_val 获取到的pvie值
 * @return VOID_T
 */
STATIC VOID_T __bl0937_hlw8012_read_pvie_value(OUT TDL_ELE_ENERGY_PVIE_T *pvie_val)
{
    if ((NULL == pvie_val) || (FALSE == sg_is_sample_finish)) {
        return;
    }

    __read_power_measure_value();
    __read_voltage_measure_value();
    __read_current_measure_value();
    __read_energy_measure_value();
    __deal_measure_value();

    pvie_val->power_val = sg_p_samplevalue;
    pvie_val->volt_val = sg_v_samplevalue;
    pvie_val->curr_val = sg_i_samplevalue;
    pvie_val->energy_val = sg_e_samplevalue;
    sg_e_samplevalue = 0; // 上层已取，清除底层累计电量

    return;
}

/**
 * @brief bl0937、hw8012硬件初始化
 *
 * @param[in] chip_id 芯片id
 * @param[in] drv_cfg 驱动配置
 * @param[in] dev_info 平台资源
 * @return OPRT_OK 代表成功。如果是其他错误码，请参考 tuya_error_code.h
 */
STATIC OPERATE_RET __bl0937_hlw8012_hw_init(IN TDD_BL0937_HW8012_CHIP_E chip_id, IN CONST TDL_ELE_ENERGY_CFG_T *drv_cfg,
                                            IN CONST VOID_T *dev_info)
{
    if ((NULL == drv_cfg) || (NULL == dev_info)) {
        PR_DEBUG("init info is null ptr");
        return OPRT_INVALID_PARM;
    }
    OPERATE_RET ret = OPRT_OK;

    /*全局数据初始化*/
    sg_chipid = chip_id;
    memset(&sg_coe_params, 0x00, SIZEOF(sg_coe_params));
    memset(&sg_def_coe_params, 0x00, SIZEOF(sg_def_coe_params));
    memset(&sg_factorytest, 0x00, SIZEOF(sg_factorytest));
    memset(&sg_hw_cfg, 0x00, SIZEOF(sg_hw_cfg));
    memcpy(&sg_hw_cfg, (TDD_BL0937_HLW8012_DRV_RESOURCE_T *)dev_info, SIZEOF(sg_hw_cfg));

    /*硬件资源初始化*/
    ret = __drv_out_pin_init(&sg_hw_cfg.ivcpin);
    if (OPRT_OK != ret) {
        return ret;
    }
    ret = __drv_hw_timer_init(sg_hw_cfg.timer_no);
    if (OPRT_OK != ret) {
        return ret;
    }

    /*运行参数初始化*/
    if (TDL_ELE_VOLT_120V == drv_cfg->params.volt_type) {
        __bl0937_hlw8012_coe_value_init(drv_cfg->params.resval, TDL_ELE_V_REF_120, TDL_ELE_I_REF_120,
                                        TDL_ELE_P_REF_120);
    } else {
        __bl0937_hlw8012_coe_value_init(drv_cfg->params.resval, TDL_ELE_V_REF_220, TDL_ELE_I_REF_220,
                                        TDL_ELE_P_REF_220);
    }
    __bl0937_hlw8012_run_params_init();

    if (TDL_ELE_TEST_MODE == drv_cfg->mode) {
        sg_factorytest.is_start_test = TRUE;
        sg_factorytest.test_result = TDL_ELE_TEST_FAIL;
        sg_factorytest.err_percent = drv_cfg->params.err_percent;
        if ((sg_factorytest.err_percent > 100) || (sg_factorytest.err_percent < 1)) {
            sg_factorytest.err_percent = 50; // 默认产测误差
        }
        PR_NOTICE("-->%s test err percent:%d", (TDD_ELE_CHIP_BL0937 == sg_chipid) ? ("bl0937") : ("hlw8012"),
                  sg_factorytest.err_percent);
        ret = sys_add_timer(__bl0937_hlw8012_fac_test_timer_cb, NULL, &sg_factorytest.timer_id);
        if (OPRT_OK != ret) {
            PR_ERR("bl0937_fac_test timer init err:%d", ret);
            return ret;
        }
        sys_start_timer(sg_factorytest.timer_id, 1000, TIMER_CYCLE);
    } else {
        sg_factorytest.is_start_test = FALSE;
        sg_factorytest.test_result = drv_cfg->fac_test.test_result;
        if (TDL_ELE_TEST_PASS == sg_factorytest.test_result) {
            memcpy(&sg_coe_params, &drv_cfg->fac_test.coe_data, SIZEOF(sg_coe_params));
        } else {
            memcpy(&sg_coe_params, &sg_def_coe_params, SIZEOF(sg_coe_params));
        }
    }

    return OPRT_OK;
}

/**
 * @brief hw8012硬件初始化
 *
 * @param[in] drv_cfg 驱动配置
 * @param[in] platform_resource 平台资源，例如：外部中断...
 * @return OPRT_OK 代表成功。如果是其他错误码，请参考 tuya_error_code.h
 */
STATIC OPERATE_RET __hlw8012_chip_init(IN CONST TDL_ELE_ENERGY_CFG_T *drv_cfg, IN CONST VOID_T *platform_resource)
{
    if ((NULL == drv_cfg) || (NULL == platform_resource)) {
        return OPRT_INVALID_PARM;
    }

    return __bl0937_hlw8012_hw_init(TDD_ELE_CHIP_HLW8012, drv_cfg, platform_resource);
}

/**
 * @brief bl0937硬件初始化
 *
 * @param[in] drv_cfg 驱动配置
 * @param[in] platform_resource 平台资源，例如：外部中断...
 * @return OPRT_OK 代表成功。如果是其他错误码，请参考 tuya_error_code.h
 */
STATIC OPERATE_RET __bl0937_init(IN CONST TDL_ELE_ENERGY_CFG_T *drv_cfg, IN CONST VOID_T *platform_resource)
{
    if ((NULL == drv_cfg) || (NULL == platform_resource)) {
        return OPRT_INVALID_PARM;
    }

    return __bl0937_hlw8012_hw_init(TDD_ELE_CHIP_BL0937, drv_cfg, platform_resource);
}

/*******************************************drv init && ops
 * register*************************************************************/
/**
 * @brief bl0937 ops 注册
 */
STATIC CONST TDL_ELE_ENERGY_DRV_OPS_T __bl0937_drv_ops = {
    .drv_init_cb = __bl0937_init,
    .drv_ctrl_cb = __bl0937_hlw8012_ctrl,
    .drv_read_pvie_cb = __bl0937_hlw8012_read_pvie_value,
    .drv_read_fac_test_data_cb = __bl0937_hlw8012_read_fac_test_data,
    .drv_read_fac_test_state_cb = __bl0937_hlw8012_read_fac_test_state,
};

/**
 * @brief hlw8012 ops 注册
 */
STATIC CONST TDL_ELE_ENERGY_DRV_OPS_T __hlw8012_drv_ops = {
    .drv_init_cb = __hlw8012_chip_init,
    .drv_ctrl_cb = __bl0937_hlw8012_ctrl,
    .drv_read_pvie_cb = __bl0937_hlw8012_read_pvie_value,
    .drv_read_fac_test_data_cb = __bl0937_hlw8012_read_fac_test_data,
    .drv_read_fac_test_state_cb = __bl0937_hlw8012_read_fac_test_state,
};

/**
 * @brief bl0937初始化
 *
 * @param[in] drv_cfg 芯片配置参数
 * @param[in] platform_resource 驱动芯片所需的外设资源
 * @return OPRT_OK 代表成功。如果是其他错误码，请参考 tuya_error_code.h
 */
OPERATE_RET tdd_ele_energy_bl0937_init(IN CONST TDL_ELE_ENERGY_CFG_T *drv_cfg,
                                       IN CONST TDD_BL0937_HLW8012_DRV_RESOURCE_T *platform_resource)
{
    if ((NULL == drv_cfg) || (NULL == platform_resource)) {
        return OPRT_INVALID_PARM;
    }
    TDL_ELE_ENERGY_DRIVER_T bl0937_drv;
    memset(&bl0937_drv, 0x00, SIZEOF(bl0937_drv));

    bl0937_drv.name = "bl0937";
    bl0937_drv.cfg = drv_cfg;
    bl0937_drv.ops = &__bl0937_drv_ops;
    bl0937_drv.platform_resource = (CONST VOID_T *)platform_resource;

    return tdl_ele_energy_driver_register(&bl0937_drv);
}

/**
 * @brief bl8012初始化
 *
 * @param[in] drv_cfg 芯片配置参数
 * @param[in] platform_resource 驱动芯片所需的外设资源
 * @return OPRT_OK 代表成功。如果是其他错误码，请参考 tuya_error_code.h
 */
OPERATE_RET tdd_ele_energy_hlw8012_init(IN CONST TDL_ELE_ENERGY_CFG_T *drv_cfg,
                                        IN CONST TDD_BL0937_HLW8012_DRV_RESOURCE_T *platform_resource)
{
    if ((NULL == drv_cfg) || (NULL == platform_resource)) {
        return OPRT_INVALID_PARM;
    }
    TDL_ELE_ENERGY_DRIVER_T hlw8012_drv;
    memset(&hlw8012_drv, 0x00, SIZEOF(hlw8012_drv));

    hlw8012_drv.name = "hlw8012";
    hlw8012_drv.cfg = drv_cfg;
    hlw8012_drv.ops = &__hlw8012_drv_ops;
    hlw8012_drv.platform_resource = (CONST VOID_T *)platform_resource;

    return tdl_ele_energy_driver_register(&hlw8012_drv);
}