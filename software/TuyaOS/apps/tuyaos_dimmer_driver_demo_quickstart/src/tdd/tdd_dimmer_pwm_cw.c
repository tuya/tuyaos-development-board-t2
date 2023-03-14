/**
 * @file tdd_dimmer_pwm.c
 * @author www.tuya.com
 * @brief tdd_dimmer_pwm module is used to drive led by PWM
 * @version 0.1
 * @date 2022-08-26
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */
#include <string.h>

#include "tkl_pwm.h"
#include "tal_memory.h"
#include "tal_log.h"

#include "tdl_dimmer_driver.h"
#include "tdd_dimmer_pwm.h"
/***********************************************************
************************macro define************************
***********************************************************/


/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief open pwm dimmer
 *
 * @param[in] drv_handle: dimmer driver handle
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
STATIC OPERATE_RET __tdd_dimmer_pwm_open(IN DIMMER_DRV_HANDLE drv_handle)
{
    OPERATE_RET rt = OPRT_OK;
    UINT_T i = 0;
    TUYA_PWM_BASE_CFG_T pwm_cfg = {0};
    DIMMER_PWM_CFG_T *p_drv = (DIMMER_PWM_CFG_T *)drv_handle;

    if(NULL == p_drv) {
        return OPRT_INVALID_PARM;
    }

    memset((UCHAR_T *)&pwm_cfg, 0x00, SIZEOF(pwm_cfg));

    pwm_cfg.frequency = p_drv->pwm_freq;
    pwm_cfg.polarity  = (FALSE == p_drv->active_level) ? TUYA_PWM_NEGATIVE : TUYA_PWM_POSITIVE;
    pwm_cfg.duty      = (FALSE == p_drv->active_level) ? DIMMER_PWM_DUTY_MAX : 0;

    for (i = 0; i < DIMMER_PWM_NUM_MAX; i++) {
        if(DIMMER_PWM_ID_INVALID == p_drv->pwm_ch_arr[i]) {
            continue;
        }

        TUYA_CALL_ERR_RETURN(tkl_pwm_init(p_drv->pwm_ch_arr[i], &pwm_cfg));
    }

    return OPRT_OK;
}

/**
 * @brief close pwm dimmer
 *
 * @param[in] drv_handle: dimmer driver handle
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
STATIC OPERATE_RET __tdd_dimmer_pwm_close(IN DIMMER_DRV_HANDLE drv_handle)
{
    OPERATE_RET rt = OPRT_OK;
    UINT_T i = 0;
    DIMMER_PWM_CFG_T *p_drv = (DIMMER_PWM_CFG_T *)drv_handle;

    if(NULL == p_drv) {
        return OPRT_INVALID_PARM;
    }

    for (i = 0; i < DIMMER_PWM_NUM_MAX; i++) {
        if(DIMMER_PWM_ID_INVALID == p_drv->pwm_ch_arr[i]) {
            continue;
        }

        if(DIMMER_PWM_CH_IDX_COLD == i && DIMMER_PWM_ID_INVALID != p_drv->pwm_ch_arr[DIMMER_PWM_CH_IDX_WARM]) {
            ;
        }else if(DIMMER_PWM_CH_IDX_WARM == i && DIMMER_PWM_ID_INVALID != p_drv->pwm_ch_arr[DIMMER_PWM_CH_IDX_COLD]){
            TUYA_CALL_ERR_RETURN(tkl_pwm_multichannel_stop(&p_drv->pwm_ch_arr[DIMMER_PWM_CH_IDX_COLD], 2));
        }else {
            TUYA_CALL_ERR_RETURN(tkl_pwm_stop(p_drv->pwm_ch_arr[i]));
        }

        TUYA_CALL_ERR_RETURN(tkl_pwm_deinit(p_drv->pwm_ch_arr[i]));
    }

    return OPRT_OK;
}

/**
 * @brief control dimmer output
 *
 * @param[in] drv_handle: driver handle
 * @param[in] p_rgbcw: the value of the value
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
STATIC OPERATE_RET __tdd_dimmer_pwm_output(DIMMER_DRV_HANDLE drv_handle, LIGHT_RGBCW_U *p_rgbcw)
{
    DIMMER_PWM_CFG_T *p_drv = (DIMMER_PWM_CFG_T *)drv_handle;
    USHORT_T pwm_duty = 0, i = 0;

    for (i = 0; i < DIMMER_PWM_NUM_MAX; i++) {
        if(DIMMER_PWM_ID_INVALID == p_drv->pwm_ch_arr[i]) {
            continue;
        }

        pwm_duty = (TRUE == p_drv->active_level) ? p_rgbcw->array[i] :\
                                                  (DIMMER_PWM_DUTY_MAX - p_rgbcw->array[i]);

        tkl_pwm_duty_set(p_drv->pwm_ch_arr[i], pwm_duty);

        if(DIMMER_PWM_CH_IDX_COLD == i && DIMMER_PWM_ID_INVALID != p_drv->pwm_ch_arr[DIMMER_PWM_CH_IDX_WARM]) {
            ;
        }else if(DIMMER_PWM_CH_IDX_WARM == i && DIMMER_PWM_ID_INVALID != p_drv->pwm_ch_arr[DIMMER_PWM_CH_IDX_COLD]) {
            if (0 != (p_rgbcw->s.cold + p_rgbcw->s.warm)) {
                tkl_pwm_multichannel_start(&p_drv->pwm_ch_arr[DIMMER_PWM_CH_IDX_COLD], 2);
            }else {
                tkl_pwm_multichannel_stop(&p_drv->pwm_ch_arr[DIMMER_PWM_CH_IDX_COLD], 2);
            }
        }else {
            if(0 != p_rgbcw->array[i]){
                tkl_pwm_start(p_drv->pwm_ch_arr[i]);
            } else {
                tkl_pwm_stop(p_drv->pwm_ch_arr[i]);
            }
        }
    }

    return OPRT_OK;
}

/**
 * @brief register pwm dimmer
 *
 * @param[in] name: dimmer name
 * @param[in] p_cfg: dimmer configuration
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdd_dimmer_pwm_cw_register(IN CHAR_T *name, IN DIMMER_PWM_CFG_T *p_cfg)
{
    DIMMER_DRV_INTFS_T intfs;

    if ((NULL == p_cfg) || (p_cfg->pwm_freq < DIMMER_PWM_FREQ_MIN) || (p_cfg->pwm_freq > DIMMER_PWM_FREQ_MAX)) {
        return OPRT_INVALID_PARM;
    }

    DIMMER_PWM_CFG_T *p_drv = tal_malloc(SIZEOF(DIMMER_PWM_CFG_T));
    if (NULL == p_drv) {
        return OPRT_MALLOC_FAILED;
    }
    memset(p_drv, 0, SIZEOF(DIMMER_PWM_CFG_T));

    p_drv->pwm_freq     = p_cfg->pwm_freq;
    p_drv->active_level = p_cfg->active_level;

    memcpy((UCHAR_T *)p_drv->pwm_ch_arr, (UCHAR_T *)p_cfg->pwm_ch_arr, SIZEOF(p_drv->pwm_ch_arr));

    intfs.open   = __tdd_dimmer_pwm_open;
    intfs.close  = __tdd_dimmer_pwm_close;
    intfs.output = __tdd_dimmer_pwm_output;

    return tdl_dimmer_register(name, &intfs, DIMMER_PWM_DUTY_MAX, DIMMER_DRIVER_PWM_CW, (DIMMER_DRV_HANDLE)p_drv);
}
