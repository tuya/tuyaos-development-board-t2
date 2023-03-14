/**
 * @file tdd_dimmer_common.c
 * @author www.tuya.com
 * @version 0.1
 * @date 2023-03-06
 *
 * @copyright Copyright (c) tuya.inc 2023
 *
 */

#include "tdu_light_types.h"

#include "tdd_dimmer_common.h"
/***********************************************************
*********************** macro define ***********************
***********************************************************/


/***********************************************************
********************** typedef define **********************
***********************************************************/


/***********************************************************
********************** variable define *********************
***********************************************************/


/***********************************************************
********************** function define *********************
***********************************************************/
/**
 * @brief       检查颜色通道配置参数合法性
 *
 * @param[in] : p_ch_cfg: color channel config
 *
 * @return TRUE: 合法  FALSE: 非法
 */
BOOL_T tdd_check_channel_cfg_param(DIMMER_CH_CFG_U *p_ch_cfg)
{
    if(NULL == p_ch_cfg) {
        return FALSE;
    }

    if(p_ch_cfg->r < DIMMER_RGB_CH_ID_MIN || p_ch_cfg->r > DIMMER_RGB_CH_ID_MAX) {
        return FALSE;
    }

    if(p_ch_cfg->g < DIMMER_RGB_CH_ID_MIN || p_ch_cfg->g > DIMMER_RGB_CH_ID_MAX) {
        return FALSE;
    }

    if(p_ch_cfg->b < DIMMER_RGB_CH_ID_MIN || p_ch_cfg->b > DIMMER_RGB_CH_ID_MAX) {
        return FALSE;
    }

    if(p_ch_cfg->c < DIMMER_CW_CH_ID_MIN || p_ch_cfg->c > DIMMER_CW_CH_ID_MAX) {
        return FALSE;
    }

    if(p_ch_cfg->w < DIMMER_CW_CH_ID_MIN || p_ch_cfg->w > DIMMER_CW_CH_ID_MAX) {
        return FALSE;
    }

    return TRUE;
}

/**
 * @brief       send data
 *
 * @param[in] i2c_pin: the information of iic pin number
 * @param[in] data: data buffer
 * @param[in] len: data length
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdd_dimmer_i2c_send_data(TDD_I2C_PIN_T i2c_pin, IN UCHAR_T *data, IN UCHAR_T len)
{
    TDD_I2C_MSG_T wr_msg = {
        .flags = I2C_FLAG_WR | I2C_FLAG_NO_ADDR,
        .addr  = 0,
        .len   = len,
        .buff  = data
    };

    return tdd_sw_i2c_xfer(i2c_pin, &wr_msg);
}

/**
 * @brief       send data (flags)
 *
 * @param[in] i2c_pin: the information of iic pin number
 * @param[in] flags: iic protocol flags
 * @param[in] data: data buffer
 * @param[in] len: data length
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdd_dimmer_i2c_flag_send_data(TDD_I2C_PIN_T i2c_pin, INT_T flags, IN UCHAR_T *data, IN UCHAR_T len)
{
    TDD_I2C_MSG_T wr_msg = {
        .flags = flags,
        .addr  = 0,
        .len   = len,
        .buff  = data
    };

    return tdd_sw_i2c_xfer(i2c_pin, &wr_msg);    
}