/**
 * @file tdd_dimmer_bp5758d.h
 * @author www.tuya.com
 * @brief tdd_dimmer_bp5758d module is used to drive BP5758D chip
 * @version 0.1
 * @date 2022-08-26
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#ifndef __TDD_DIMMER_BP5758D_H__
#define __TDD_DIMMER_BP5758D_H__

#include "tdd_dimmer_types.h"
#include "tdd_sw_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************macro define************************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef struct {
    CURRENT_RGBCW_CFG_U   max_curr_code;
}BP5758D_CFG_T;
/***********************************************************
********************function declaration********************
***********************************************************/

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
                                        DIMMER_CH_CFG_U *p_ch_cfg, BP5758D_CFG_T *p_chip_cfg);

/**
 * @brief convert maximum current ma to code
 *
 * @param[in] cur: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_bp5758d_convert_curr_code(UCHAR_T current_ma);

#ifdef __cplusplus
}
#endif

#endif /* __TDD_DIMMER_BP5758D_H__ */
