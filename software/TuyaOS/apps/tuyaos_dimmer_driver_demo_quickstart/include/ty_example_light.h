/**
 * @file ty_example_light.h
 * @author www.tuya.com
 * @version 0.1
 * @date 2023-03-09
 *
 * @copyright Copyright (c) tuya.inc 2023
 *
 */

#ifndef __TY_EXAMPLE_LIGHT_H__
#define __TY_EXAMPLE_LIGHT_H__

#include "tal_log.h"
#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
*********************** macro define ***********************
***********************************************************/
#define LIGHT_DEV_NAME         "my_device"
#define LIGHT_RESOLUTION       1000         //颜色分辨率

/***********************************************************
********************** typedef define **********************
***********************************************************/


/***********************************************************
******************* function declaration *******************
***********************************************************/
VOID_T ty_example_light(VOID_T);

OPERATE_RET dimmer_hardware_register(VOID_T);

VOID_T example_dimmer_driver(VOID_T);

#ifdef __cplusplus
}
#endif

#endif /* __TY_EXAMPLE_LIGHT_H__ */
