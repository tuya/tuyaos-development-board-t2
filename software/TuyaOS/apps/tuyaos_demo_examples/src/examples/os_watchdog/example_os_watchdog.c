/**
* @file examples_os_watchdog.c
* @author www.tuya.com
* @brief examples_os_watchdog module is used to 
* @version 0.1
* @date 2022-05-20
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include "tuya_cloud_types.h"

#include "tal_log.h"
#include "tal_watchdog.h"

/***********************************************************
*************************micro define***********************
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
* @brief watchdog task
*
* @param[in] param:Task parameters
* @return none
*/
VOID example_feed_watchdog(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;

    /* Feed watchdog */
    TUYA_CALL_ERR_LOG(tal_watchdog_refresh());

    return;
}
