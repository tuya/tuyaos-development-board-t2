/**
 * @file tdd_dimmer_kp18058.c
 * @author www.tuya.com
 * @brief tdd_dimmer_kp18058 module is used to drive KP18058ESP / KP18059ESP chip
 * @version 0.1
 * @date 2022-08-26
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */
#include <string.h>

#include "tal_memory.h"
#include "tal_log.h"

#include "tdl_dimmer_driver.h"
#include "tdd_dimmer_common.h"

#include "tdd_dimmer_kp1805x.h"
/***********************************************************
************************macro define************************
***********************************************************/
#define KP1805X_OUTPUT_CH_MAX       5u

/**
 * @brief frame format
 */
#define KP1805X_SETTING_TAG         0x01

/**
 * @brief work mode
 */
#define KP1805X_MODE_SLEEP          0x0     // sleep mode
#define KP1805X_MODE_RGB            0x1     // 3 channel output mode
#define KP1805X_MODE_CW             0x2     // 2 channel output mode
#define KP1805X_MODE_RGBCW          0x3     // 5 channel output mode

/**
 * @brief address
 */
#define KP1805X_ADDR_LVC            0x0     // address to set line voltage compensation

#define KP1805X_IMAX_SCALE_MAX      32
/**
 * @brief max current range (mA)
 */
#define KP18058_RGB_CUR_MAX         48      // RGB channel upper limit
#define KP18058_RGB_CUR_MIN         2       // RGB channel lower limit, actually 1.5
#define KP18058_CW_CUR_MAX          78      // CW channel upper limit, actually 77.5
#define KP18059_RGB_CUR_MAX         160     // RGB channel upper limit
#define KP18059_RGB_CUR_MIN         5       // RGB channel lower limit
#define KP18059_CW_CUR_MAX          78      // CW channel upper limit, actually 77.5

/**
 * @brief max output gray value
 */
#define KP1805X_GRAY_VAL_MAX        1023    // 1024 gray level

/***********************************************************
***********************typedef define***********************
***********************************************************/
/**
 * @brief work mode
 */
typedef UCHAR_T KP1805X_WORK_MODE_E;
#define KP1805X_MODE_SLEEP          0x0     // sleep mode
#define KP1805X_MODE_RGB            0x1     // 3 channel output mode
#define KP1805X_MODE_CW             0x2     // 2 channel output mode
#define KP1805X_MODE_RGBCW          0x3     // 5 channel output mode

/**
 * @brief driver type
 */
typedef UCHAR_T KP1805X_TYPE_E;
#define KP18059_TYPE    0
#define KP18058_TYPE    1

#pragma pack(1)
typedef struct {
    UCHAR_T    check:       1;   //奇偶校验位
    UCHAR_T    addr:        4;   //下一个byte的地址
    UCHAR_T    mode:        2;   //KP1805X_WORK_MODE_E
    UCHAR_T    tag:         1;   //固定标识
}SETTING_T;

typedef union{
    struct {
        UCHAR_T    check:       1;   //奇偶校验位
        UCHAR_T    start:       4;   //起始电压
        UCHAR_T    slop:        2;   //补偿斜率
        UCHAR_T    is_compen:   1;   //是否补偿
    }bit;

    UCHAR_T whole;
}LINE_VOLT_SET_U;

typedef union {
    struct {
        UCHAR_T    check:       1;   //奇偶校验位
        UCHAR_T    max_curr:    5;   //最大电流
        UCHAR_T    freq:        2;   //斩波频率设定
    }bit;
    
    UCHAR_T whole;
}MAX_CURR_RGB_CFG_U;

typedef union {
    struct {
        UCHAR_T    check:              1;   //奇偶校验位
        UCHAR_T    max_curr:           5;   //最大电流
        UCHAR_T    no_rc:              1;   //1:无 rc 滤波 0：有 rc 滤波
        UCHAR_T    rgb_dimming_mode:   1;   //1: 1-3 路斩波调光  0: 1-3 模拟调光
    }bit;

    UCHAR_T whole;
}MAX_CURR_CW_CFG_U;
typedef struct {
    USHORT_T    check_h:     1;   //高位 奇偶校验位
    USHORT_T    high:        5;   //颜色数值高 5 位
    USHORT_T    res1:        2;  
    USHORT_T    check_l:     1;   //低位 奇偶校验位
    USHORT_T    low:         5;   //颜色数值低 5 位
    USHORT_T    res2:        2;
}OUTPUT_T;

