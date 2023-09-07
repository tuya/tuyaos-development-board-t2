/**
 * @file example_light_hardware.c
 * @author www.tuya.com
 * @version 0.1
 * @date 2023-03-09
 *
 * @copyright Copyright (c) tuya.inc 2023
 *
 */

#include "ty_example_ir.h"
#include "tal_log.h"

#include "tkl_gpio.h"
#include "tdd_ir_driver.h"

/***********************************************************
*********************** macro define ***********************
***********************************************************/


/***********************************************************
********************** typedef define **********************
***********************************************************/


/***********************************************************
********************** variable define *********************
***********************************************************/


/***********************************************************
********************** function define *********************
***********************************************************/
OPERATE_RET ir_hardware_register(VOID_T)
{
    OPERATE_RET rt =  OPRT_OK;

    IR_DRV_CFG_T ir_hw_cfg = {
        .send_pin = GPIO_NUM_7,
        .recv_pin = GPIO_NUM_8,
        .send_timer = TIMER_NUM_0,
        .recv_timer = TIMER_NUM_0,
        .send_duty = 50,
    };

    TUYA_CALL_ERR_RETURN(tdd_ir_driver_register(IR_DEV_NAME, IR_DRV_SINGLE_TIMER, ir_hw_cfg)); 

    return OPRT_OK;
}