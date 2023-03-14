/**
 * @file example_sw_timer.c
 * @author www.tuya.com
 * @brief example_sw_timer module is used to 
 * @version 0.1
 * @date 2022-11-21
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include "tuya_cloud_types.h"
#include "tal_log.h"
#include "tal_sw_timer.h"

/***********************************************************
************************macro define************************
***********************************************************/


/***********************************************************
***********************typedef define***********************
***********************************************************/


/***********************************************************
********************function declaration********************
***********************************************************/


/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC TIMER_ID sw_timer_id = NULL;
STATIC UINT8_T timer_run_cnt = 0;

/***********************************************************
***********************function define**********************
***********************************************************/

/**
 * @brief software timer callback
 *
 * @param[in] : 
 *
 * @return none
 */
STATIC VOID_T __timer_cb(TIMER_ID timer_id, VOID_T *arg)
{
    TAL_PR_NOTICE("--- tal sw timer callback");
    timer_run_cnt++;
    if (timer_run_cnt >= 5) {
        TAL_PR_NOTICE("stop and delete software timer");
        timer_run_cnt=0;
        tal_sw_timer_stop(sw_timer_id);
        tal_sw_timer_delete(sw_timer_id);
    }
}

/**
 * @brief software timer example
 *
 * @param[in] : 
 *
 * @return none
 */
VOID example_sw_timer(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;

    TUYA_CALL_ERR_GOTO(tal_sw_timer_create(__timer_cb, NULL, &sw_timer_id), __EXIT);

    TAL_PR_DEBUG("sw timer start");
    TUYA_CALL_ERR_LOG(tal_sw_timer_start(sw_timer_id, 3000, TAL_TIMER_CYCLE));

__EXIT:
    return;
}