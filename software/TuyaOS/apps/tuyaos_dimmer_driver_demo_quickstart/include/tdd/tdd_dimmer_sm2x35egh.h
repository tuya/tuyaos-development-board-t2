/**
 * @file tdd_dimmer_sm2235egh.h
 * @author www.tuya.com
 * @brief tdd_dimmer_sm2235egh module is used to drive SM2235EGH
 * @version 0.1
 * @date 2022-08-26
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#ifndef __TDD_DIMMER_SM2235EGH_H__
#define __TDD_DIMMER_SM2235EGH_H__

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
/**
 * @brief SM2x35EGH dimmer configuration
 */
typedef struct {
    CURRENT_COLOR_CFG_T   max_curr_code;
}SM2X35EGH_CFG_T;

/***********************************************************
********************function declaration********************
***********************************************************/
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
                                         DIMMER_CH_CFG_U *p_ch_cfg, SM2X35EGH_CFG_T *p_chip_cfg);

/**
 * @brief convert rgb maximum current
 *
 * @param[in] cur: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_sm2235egh_convert_rgb_curr_code(UCHAR_T current_ma);

/**
 * @brief convert cw maximum current
 *
 * @param[in] cur: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_sm2235egh_convert_cw_curr_code(UCHAR_T current_ma);

/**
 * @brief convert rgb maximum current
 *
 * @param[in] cur: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_sm2335egh_convert_rgb_curr_code(UCHAR_T current_ma);

/**
 * @brief convert cw maximum current
 *
 * @param[in] cur: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_sm2335egh_convert_cw_curr_code(UCHAR_T current_ma);


#ifdef __cplusplus
}
#endif

#endif /* __TDD_DIMMER_SM2235EGH_H__ */
