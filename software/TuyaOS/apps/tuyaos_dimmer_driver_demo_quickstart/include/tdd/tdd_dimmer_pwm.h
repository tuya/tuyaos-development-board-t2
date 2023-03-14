/**
 * @file tdd_dimmer_pwm.h
 * @author www.tuya.com
 * @brief tdd_dimmer_pwm module is used to driver led by PWM
 * @version 0.1
 * @date 2022-08-25
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#ifndef __TDD_DIMMER_PWM_H__
#define __TDD_DIMMER_PWM_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************macro define************************
***********************************************************/
#define DIMMER_PWM_ID_INVALID               0xFE

#define DIMMER_PWM_NUM_MAX                  5u

#define DIMMER_PWM_CH_IDX_RED               0
#define DIMMER_PWM_CH_IDX_GREEN             1
#define DIMMER_PWM_CH_IDX_BLUE              2
#define DIMMER_PWM_CH_IDX_COLD              3
#define DIMMER_PWM_CH_IDX_WARM              4

/**
 * @brief pwm frequency limit
 */
#define DIMMER_PWM_FREQ_MAX                20000   // max: 20kHz
#define DIMMER_PWM_FREQ_MIN                100     // min: 100Hz

/**
 * @brief pwm duty limit
 */
#define DIMMER_PWM_DUTY_MAX                10000   // max: 100%

/***********************************************************
***********************typedef define***********************
***********************************************************/
/**
 * @brief dimmer configuration
 */
typedef struct {
    UINT_T                  pwm_freq;       // pwm frequency (Hz)
    BOOL_T                  active_level;   // true means active high, false means active low
    TUYA_PWM_NUM_E          pwm_ch_arr[DIMMER_PWM_NUM_MAX];   // pwm id of each channel
} DIMMER_PWM_CFG_T;

/***********************************************************
********************function declaration********************
***********************************************************/
/**
 * @brief register pwm cw dimmer
 *
 * @param[in] name: dimmer name
 * @param[in] p_cfg: dimmer configuration
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdd_dimmer_pwm_cw_register(IN CHAR_T *name, IN DIMMER_PWM_CFG_T *p_cfg);

/**
 * @brief register pwm cct dimmer
 *
 * @param[in] name: dimmer name
 * @param[in] p_cfg: dimmer configuration
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdd_dimmer_pwm_cct_register(IN CHAR_T *name, IN DIMMER_PWM_CFG_T *p_cfg);

#ifdef __cplusplus
}
#endif

#endif /* __TDD_DIMMER_PWM_H__ */
