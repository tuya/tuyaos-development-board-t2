/**
 * @file tdd_dimmer_sm2235egh.c
 * @author www.tuya.com
 * @brief tdd_dimmer_sm2235egh module is used to drive SM2235EGH chip
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

#include "tdd_dimmer_sm2x35egh.h"
/***********************************************************
************************macro define************************
***********************************************************/
#define SM2X35EGH_OUTPUT_CH_MAX    5u

/**
 * @brief frame format
 */
#define SM2X35EGH_SETTING_TAG      0x03

/**
 * @brief address
 */
#define SM2X35EGH_ADDR_GRAY_OUT1    0x00     // address to set gray data of OUT1 channel

/**
 * @brief max current range (mA)
 */
#define SM2235EGH_RGB_CUR_MAX       64      // RGB channel upper limit
#define SM2235EGH_RGB_CUR_MIN       4       // RGB channel lower limit
#define SM2235EGH_CW_CUR_MAX        80      // CW channel upper limit
#define SM2235EGH_CW_CUR_MIN        5       // CW channel lower limit
#define SM2335EGH_RGB_CUR_MAX       160     // RGB channel upper limit
#define SM2335EGH_RGB_CUR_MIN       10      // RGB channel lower limit
#define SM2335EGH_CW_CUR_MAX        80      // CW channel upper limit
#define SM2335EGH_CW_CUR_MIN        5       // CW channel lower limit

/**
 * @brief max output gray value
 */
#define SM2X35EGH_GRAY_VAL_MAX      1023   // 1024 gray level

/***********************************************************
***********************typedef define***********************
***********************************************************/
/**
 * @brief work mode
 */
typedef UCHAR_T SM2X35EGH_WORK_MODE_E;
#define SM2X35EGH_MODE_SLEEP        0x0     // sleep mode
#define SM2X35EGH_MODE_RGB          0x1     // 3 channel output mode
#define SM2X35EGH_MODE_CW           0x2     // 2 channel output mode
#define SM2X35EGH_MODE_RGBCW        0x3     // 5 channel output mode

#pragma pack(1)
typedef struct {
    UCHAR_T    addr:        3;   //下一个byte的地址
    UCHAR_T    mode:        2;
    UCHAR_T    res:         1;  
    UCHAR_T    tag:         2;   //固定标识
}SETTING_T;

typedef struct {
    UCHAR_T    high;
    UCHAR_T    low;
}OUTPUT_T;

typedef struct {
    SETTING_T                setting;
    UCHAR_T                  max_current;
    OUTPUT_T                 out[SM2X35EGH_OUTPUT_CH_MAX];
}IIC_PROT_T;

#pragma pack()


/**
 * @brief driver information
 */
typedef struct {
    TDD_I2C_PIN_T            i2c_pin;
    UCHAR_T                  max_cur_code;
    DIMMER_CH_CFG_U          ch_cfg;
    IIC_PROT_T               send_buf;
}DIMMER_SM2X35EGH_DRV_T;