typedef struct {
    SETTING_T                setting;
    LINE_VOLT_SET_U          line_volt;
    MAX_CURR_RGB_CFG_U       rgb_max_curr;
    MAX_CURR_CW_CFG_U        cw_max_curr;
    OUTPUT_T                 out[KP1805X_OUTPUT_CH_MAX];
}IIC_PROT_T;

#pragma pack()
/**
 * @brief driver information
 */
typedef struct {
    KP1805X_TYPE_E           type;
    LINE_VOLT_SET_U          line_volt;
    MAX_CURR_RGB_CFG_U       rgb_max_curr;
    MAX_CURR_CW_CFG_U        cw_max_curr;
    TDD_I2C_PIN_T            i2c_pin;
    DIMMER_CH_CFG_U          ch_cfg;
    IIC_PROT_T               send_buf;
}DIMMER_KP1805X_DRV_T;

/***********************************************************
********************function declaration********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
STATIC KP1805X_WORK_MODE_E __kp1805x_get_work_mode(DIMMER_CH_CFG_U *p_cf_cfg, LIGHT_RGBCW_U *p_rgbcw)
{
    UCHAR_T i = 0, ch_id = 0;
    KP1805X_WORK_MODE_E mode = KP1805X_MODE_SLEEP;

    if(NULL == p_cf_cfg || NULL == p_rgbcw) {
        return KP1805X_MODE_SLEEP;
    }

    for(i = 0; i<LIGHT_COLOR_CHANNEL_MAX; i++) {
        ch_id = p_cf_cfg->array[i];
        if((DIMMER_CH_ID_INVALID == ch_id) || (p_rgbcw->array[i] == 0)) {
            continue;
        }

        if((ch_id >= DIMMER_RGB_CH_ID_MIN) && (ch_id <= DIMMER_RGB_CH_ID_MAX)) {
            mode |= KP1805X_MODE_RGB;
        }else if ((ch_id >= DIMMER_CW_CH_ID_MIN) && (ch_id <= DIMMER_CW_CH_ID_MAX)){
            mode |= KP1805X_MODE_CW;
        }
    }

    return mode;
}


STATIC VOID_T __kp1805x_add_parity_bit(UCHAR_T *p_data_buf, UINT_T buf_len)
{
    UCHAR_T i = 0, j = 0, num = 0, data = 0;

    if(NULL == p_data_buf) {
        return;
    }

    for(i = 0; i<buf_len; i++) {
        num  = 0;
        data = p_data_buf[i];

        for (j = 0; j < 7; j++) {
            if (data & 0x80) {
                num++;
            }
            data <<= 1;
        }

        if (num % 2 != 0) {
            p_data_buf[i] =  p_data_buf[i] | 0x01;
        }
    }

    return;
}

/**
 * @brief open KP1805x dimmer
 *
 * @param[in] drv_handle: dimmer driver handle
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
STATIC OPERATE_RET __tdd_dimmer_kp1805x_open(IN DIMMER_DRV_HANDLE drv_handle)
{
    DIMMER_KP1805X_DRV_T *p_drv = (DIMMER_KP1805X_DRV_T *)drv_handle;

    if(NULL == p_drv) {
        return OPRT_INVALID_PARM;
    }

    return tdd_sw_i2c_init(p_drv->i2c_pin);
}

/**
 * @brief close KP1805x dimmer
 *
 * @param[in] drv_handle: dimmer driver handle
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
STATIC OPERATE_RET __tdd_dimmer_kp1805x_close(IN DIMMER_DRV_HANDLE drv_handle)
{
    DIMMER_KP1805X_DRV_T *p_drv = (DIMMER_KP1805X_DRV_T *)drv_handle;

    if(NULL == p_drv) {
        return OPRT_INVALID_PARM;
    }

    return tdd_sw_i2c_deinit(p_drv->i2c_pin);
}

STATIC UCHAR_T __get_rgb_new_imax_scale(IN KP1805X_TYPE_E type, USHORT_T val, UCHAR_T imax_scale)
{
    FLOAT_T i_step = 1.5, i_max = 0.0;
	INT_T i_min = 0, new_imax_scale = 0;

    i_step = (type == KP18059_TYPE) ? 5.0 : 1.5;

    i_max = imax_scale*i_step;
    i_min = ((INT_T)(i_max*val)) >> 10;     // total Io/gray_level(1024)

    if (type == KP18058_TYPE){               //todo: 理论上将step 放大10 就会减少精度的损失，也不会有+1 +2的区别。不过需要验证
        new_imax_scale = (i_min/i_step) + 2; // Imax21: Iomax20/1.5+2; 1,2,3...32;  minimum Iomax=1.5mA
    }else if(type == KP18059_TYPE){
        new_imax_scale=(i_min/i_step) + 1;   // Imax21: Iomax20/5+1; 1,2,3...32;  minimum Iomax=5mA
    }

    if(new_imax_scale > KP1805X_IMAX_SCALE_MAX) {
        new_imax_scale = KP1805X_IMAX_SCALE_MAX;
    }
    
    return new_imax_scale;
}

/**
 * @brief send data to KP1805x dimmer
 *
 * @param[in] drv_handle: driver handle
 * @param[in] p_rgbcw: the value of the value
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
STATIC OPERATE_RET __tdd_dimmer_kp1805x_output(IN DIMMER_DRV_HANDLE drv_handle, LIGHT_RGBCW_U *p_rgbcw)
{
    DIMMER_KP1805X_DRV_T *p_drv = (DIMMER_KP1805X_DRV_T *)drv_handle;
    IIC_PROT_T *p_prot = NULL;
    UCHAR_T i = 0, ch_id = 0, rgb_imax_scale = 0, new_rgb_imax_scale = 0;
    USHORT_T max_val = 0, new_val = 0;

    if(NULL == p_drv || NULL == p_rgbcw) {
        return OPRT_INVALID_PARM;
    }

    p_prot = &p_drv->send_buf;
    memset((UCHAR_T *)p_prot, 0x00, SIZEOF(IIC_PROT_T));
    
    p_prot->setting.addr = KP1805X_ADDR_LVC;
    p_prot->setting.tag  = KP1805X_SETTING_TAG;
    p_prot->setting.mode = __kp1805x_get_work_mode(&p_drv->ch_cfg, p_rgbcw);

    p_prot->line_volt.whole       = p_drv->line_volt.whole;
    p_prot->cw_max_curr.whole     = p_drv->cw_max_curr.whole;
    p_prot->rgb_max_curr.bit.freq = p_drv->rgb_max_curr.bit.freq;

    //get new imax code according current gray value
    max_val = LIG_THREE_MAX(p_rgbcw->s.red, p_rgbcw->s.green, p_rgbcw->s.blue);
    rgb_imax_scale = p_drv->rgb_max_curr.bit.max_curr+1;
    new_rgb_imax_scale = __get_rgb_new_imax_scale(p_drv->type, max_val, rgb_imax_scale);
    p_prot->rgb_max_curr.bit.max_curr = new_rgb_imax_scale-1;

    for(i = 0; i< LIGHT_COLOR_CHANNEL_MAX; i++) {
        ch_id = p_drv->ch_cfg.array[i];

        if(DIMMER_CH_ID_INVALID == ch_id || ch_id >= KP1805X_OUTPUT_CH_MAX) {
            continue;
        } 

        if((ch_id >= DIMMER_RGB_CH_ID_MIN) && (ch_id <= DIMMER_RGB_CH_ID_MAX)) {
            if(new_rgb_imax_scale) {
                new_val = p_rgbcw->array[i]*rgb_imax_scale/new_rgb_imax_scale;
            }else{
                new_val = p_rgbcw->array[i];
            }

            p_prot->out[ch_id].high = (UCHAR_T)(new_val >> 5);
            p_prot->out[ch_id].low  = (UCHAR_T)(new_val & 0x001F);
        }else {
            p_prot->out[ch_id].high = (UCHAR_T)(p_rgbcw->array[i] >> 5);
            p_prot->out[ch_id].low  = (UCHAR_T)(p_rgbcw->array[i] & 0x001F);
        }
    }

    __kp1805x_add_parity_bit((UCHAR_T *)p_prot, SIZEOF(IIC_PROT_T));

    return tdd_dimmer_i2c_send_data(p_drv->i2c_pin, (UCHAR_T *)p_prot, SIZEOF(IIC_PROT_T));
}

/**
 * @brief register KP18059 dimmer
 *
 * @param[in] name: dimmer name
 * @param[in] p_i2c: i2c pin config
 * @param[in] p_ch_cfg: color channel id config
 * @param[in] p_chip_cfg: chip config
 * @param[in] tp: chip type
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
STATIC OPERATE_RET __tdd_dimmer_kp1805x_register(CHAR_T *name, TDD_I2C_PIN_T *p_i2c, DIMMER_CH_CFG_U *p_ch_cfg,\
                                                 DIMMER_KP1805X_CFG_T *p_chip_cfg, KP1805X_TYPE_E tp)
{
    // check configuration
    DIMMER_DRV_INTFS_T intfs;

    if(NULL == p_i2c || NULL == p_chip_cfg) {
        return OPRT_INVALID_PARM;
    }

    if(FALSE == tdd_check_channel_cfg_param(p_ch_cfg)) {
        TAL_PR_ERR("channel cfg error");
        return OPRT_INVALID_PARM;
    }

    DIMMER_KP1805X_DRV_T *p_drv = tal_malloc(SIZEOF(DIMMER_KP1805X_DRV_T));
    if (NULL == p_drv) {
        return OPRT_MALLOC_FAILED;
    }
    memset((UCHAR_T *)p_drv, 0x00, SIZEOF(DIMMER_KP1805X_DRV_T));

    memcpy((UCHAR_T *)&p_drv->i2c_pin, (UCHAR_T *)p_i2c,    SIZEOF(TDD_I2C_PIN_T));
    memcpy((UCHAR_T *)&p_drv->ch_cfg,  (UCHAR_T *)p_ch_cfg, SIZEOF(DIMMER_CH_CFG_U));

    p_drv->type = tp;

    p_drv->line_volt.bit.start     = p_chip_cfg->line_volt_comp.start_volt;
    p_drv->line_volt.bit.slop      = p_chip_cfg->line_volt_comp.slope;
    p_drv->line_volt.bit.is_compen = p_chip_cfg->line_volt_comp.enable;

    p_drv->rgb_max_curr.bit.freq     = p_chip_cfg->rgb_dimming.freq;
    p_drv->rgb_max_curr.bit.max_curr = p_chip_cfg->max_curr_code.color;

    p_drv->cw_max_curr.bit.no_rc     =  (TRUE == p_chip_cfg->line_volt_comp.rc_filter_en)?0:1;
    p_drv->cw_max_curr.bit.max_curr  =  p_chip_cfg->max_curr_code.white;

    p_drv->cw_max_curr.bit.rgb_dimming_mode =  p_chip_cfg->rgb_dimming.mode;

    intfs.open   = __tdd_dimmer_kp1805x_open;
    intfs.close  = __tdd_dimmer_kp1805x_close;
    intfs.output = __tdd_dimmer_kp1805x_output;

    return tdl_dimmer_register(name, &intfs, KP1805X_GRAY_VAL_MAX, DIMMER_DRIVER_IIC, (DIMMER_DRV_HANDLE)p_drv);
}

/**
 * @brief register KP18058 dimmer
 *
 * @param[in] name: dimmer name
 * @param[in] p_i2c: i2c pin config
 * @param[in] p_ch_cfg: color channel id config
 * @param[in] p_chip_cfg: chip config
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdd_dimmer_kp18058_register(CHAR_T *name, TDD_I2C_PIN_T *p_i2c,\
                                        DIMMER_CH_CFG_U *p_ch_cfg, DIMMER_KP1805X_CFG_T *p_chip_cfg)
{
    return __tdd_dimmer_kp1805x_register(name, p_i2c, p_ch_cfg, p_chip_cfg, KP18058_TYPE);
}

/**
 * @brief convert rgb maximum current
 *
 * @param[in] cur: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_kp18058_convert_rgb_curr_code(UCHAR_T current_ma)
{
    UCHAR_T cur_tmp = 0;
    UCHAR_T cur_code = 0x00;

    if (current_ma > KP18058_RGB_CUR_MAX) {
        cur_tmp = KP18058_RGB_CUR_MAX;
        TAL_PR_NOTICE("[KP18058] The maximum current setting cannot exceed %d, \
                             the default value has been used.", KP18058_RGB_CUR_MAX);
    } else if (current_ma < KP18058_RGB_CUR_MIN) {
        cur_tmp = KP18058_RGB_CUR_MIN;
    } else {
        cur_tmp = current_ma;
    }

    cur_code = cur_tmp * 2 / 3 - 1;

    return cur_code;
}

/**
 * @brief convert cw maximum current
 *
 * @param[in] cur: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_kp18058_convert_cw_curr_code(UCHAR_T current_ma)
{
    UCHAR_T cur_tmp = 0;
    UCHAR_T cur_code = 0x00;

    if (current_ma > KP18058_CW_CUR_MAX) {
        cur_tmp = KP18058_CW_CUR_MAX;
        TAL_PR_NOTICE("[KP18058] The maximum current setting cannot exceed %d, \
                             the default value has been used.", KP18058_CW_CUR_MAX);
    } else {
        cur_tmp = current_ma;
    }

    cur_code = cur_tmp * 2 / 5;

    return cur_code;
}

/**
 * @brief register KP18059 dimmer
 *
 * @param[in] name: dimmer name
 * @param[in] p_i2c: i2c pin config
 * @param[in] p_ch_cfg: color channel id config
 * @param[in] p_chip_cfg: chip config
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdd_dimmer_kp18059_register(CHAR_T *name, TDD_I2C_PIN_T *p_i2c,\
                                        DIMMER_CH_CFG_U *p_ch_cfg, DIMMER_KP1805X_CFG_T *p_chip_cfg)
{
    return __tdd_dimmer_kp1805x_register(name, p_i2c, p_ch_cfg, p_chip_cfg, KP18059_TYPE);
}

/**
 * @brief convert rgb maximum current
 *
 * @param[in] cur: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_kp18059_convert_rgb_curr_code(UCHAR_T current_ma)
{
    UCHAR_T cur_tmp = 0;
    UCHAR_T cur_code = 0x00;

    if (current_ma > KP18059_RGB_CUR_MAX) {
        cur_tmp = KP18059_RGB_CUR_MAX;
        TAL_PR_NOTICE("[KP18059] The maximum current setting cannot exceed %d, \
                             the default value has been used.", KP18059_RGB_CUR_MAX);
    } else if (current_ma < KP18059_RGB_CUR_MIN) {
        cur_tmp = KP18059_RGB_CUR_MIN;
    } else {
        cur_tmp = current_ma;
    }

    cur_code = cur_tmp / 5 - 1;

    return cur_code;
}

/**
 * @brief convert cw maximum current
 *
 * @param[in] cur: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_kp18059_convert_cw_curr_code(UCHAR_T current_ma)
{
    UCHAR_T cur_tmp = 0;
    UCHAR_T cur_code = 0x00;

    if (current_ma > KP18059_CW_CUR_MAX) {
        cur_tmp = KP18059_CW_CUR_MAX;
        TAL_PR_NOTICE("[KP18059] The maximum current setting cannot exceed %d, \
                             the default value has been used.", KP18059_CW_CUR_MAX);
    } else {
        cur_tmp = current_ma;
    }

    cur_code = cur_tmp * 2 / 5;

    return cur_code;
}