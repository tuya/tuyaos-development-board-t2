/**
* @file tuya_device.c
* @author www.tuya.com
* @brief tuya_device module is used to 
* @version 0.1
* @date 2022-05-20
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include "tuya_cloud_types.h"

#include "tal_log.h"
#include "tkl_output.h"
#include "tkl_timer.h"

/***********************************************************
*************************micro define***********************
***********************************************************/
#define DELAY_TIME    500*1000    //us

/***********************************************************
***********************typedef define***********************
***********************************************************/


/***********************************************************
***********************variable define**********************
***********************************************************/
#define TIMER_ID        TIMER_NUM_0

STATIC CHAR_T sg_count = 0;

/***********************************************************
***********************function define**********************
***********************************************************/

/**
* @brief Timer callback function
*
* @param[in] arg:parameters
* @return none
*/
STATIC VOID_T __timer_callback(VOID *args)
{
    /* TAL_PR_ , PR_ ，这两种打印里面有锁，不要在中断里使用 */
    tkl_log_output("\r\n------------- Timer Callback --------------\r\n");
    sg_count++;

    if(sg_count >= 5) {
        sg_count=0;
        tkl_timer_stop(TIMER_ID);
        tkl_timer_deinit(TIMER_ID);
        tkl_log_output("\r\ntimer %d is stop\r\n", TIMER_ID);
    }

    return;
}

/**
* @brief example_pwm_start_and_stop
*
* @return none
*/
VOID example_timer(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;

    TUYA_TIMER_BASE_CFG_E sg_timer_cfg = {
        .mode = TUYA_TIMER_MODE_PERIOD,
        .args = NULL,
        .cb = __timer_callback
    };

    TUYA_CALL_ERR_GOTO(tkl_timer_init(TIMER_ID, &sg_timer_cfg), __EXIT);

    /*start timer*/
    TUYA_CALL_ERR_GOTO(tkl_timer_start(TIMER_ID, DELAY_TIME), __EXIT);
    TAL_PR_NOTICE("timer %d is start", TIMER_ID);

__EXIT:
    return;
}
