/**
 * @file example_light_driver.c
 * @author www.tuya.com
 * @brief example_light_driver module is used to 
 * @version 0.1
 * @date 2023-03-09
 *
 * @copyright Copyright (c) tuya.inc 2023
 *
 */
#include "tal_system.h"

#include "tdl_dimmer_manage.h"

#include "ty_example_light.h"
/***********************************************************
*********************** macro define ***********************
***********************************************************/
#define LIGHT_COLOR_VALUE      LIGHT_RESOLUTION

/***********************************************************
********************** typedef define **********************
***********************************************************/


/***********************************************************
********************** variable define *********************
***********************************************************/
STATIC TDL_DIMMER_HANDLE sg_dimmer_hdl = NULL;
STATIC LIGHT_RGBCW_U sg_color = {
    .s.red = LIGHT_COLOR_VALUE,
};

/***********************************************************
********************** function define *********************
***********************************************************/
VOID_T example_dimmer_driver(VOID_T)
{
    OPERATE_RET rt =  OPRT_OK;
    UINT_T idx = 0;

    TUYA_CALL_ERR_LOG(tdl_dimmer_find(LIGHT_DEV_NAME, &sg_dimmer_hdl));

    while(1) {
        //开
        sg_color.array[idx] = LIGHT_COLOR_VALUE;

        TAL_PR_NOTICE("array[%d] = %d", idx, sg_color.array[idx]);
        TUYA_CALL_ERR_LOG(tdl_dimmer_output(sg_dimmer_hdl, &sg_color.s));

        tal_system_sleep(2000);

        //关
        memset((UCHAR_T *)sg_color.array, 0x00, SIZEOF(LIGHT_RGBCW_U));
        TUYA_CALL_ERR_LOG(tdl_dimmer_output(sg_dimmer_hdl, &sg_color.s));

        tal_system_sleep(1000);

        //切换下一个颜色
        idx = ((idx+1) % LIGHT_COLOR_CHANNEL_MAX);
    }

    return;
}