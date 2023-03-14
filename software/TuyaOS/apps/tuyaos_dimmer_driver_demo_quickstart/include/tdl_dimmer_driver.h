/**
 * @file tdl_dimmer_driver.h
 * @author www.tuya.com
 * @brief tdl_dimmer_driver module is used to provid a unified
 *        dimmer driver access standard for the driver layer
 * @version 0.1
 * @date 2022-06-01
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#ifndef __TDL_DIMMER_DRIVER_H__
#define __TDL_DIMMER_DRIVER_H__

#include "tdu_light_types.h"

#ifdef __cpluspluss
extern "C" {
#endif

/***********************************************************
************************macro define************************
***********************************************************/
typedef UCHAR_T DIMMER_DRIVER_TYPE_E;
#define DIMMER_DRIVER_PWM_CW                      0
#define DIMMER_DRIVER_PWM_CCT                     1
#define DIMMER_DRIVER_IIC                         2

/***********************************************************
***********************typedef define***********************
***********************************************************/

/**
 * @brief driver handle
 */
typedef VOID_T* DIMMER_DRV_HANDLE;

/**
 * @brief driver interfaces abstraction
 */
typedef struct {
    OPERATE_RET (*open  )(IN DIMMER_DRV_HANDLE drv_handle);
    OPERATE_RET (*close )(IN DIMMER_DRV_HANDLE drv_handle);
    OPERATE_RET (*output)(IN DIMMER_DRV_HANDLE drv_handle, LIGHT_RGBCW_U *p_rgbcw);
} DIMMER_DRV_INTFS_T;

/***********************************************************
********************function declaration********************
***********************************************************/
/**
 * @brief register dimmer
 *
 * @param[in] name: dimmer name
 * @param[in] p_intfs: interfaces for operating the dimmer
 * @param[in] drv_resolution: the resolution of the dimmer driver
 * @param[in] tp: dimmer driver type
 * @param[in] drv_handle: dimmer driver handle
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tdl_dimmer_register(IN CHAR_T *name, IN DIMMER_DRV_INTFS_T *p_intfs, USHORT_T drv_resolution,\
                                DIMMER_DRIVER_TYPE_E tp, IN DIMMER_DRV_HANDLE drv_handle);

#ifdef __cplusplus
}
#endif

#endif /* __TDL_DIMMER_DRIVER_H__ */