/***********************************************************
********************function declaration********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
STATIC SM2X35EGH_WORK_MODE_E __sm2x35egh_get_work_mode(DIMMER_CH_CFG_U *p_cf_cfg, LIGHT_RGBCW_U *p_rgbcw)
{
    UCHAR_T i = 0, ch_id = 0;
    SM2X35EGH_WORK_MODE_E mode = SM2X35EGH_MODE_SLEEP;

    if(NULL == p_cf_cfg || NULL == p_rgbcw) {
        return SM2X35EGH_MODE_SLEEP;
    }

    for(i = 0; i<LIGHT_COLOR_CHANNEL_MAX; i++) {
        ch_id = p_cf_cfg->array[i];
        if((DIMMER_CH_ID_INVALID == ch_id) || (p_rgbcw->array[i] == 0)) {
            continue;
        }

        if( 0 == p_rgbcw->array[i]) {
            continue;
        }

        if((ch_id >= DIMMER_RGB_CH_ID_MIN) && (ch_id <= DIMMER_RGB_CH_ID_MAX)) {
            mode |= SM2X35EGH_MODE_RGB;
        }else if ((ch_id >= DIMMER_CW_CH_ID_MIN) && (ch_id <= DIMMER_CW_CH_ID_MAX)){
            mode |= SM2X35EGH_MODE_CW;
        }
    }

    return mode;
}

/**
 * @brief open SM2x35EGH dimmer
 *
 * @param[in] drv_handle: dimmer driver handle
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
STATIC OPERATE_RET __tdd_dimmer_sm2x35egh_open(IN DIMMER_DRV_HANDLE drv_handle)
{
    DIMMER_SM2X35EGH_DRV_T *p_drv = (DIMMER_SM2X35EGH_DRV_T *)drv_handle;

    if(NULL == p_drv) {
        return OPRT_INVALID_PARM;
    }

    return tdd_sw_i2c_init(p_drv->i2c_pin);
}

/**
 * @brief close SM2x35EGH dimmer
 *
 * @param[in] drv_handle: dimmer driver handle
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
STATIC OPERATE_RET __tdd_dimmer_sm2x35egh_close(IN DIMMER_DRV_HANDLE drv_handle)
{
    DIMMER_SM2X35EGH_DRV_T *p_drv = (DIMMER_SM2X35EGH_DRV_T *)drv_handle;

    if(NULL == p_drv) {
        return OPRT_INVALID_PARM;
    }

    return tdd_sw_i2c_deinit(p_drv->i2c_pin);
}

/**
 * @brief control dimmer output
 *
 * @param[in] drv_handle: driver handle
 * @param[in] p_rgbcw: the value of the value
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
STATIC OPERATE_RET __tdd_dimmer_sm2x35egh_output(IN DIMMER_DRV_HANDLE drv_handle, LIGHT_RGBCW_U *p_rgbcw)
{
    DIMMER_SM2X35EGH_DRV_T *p_drv = (DIMMER_SM2X35EGH_DRV_T *)drv_handle;
    IIC_PROT_T *p_prot = NULL;
    UCHAR_T i = 0, ch_id = 0;
    UINT_T i2c_flags = 0;

    if(NULL == p_drv || NULL == p_rgbcw) {
        return OPRT_INVALID_PARM;
    }

    p_prot = &p_drv->send_buf;
    memset((UCHAR_T *)p_prot, 0x00, SIZEOF(IIC_PROT_T));

    p_prot->setting.tag  = SM2X35EGH_SETTING_TAG;
    p_prot->setting.addr = SM2X35EGH_ADDR_GRAY_OUT1;
    p_prot->setting.mode = __sm2x35egh_get_work_mode(&p_drv->ch_cfg, p_rgbcw);
    p_prot->max_current  = p_drv->max_cur_code;

    for (i = 0; i < LIGHT_COLOR_CHANNEL_MAX; i++) {
        ch_id = p_drv->ch_cfg.array[i];

        if(DIMMER_CH_ID_INVALID == ch_id || ch_id >= SM2X35EGH_OUTPUT_CH_MAX) {
            continue;
        }

        p_prot->out[ch_id].low  = (UCHAR_T)(p_rgbcw->array[i] & 0x00FF);
        p_prot->out[ch_id].high = (UCHAR_T)(p_rgbcw->array[i] >> 8);
    }

    i2c_flags = I2C_FLAG_WR | I2C_FLAG_NO_ADDR | I2C_FLAG_IGNORE_NACK;

    return tdd_dimmer_i2c_flag_send_data(p_drv->i2c_pin, i2c_flags, (UCHAR_T *)p_prot, SIZEOF(IIC_PROT_T));
}

/**
 * @brief register SM2235EGH/SM2335EGH dimmer
 *
 * @param[in] name: dimmer name
 * @param[in] p_i2c: i2c pin config
 * @param[in] p_ch_cfg: color channel id config
 * @param[in] p_chip_cfg:  chip config (max current code ex.)
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdd_dimmer_sm2x35egh_register(CHAR_T *name, TDD_I2C_PIN_T *p_i2c,\
                                         DIMMER_CH_CFG_U *p_ch_cfg, SM2X35EGH_CFG_T *p_chip_cfg)
{
    DIMMER_DRV_INTFS_T intfs;

    if(NULL == p_i2c || NULL == p_chip_cfg) {
        return OPRT_INVALID_PARM;
    }

    if(FALSE == tdd_check_channel_cfg_param(p_ch_cfg)) {
        TAL_PR_ERR("channel cfg error");
        return OPRT_INVALID_PARM;
    }

    DIMMER_SM2X35EGH_DRV_T *p_drv = tal_malloc(SIZEOF(DIMMER_SM2X35EGH_DRV_T));
    if (NULL == p_drv) {
        return OPRT_MALLOC_FAILED;
    }
    memset((UCHAR_T *)p_drv, 0x00, SIZEOF(DIMMER_SM2X35EGH_DRV_T));

    memcpy((UCHAR_T *)&p_drv->i2c_pin, (UCHAR_T *)p_i2c,    SIZEOF(TDD_I2C_PIN_T));
    memcpy((UCHAR_T *)&p_drv->ch_cfg,  (UCHAR_T *)p_ch_cfg, SIZEOF(DIMMER_CH_CFG_U));

    p_drv->max_cur_code = ((p_chip_cfg->max_curr_code.color << 4) | (p_chip_cfg->max_curr_code.white & 0x0F));

    intfs.open   = __tdd_dimmer_sm2x35egh_open;
    intfs.close  = __tdd_dimmer_sm2x35egh_close;
    intfs.output = __tdd_dimmer_sm2x35egh_output;

    return tdl_dimmer_register(name, &intfs, SM2X35EGH_GRAY_VAL_MAX, DIMMER_DRIVER_IIC, (DIMMER_DRV_HANDLE)p_drv);
}

/**
 * @brief convert rgb maximum current
 *
 * @param[in] cur: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_sm2235egh_convert_rgb_curr_code(UCHAR_T current_ma)
{
    UCHAR_T cur_tmp = 0;
    UCHAR_T cur_code = 0x00;

    if (current_ma > SM2235EGH_RGB_CUR_MAX) {
        cur_tmp = SM2235EGH_RGB_CUR_MAX;
        TAL_PR_NOTICE("[SM2235EGH] The maximum current setting cannot exceed %d, \
                               the default value has been used.", SM2235EGH_RGB_CUR_MAX);
    } else if (current_ma < SM2235EGH_RGB_CUR_MIN) {
        cur_tmp = SM2235EGH_RGB_CUR_MIN;
    } else {
        cur_tmp = current_ma;
    }
    cur_code = ((cur_tmp / 4 - 1) << 4);

    return cur_code;
}

/**
 * @brief convert cw maximum current
 *
 * @param[in] cur: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_sm2235egh_convert_cw_curr_code(UCHAR_T current_ma)
{
    UCHAR_T cur_tmp = 0;
    UCHAR_T cur_code = 0x00;

    if (current_ma > SM2235EGH_CW_CUR_MAX) {
        cur_tmp = SM2235EGH_CW_CUR_MAX;
        TAL_PR_NOTICE("[SM2235EGH] The maximum current setting cannot exceed %d, \
                               the default value has been used.", SM2235EGH_CW_CUR_MAX);
    } else if (current_ma < SM2235EGH_CW_CUR_MIN) {
        cur_tmp = SM2235EGH_CW_CUR_MIN;
    } else {
        cur_tmp = current_ma;
    }

    cur_code = (cur_tmp / 5 - 1);

    return cur_code;
}

/**
 * @brief convert rgb maximum current
 *
 * @param[in] cur: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_sm2335egh_convert_rgb_curr_code(UCHAR_T current_ma)
{
    UCHAR_T cur_tmp = 0;
    UCHAR_T cur_code = 0x00;

   if (current_ma > SM2335EGH_RGB_CUR_MAX) {
        cur_tmp = SM2335EGH_RGB_CUR_MAX;
        TAL_PR_NOTICE("[SM2335EGH] The maximum current setting cannot exceed %d, \
                               the default value has been used.", SM2335EGH_RGB_CUR_MAX);
    } else if (current_ma < SM2335EGH_RGB_CUR_MIN) {
        cur_tmp = SM2335EGH_RGB_CUR_MIN;
    } else {
        cur_tmp = current_ma;
    }

    cur_code = ((cur_tmp / 10 - 1) << 4);

    return cur_code;
}

/**
 * @brief convert cw maximum current
 *
 * @param[in] cur: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_sm2335egh_convert_cw_curr_code(UCHAR_T current_ma)
{
    UCHAR_T cur_tmp = 0;
    UCHAR_T cur_code = 0x00;

    if (current_ma > SM2335EGH_CW_CUR_MAX) {
        cur_tmp = SM2335EGH_CW_CUR_MAX;
        TAL_PR_NOTICE("[SM2335EGH] The maximum current setting cannot exceed %d, \
                               the default value has been used.", SM2335EGH_CW_CUR_MAX);
    } else if (current_ma < SM2335EGH_CW_CUR_MIN) {
        cur_tmp = SM2335EGH_CW_CUR_MIN;
    } else {
        cur_tmp = current_ma;
    }

    cur_code = (cur_tmp / 5 - 1);

    return cur_code;
}