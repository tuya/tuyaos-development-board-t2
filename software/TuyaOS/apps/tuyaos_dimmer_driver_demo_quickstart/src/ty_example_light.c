/**
 * @file ty_example_light.c
 * @author www.tuya.com
 * @version 0.1
 * @date 2023-03-09
 *
 * @copyright Copyright (c) tuya.inc 2023
 *
 */

#include "tdl_dimmer_manage.h"

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
STATIC TDL_DIMMER_HANDLE sg_dimmer_hdl = NULL;

/***********************************************************
********************** function define *********************
***********************************************************/
OPERATE_RET dimmer_open_device(VOID_T)
{
    OPERATE_RET rt =  OPRT_OK;

    TUYA_CALL_ERR_RETURN(tdl_dimmer_find(LIGHT_DEV_NAME, &sg_dimmer_hdl));

    TUYA_CALL_ERR_RETURN(tdl_dimmer_open(sg_dimmer_hdl, LIGHT_RESOLUTION));

    return OPRT_OK;
}


VOID_T ty_example_light(VOID_T)
{
    OPERATE_RET rt =  OPRT_OK;

    TUYA_CALL_ERR_LOG(dimmer_hardware_register());

    TUYA_CALL_ERR_LOG(dimmer_open_device());

    //驱动测试
    example_dimmer_driver();

    return;
}