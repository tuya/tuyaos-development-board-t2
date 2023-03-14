/**
 * @file example_system_thread.c
 * @author www.tuya.com
 * @brief example_system_thread module is used to 
 * @version 0.1
 * @date 2022-11-21
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include "tuya_cloud_types.h"
#include "tal_log.h"
#include "tal_thread.h"
#include "tal_system.h"

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
STATIC THREAD_HANDLE example_thrd_hdl = NULL;

/***********************************************************
***********************function define**********************
***********************************************************/

/**
 * @brief thread example task
 *
 * @param[in] : 
 *
 * @return none
 */
STATIC VOID example_task(PVOID_T args)
{
    UINT8_T cnt = 0;
    TAL_PR_NOTICE("example task is run...");
    for (;;) {
        TAL_PR_NOTICE("this is example task");
        tal_system_sleep(2000);

        cnt++;
        if (cnt >= 5) {
            break;
        }
    }

    /* 删除线程不要进行异步删除，否则可能无法立马删除掉线程 */
    TAL_PR_NOTICE("example task will delete");
    tal_thread_delete(example_thrd_hdl);

    return;
}

/**
 * @brief thread example
 *
 * @param[in] : 
 *
 * @return none
 */
VOID example_thread(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;

    const THREAD_CFG_T thread_cfg = {
        .thrdname = "example_task",
        .stackDepth = 2*1024,
        .priority = THREAD_PRIO_2,
    };
    TUYA_CALL_ERR_LOG(tal_thread_create_and_start(&example_thrd_hdl, NULL, NULL, example_task, NULL, &thread_cfg));

    return;
}
