/**
 * @file tdd_dimmer_bp5758d.c
 * @author www.tuya.com
 * @brief tdd_dimmer_bp5758d module is used to drive BP5758D chip
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

#include "tdd_dimmer_bp5758d.h"
/***********************************************************
************************macro define************************
***********************************************************/
#define BP5758D_OUTPUT_CH_MAX       5u

/**
 * @brief frame format
 */
#define BP5758D_SETTING_TAG         0x02 

/**
 * @brief address
 */
#define BP5758D_ADDR_ENABLE         0x00     // address to set enabled channel
#define BP5758D_ADDR_OUTPUT         0x06     // address to set output

/**
 * @brief max current range (mA)
 */
#define BP5758D_RGBCW_CUR_MAX       90      // RGBCW channel upper limit

/**
 * @brief enable channel
 */
#define BP5758D_ENABLE_OUT1         0x1     // OUT1 enable
#define BP5758D_ENABLE_OUT2         0x2     // OUT2 enable
#define BP5758D_ENABLE_OUT3         0x4     // OUT3 enable
#define BP5758D_ENABLE_OUT4         0x8     // OUT4 enable
#define BP5758D_ENABLE_OUT5         0x10    // OUT5 enable

/**
 * @brief max output gray value
 */
#define BP5758D_GRAY_VAL_MAX        1023    // 1024 gray level

/***********************************************************
***********************typedef define***********************
***********************************************************/
/**
 * @brief work mode
 */
typedef UCHAR_T BP5758D_WORK_MODE_E;
#define BP5758D_MODE_SLEEP          0x00     // 00: sleep mode
#define BP5758D_MODE_WORK           0x01     // 01~11: work mode

#pragma pack(1)
typedef struct {
    UCHAR_T    addr:        4;   //下一个byte的地址
    UCHAR_T    mode:        2;   //0:sleep  非0: work
    UCHAR_T    tag:         2;   //固定标识
}SETTING_T;

typedef struct {
    USHORT_T    low:         5;     //颜色数值低 5 位
    USHORT_T    res1:        3; 
    USHORT_T    high:        5;     //颜色数值高 5 位
    USHORT_T    res2:        3;
}OUTPUT_T;

typedef struct {
    SETTING_T  setting;
    UCHAR_T    enable;
    UCHAR_T    max_current[BP5758D_OUTPUT_CH_MAX];
    OUTPUT_T   out[BP5758D_OUTPUT_CH_MAX];
}IIC_PROT_T;

#pragma pack()

/**
 * @brief driver information
 */
typedef struct {
    TDD_I2C_PIN_T            i2c_pin;
    CURRENT_RGBCW_CFG_U      max_cur;
    DIMMER_CH_CFG_U          ch_cfg;
    UCHAR_T                  ch_enable;
    IIC_PROT_T               send_buf;
}DIMMER_BP5758D_DRV_T;

