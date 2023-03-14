/**
 * @file example_mutex.c
 * @author www.tuya.com
 * @brief example_mutex module is used to 
 * @version 0.1
 * @date 2022-11-21
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include "tuya_cloud_types.h"
#include "tal_log.h"
#include "tal_system.h"
#include "tal_thread.h"
#include "tal_mutex.h"

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
STATIC THREAD_HANDLE mutex1_thrd_hdl = NULL;
STATIC THREAD_HANDLE mutex2_thrd_hdl = NULL;
STATIC MUTEX_HANDLE mutex_hdl = NULL;

STATIC INT32_T temp_value = 0;

/***********************************************************
***********************function define**********************
***********************************************************/

/**
 * @brief mutex task
 *
 * @param[in] : 
 *
 * @return none
 */
STATIC VOID __mutex_task1(PVOID_T args)
{
    for (;;) {
        tal_mutex_lock(mutex_hdl);
        temp_value = temp_value + 2;
        TAL_PR_NOTICE("task1 temp_value: %d", temp_value);
        tal_mutex_unlock(mutex_hdl);
        tal_system_sleep(2000);

        if (THREAD_STATE_STOP == tal_thread_get_state(mutex1_thrd_hdl)) {
            break;
        }
    }

    mutex1_thrd_hdl=NULL;
    TAL_PR_DEBUG("thread mutex1_thrd_hdl will delete");

    return;
}

/**
 * @brief mutex task
 *
 * @param[in] : 
 *
 * @return none
 */
STATIC VOID __mutex_task2(PVOID_T args)
{
    for (;;) {
        tal_mutex_lock(mutex_hdl);
        temp_value--;
        TAL_PR_NOTICE("task2 temp_value: %d", temp_value);
        tal_mutex_unlock(mutex_hdl);
        tal_system_sleep(2000);

        if (THREAD_STATE_STOP == tal_thread_get_state(mutex2_thrd_hdl)) {
            break;
        }
    }

    mutex2_thrd_hdl=NULL;
    TAL_PR_DEBUG("thread mutex2_thrd_hdl will delete");

    return;
}

/**
 * @brief mutex example
 *
 * @param[in] : 
 *
 * @return none
 */
VOID example_mutex(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;

    if (NULL == mutex_hdl) {
        TUYA_CALL_ERR_GOTO(tal_mutex_create_init(&mutex_hdl), __EXIT);
    }

    THREAD_CFG_T thread_cfg = {
        .thrdname = "example_mutex1",
        .stackDepth = 1024,
        .priority = THREAD_PRIO_2,
    };
    if (NULL == mutex1_thrd_hdl) {
        TUYA_CALL_ERR_GOTO(tal_thread_create_and_start(&mutex1_thrd_hdl, NULL, NULL, __mutex_task1, NULL, &thread_cfg), __EXIT);
    }

    thread_cfg.thrdname = "example_mutex2";
    if (NULL == mutex2_thrd_hdl) {
        TUYA_CALL_ERR_GOTO(tal_thread_create_and_start(&mutex2_thrd_hdl, NULL, NULL, __mutex_task2, NULL, &thread_cfg), __EXIT);
    }
__EXIT:
    return;
}

/**
 * @brief stop mutex example, delete thread, mutex
 *
 * @param[in] : 
 *
 * @return none
 */
VOID example_mutex_stop(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;

    if (NULL != mutex1_thrd_hdl) {
        TUYA_CALL_ERR_LOG(tal_thread_delete(mutex1_thrd_hdl));
    }

    if (NULL != mutex2_thrd_hdl) {
        TUYA_CALL_ERR_LOG(tal_thread_delete(mutex2_thrd_hdl));
    }

    /* 等待线程删除完成，然后在释放队列资源。避免线程还未删除，队列已被释放出现错误 */
    while (NULL != mutex1_thrd_hdl || NULL != mutex2_thrd_hdl) {
        tal_system_sleep(500);
    }

    if(NULL != mutex_hdl) {
        tal_mutex_release(mutex_hdl);
        mutex_hdl = NULL;
        TAL_PR_DEBUG("mutex_hdl free finish");
    }

    return;
}
