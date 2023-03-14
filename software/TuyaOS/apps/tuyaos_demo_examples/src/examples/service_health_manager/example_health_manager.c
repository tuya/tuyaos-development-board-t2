/**
 * @file example_health_manager.c
 * @author www.tuya.com
 * @brief example_health_manager module is used to 
 * @version 0.1
 * @date 2022-09-20
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include "tuya_cloud_types.h"
#include "tuya_devos_health.h"

#include "tal_log.h"
#include "tkl_memory.h"

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
INT_T my_health_manage_id = 0;

/***********************************************************
***********************function define**********************
***********************************************************/

STATIC VOID_T health_manage_notify_callback(VOID)
{
    TAL_PR_NOTICE("Health manage number of errors reach the threshold number.");

    return;
}

STATIC BOOL_T health_manage_query_cb(VOID)
{
    /* check... */

    TAL_PR_NOTICE("Find error, return TRUE.");

    return TRUE;
}

VOID example_health_manage(INT_T argc, CHAR_T *argv[])
{
    TAL_PR_NOTICE("Make sure you have called \"example_soc_init\" before executing this example.");

    my_health_manage_id = tuya_devos_add_health_item(3, 5, health_manage_query_cb, health_manage_notify_callback);
    if (my_health_manage_id < 0) {
        TAL_PR_ERR("health manage add event item failed: %d", my_health_manage_id);
    }

    return;
}
