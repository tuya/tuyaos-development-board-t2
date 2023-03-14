/**
 * @file tdd_dimmer_bp1658cj.h
 * @author www.tuya.com
 * @brief tdd_dimmer_bp1658cj module is used to drive BP1658CJ chip
 * @version 0.1
 * @date 2022-08-26
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#ifndef __TDD_DIMMER_BP1658CJ_H__
#define __TDD_DIMMER_BP1658CJ_H__

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
    CURRENT_COLOR_CFG_T   max_curr_code;
}BP1658CJ_CFG_T;

/***********************************************************
********************function declaration********************
***********************************************************/
/**
 * @brief register BP1658CJ dimmer
 *
 * @param[in] name: dimmer name
 * @param[in] p_i2c: i2c pin config
 * @param[in] p_ch_cfg: color channel id config
 * @param[in] p_chip_cfg:  chip config (max current code ex.)
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdd_dimmer_bp1658cj_register(CHAR_T *name, TDD_I2C_PIN_T *p_i2c,\
                                         DIMMER_CH_CFG_U *p_ch_cfg, BP1658CJ_CFG_T *p_chip_cfg);

/**
 * @brief convert maximum rgb current ma to code
 *
 * @param[in] cur: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_bp1658cj_convert_rgb_curr_code(UCHAR_T current_ma);

/**
 * @brief convert maximum cw current ma to code
 *
 * @param[in] cur: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_bp1658cj_convert_cw_curr_code(UCHAR_T current_ma);

#ifdef __cplusplus
}
#endif

#endif /* __TDD_DIMMER_BP1658CJ_H__ */