/***********************************************************
********************function declaration********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
STATIC UCHAR_T __bp5758d_get_work_mode(DIMMER_CH_CFG_U *p_cf_cfg, LIGHT_RGBCW_U *p_rgbcw)
{
    UCHAR_T i = 0;

    if(NULL == p_cf_cfg || NULL == p_rgbcw) {
        return BP5758D_MODE_SLEEP;
    }

    for(i = 0; i<LIGHT_COLOR_CHANNEL_MAX; i++) {
        if((p_cf_cfg->array[i] != DIMMER_CH_ID_INVALID) && (p_rgbcw->array[i] != 0)) {
            return BP5758D_MODE_WORK;
        }
    }

    return BP5758D_MODE_SLEEP;
}

STATIC UCHAR_T __bp5758d_get_ch_enable(DIMMER_CH_CFG_U *p_cf_cfg)
{
    UCHAR_T i = 0, ch_id = 0, enable_code = 0; 

    if(NULL == p_cf_cfg) {
        return 0;
    }

    for (i = 0; i < BP5758D_OUTPUT_CH_MAX; i++) { 
        ch_id = p_cf_cfg->array[i];

        if(DIMMER_CH_ID_INVALID == ch_id || ch_id >= BP5758D_OUTPUT_CH_MAX) {
            continue;
        }

        enable_code |= (1 << ch_id);
    }

    return enable_code;
}

STATIC OPERATE_RET __bp5758d_send_prot(DIMMER_BP5758D_DRV_T *p_drv, UCHAR_T work_mode,\
                                       UCHAR_T ch_enable, LIGHT_RGBCW_U *p_rgbcw)
{
    IIC_PROT_T *p_prot = NULL;
    UCHAR_T i = 0, ch_id = 0, len = 0;

    if(NULL == p_drv) {
        return OPRT_INVALID_PARM;
    }

    p_prot = &p_drv->send_buf;
    memset((UCHAR_T *)p_prot, 0x00, SIZEOF(IIC_PROT_T));

    p_prot->setting.mode =  work_mode;
    p_prot->setting.tag  =  BP5758D_SETTING_TAG;

    p_prot->setting.addr =  BP5758D_ADDR_ENABLE;
    p_prot->enable = ch_enable;

    for (i = 0; i < LIGHT_COLOR_CHANNEL_MAX; i++) {
        ch_id = p_drv->ch_cfg.array[i];
        if(DIMMER_CH_ID_INVALID == ch_id || ch_id >= BP5758D_OUTPUT_CH_MAX) {
            continue;
        }

        p_prot->max_current[ch_id] = p_drv->max_cur.array[i];

        //设置灰度
        if(NULL == p_rgbcw) {
            p_prot->out[ch_id].low  = 0;
            p_prot->out[ch_id].high = 0;
        }else {
            p_prot->out[ch_id].low  = (UCHAR_T)(p_rgbcw->array[i] & 0x001F);
            p_prot->out[ch_id].high = (UCHAR_T)(p_rgbcw->array[i] >> 5);
        }
    }

    len = (BP5758D_MODE_SLEEP == work_mode)? 1 : SIZEOF(IIC_PROT_T);

    return tdd_dimmer_i2c_send_data(p_drv->i2c_pin, (UCHAR_T *)p_prot, len);
}

/**
 * @brief open BP5758D dimmer
 *
 * @param[in] drv_handle: dimmer driver handle
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
STATIC OPERATE_RET __tdd_dimmer_bp5758d_open(IN DIMMER_DRV_HANDLE drv_handle)
{
    DIMMER_BP5758D_DRV_T *p_drv = (DIMMER_BP5758D_DRV_T *)drv_handle;

    if(NULL == p_drv) {
        return OPRT_INVALID_PARM;
    }

    return tdd_sw_i2c_init(p_drv->i2c_pin);
}

/**
 * @brief close BP5758D dimmer
 *
 * @param[in] drv_handle: dimmer driver handle
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
STATIC OPERATE_RET __tdd_dimmer_bp5758d_close(IN DIMMER_DRV_HANDLE drv_handle)
{
    DIMMER_BP5758D_DRV_T *p_drv = (DIMMER_BP5758D_DRV_T *)drv_handle;

    if(NULL == p_drv) {
        return OPRT_INVALID_PARM;
    }

    return tdd_sw_i2c_deinit(p_drv->i2c_pin);
}

/**
 * @brief send data to BP5758D dimmer
 *
 * @param[in] drv_handle: dimmer driver handle
 * @param[in] p_rgbcw: the value of the value
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
STATIC OPERATE_RET __tdd_dimmer_bp5758d_output(DIMMER_DRV_HANDLE drv_handle, LIGHT_RGBCW_U *p_rgbcw)
{
    OPERATE_RET rt = OPRT_OK;
    DIMMER_BP5758D_DRV_T *p_drv = (DIMMER_BP5758D_DRV_T *)drv_handle;
    UCHAR_T work_mode = 0, ch_enable = 0;

    if(NULL == p_drv) {
        return OPRT_INVALID_PARM;
    }

    work_mode = __bp5758d_get_work_mode(&p_drv->ch_cfg, p_rgbcw);
    ch_enable = p_drv->ch_enable;

    if (BP5758D_MODE_SLEEP == work_mode) {
        //睡眠模式下无法接收byte0之外的数据,所以睡眠之前需要在正常模式下,关闭通道,灰度置0
        TUYA_CALL_ERR_RETURN(__bp5758d_send_prot(p_drv, BP5758D_MODE_WORK, 0, NULL));
        ch_enable = 0;
    }

    TUYA_CALL_ERR_RETURN(__bp5758d_send_prot(p_drv, work_mode, ch_enable, p_rgbcw));

    return OPRT_OK;
}

/**
 * @brief register BP5758D dimmer
 *
 * @param[in] name: dimmer name
 * @param[in] p_i2c: i2c pin config
 * @param[in] p_max_curr_code: the code of max electric current setting
 * @param[in] p_ch_cfg: color channel id config
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdd_dimmer_bp5758d_register(CHAR_T *name, TDD_I2C_PIN_T *p_i2c, \
                                        DIMMER_CH_CFG_U *p_ch_cfg, BP5758D_CFG_T *p_chip_cfg)
{
    DIMMER_DRV_INTFS_T intfs;
    CURRENT_RGBCW_CFG_U *p_max_curr = NULL;

    if(NULL == p_i2c || NULL == p_chip_cfg) {
        return OPRT_INVALID_PARM;
    }

    if(FALSE == tdd_check_channel_cfg_param(p_ch_cfg)) {
        TAL_PR_ERR("channel cfg error");
        return OPRT_INVALID_PARM;
    }

    DIMMER_BP5758D_DRV_T *p_drv = tal_malloc(SIZEOF(DIMMER_BP5758D_DRV_T));
    if (NULL == p_drv) {
        return OPRT_MALLOC_FAILED;
    }
    memset((UCHAR_T *)p_drv, 0x00, SIZEOF(DIMMER_BP5758D_DRV_T));

    memcpy((UCHAR_T *)&p_drv->i2c_pin, (UCHAR_T *)p_i2c,           SIZEOF(TDD_I2C_PIN_T));
    memcpy((UCHAR_T *)&p_drv->ch_cfg,  (UCHAR_T *)p_ch_cfg,        SIZEOF(DIMMER_CH_CFG_U));

    p_max_curr = &p_chip_cfg->max_curr_code;
    memcpy((UCHAR_T *)&p_drv->max_cur, (UCHAR_T *)p_max_curr, SIZEOF(CURRENT_RGBCW_CFG_U));

    p_drv->ch_enable = __bp5758d_get_ch_enable(&p_drv->ch_cfg);

    intfs.open   = __tdd_dimmer_bp5758d_open;
    intfs.close  = __tdd_dimmer_bp5758d_close;
    intfs.output = __tdd_dimmer_bp5758d_output;

    return tdl_dimmer_register(name, &intfs, BP5758D_GRAY_VAL_MAX, DIMMER_DRIVER_IIC, (DIMMER_DRV_HANDLE)p_drv);
}

/**
 * @brief convert maximum current ma to code
 *
 * @param[in] cur: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_bp5758d_convert_curr_code(UCHAR_T current_ma)
{
    UCHAR_T cur_tmp_ma = 0;
    UCHAR_T cur_code = 0x00;

    if (current_ma > BP5758D_RGBCW_CUR_MAX) {
        cur_tmp_ma = BP5758D_RGBCW_CUR_MAX;
        TAL_PR_NOTICE("[BP5758D] The maximum current setting cannot exceed %d, \
                      the default value has been used.", BP5758D_RGBCW_CUR_MAX);
    } else {
        cur_tmp_ma = current_ma;
    }

    if (cur_tmp_ma <= 63) {
        cur_code = cur_tmp_ma & 0x3F;
    } else {
        cur_code = 0x40 | ((cur_tmp_ma - 30) & 0x3F);
    }

    return cur_code;
}