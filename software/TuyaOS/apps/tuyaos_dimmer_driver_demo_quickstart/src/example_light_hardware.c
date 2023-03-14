/**
 * @file example_light_hardware.c
 * @author www.tuya.com
 * @version 0.1
 * @date 2023-03-09
 *
 * @copyright Copyright (c) tuya.inc 2023
 *
 */

#include "tdu_light_types.h"

#include "tdd_dimmer_pwm.h"
#include "tdd_dimmer_bp1658cj.h"
#include "tdd_dimmer_bp5758d.h"
#include "tdd_dimmer_kp1805x.h"
#include "tdd_dimmer_sm2x35egh.h"
#include "tdd_dimmer_sm2135e.h"
#include "tdd_dimmer_sm2135ex.h"

#include "ty_example_light.h"
/***********************************************************
*********************** macro define ***********************
***********************************************************/


/***********************************************************
********************** typedef define **********************
***********************************************************/


/***********************************************************
********************** variable define *********************
***********************************************************/
STATIC DIMMER_PWM_CFG_T sg_pwm_cfg = {
    .active_level = TRUE,
    .pwm_freq  = 1000,
    .pwm_ch_arr = {
        PWM_NUM_5,
        PWM_NUM_4,
        PWM_NUM_0,
        PWM_NUM_1,
        PWM_NUM_2,
    },
};

STATIC TDD_I2C_PIN_T sg_i2c_pin = {
    .scl = GPIO_NUM_26,
    .sda = GPIO_NUM_6,
};

STATIC DIMMER_CH_CFG_U sg_channel_cfg = {
    .r = DIMMER_CH_ID_3,
    .g = DIMMER_CH_ID_2,
    .b = DIMMER_CH_ID_1,
    .c = DIMMER_CH_ID_5,
    .w = DIMMER_CH_ID_4,
};

STATIC BP1658CJ_CFG_T sg_bp1658cj_cfg = {
    .max_curr_code.color = 10,
    .max_curr_code.white = 10,
};

STATIC BP5758D_CFG_T sg_bp5758d_cfg = {
    .max_curr_code.r = 10,
    .max_curr_code.g = 10,
    .max_curr_code.b = 10,
    .max_curr_code.c = 10,   
    .max_curr_code.w = 10,
};

STATIC DIMMER_KP1805X_CFG_T sg_kp18058_cfg = {
    .max_curr_code.color = 2,
    .max_curr_code.white = 4,
};

STATIC DIMMER_KP1805X_CFG_T sg_kp18059_cfg = {
    .max_curr_code.color = 2,
    .max_curr_code.white = 4,
};

STATIC SM2X35EGH_CFG_T sg_sm2x35egh_cfg = {
    .max_curr_code.color = 2,
    .max_curr_code.white = 4,
};

STATIC SM2135E_CFG_T sg_sm2135e_cfg = {
    .max_curr_code.color = 2,
    .max_curr_code.white = 4,
};

STATIC SM2135EX_CFG_T sg_sm2135ex_cfg = {
    .max_curr_code.color = 2,
    .max_curr_code.white = 4,
};

/***********************************************************
********************** function define *********************
***********************************************************/
OPERATE_RET dimmer_hardware_register(VOID_T)
{
    OPERATE_RET rt =  OPRT_OK;

    // TUYA_CALL_ERR_RETURN(tdd_dimmer_bp1658cj_register(LIGHT_DEV_NAME, &sg_i2c_pin, &sg_channel_cfg, &sg_bp1658cj_cfg));

    TUYA_CALL_ERR_RETURN(tdd_dimmer_bp5758d_register(LIGHT_DEV_NAME, &sg_i2c_pin, &sg_channel_cfg, &sg_bp5758d_cfg)); 

    // TUYA_CALL_ERR_RETURN(tdd_dimmer_kp18058_register(LIGHT_DEV_NAME, &sg_i2c_pin, &sg_channel_cfg, &sg_kp18058_cfg));

    // TUYA_CALL_ERR_RETURN(tdd_dimmer_kp18059_register(LIGHT_DEV_NAME, &sg_i2c_pin, &sg_channel_cfg, &sg_kp18059_cfg));

    // TUYA_CALL_ERR_RETURN(tdd_dimmer_sm2x35egh_register(LIGHT_DEV_NAME, &sg_i2c_pin, &sg_channel_cfg, &sg_sm2x35egh_cfg));

    // TUYA_CALL_ERR_RETURN(tdd_dimmer_sm2135e_register(LIGHT_DEV_NAME, &sg_i2c_pin, &sg_channel_cfg, &sg_sm2135e_cfg));

    // TUYA_CALL_ERR_RETURN(tdd_dimmer_sm2135ex_register(LIGHT_DEV_NAME, &sg_i2c_pin, &sg_channel_cfg, &sg_sm2135ex_cfg));

    // TUYA_CALL_ERR_RETURN(tdd_dimmer_pwm_cw_register(LIGHT_DEV_NAME, &sg_pwm_cfg));

    // TUYA_CALL_ERR_RETURN(tdd_dimmer_pwm_cct_register(LIGHT_DEV_NAME, &sg_pwm_cfg));

    return OPRT_OK;
}