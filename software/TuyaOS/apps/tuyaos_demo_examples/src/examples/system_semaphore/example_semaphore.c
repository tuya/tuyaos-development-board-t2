/**
 * @file example_semaphore.c
 * @author www.tuya.com
 * @brief example_semaphore module is used to 
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
#include "tal_semaphore.h"

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
STATIC THREAD_HANDLE post_thrd_hdl = NULL;
STATIC THREAD_HANDLE wait_thrd_hdl = NULL;
STATIC SEM_HANDLE example_sem_hdl = NULL;

/***********************************************************
***********************function define**********************
***********************************************************/

/**
 * @brief semaphore post task
 *
 * @param[in] : 
 *
 * @return none
 */
STATIC VOID __sema_post_task(PVOID_T args)
{
    for (;;) {
        tal_semaphore_post(example_sem_hdl);
        TAL_PR_NOTICE("post semaphore");

        if (THREAD_STATE_STOP == tal_thread_get_state(post_thrd_hdl)) {
            break;
        }
        tal_system_sleep(2000);
    }

    post_thrd_hdl=NULL;
    TAL_PR_DEBUG("thread post_thrd_hdl will delete");

    return;
}

/**
 * @brief semaphore wait task
 *
 * @param[in] : 
 *
 * @return none
 */
STATIC VOID __sema_wait_task(PVOID_T args)
{
    for (;;) {
        tal_semaphore_wait(example_sem_hdl, SEM_WAIT_FOREVER);
        TAL_PR_NOTICE("get semaphore");

        if (THREAD_STATE_STOP == tal_thread_get_state(wait_thrd_hdl)) {
            break;
        }
    }

    wait_thrd_hdl=NULL;
    TAL_PR_DEBUG("thread __sema_wait_task will delete");

    return;
}

/**
 * @brief semaphore example
 *
 * @param[in] : 
 *
 * @return none
 */
VOID example_semaphore(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;
    if (NULL == example_sem_hdl) {
        TUYA_CALL_ERR_GOTO(tal_semaphore_create_init(&example_sem_hdl, 0, 1), __EXIT);
    }

    THREAD_CFG_T thread_cfg = {
        .thrdname = "sema_post",
        .stackDepth = 1024,
        .priority = THREAD_PRIO_2,
    };
    if (NULL == post_thrd_hdl) {
        TUYA_CALL_ERR_GOTO(tal_thread_create_and_start(&post_thrd_hdl, NULL, NULL, __sema_post_task, NULL, &thread_cfg), __EXIT);
    }

    thread_cfg.thrdname = "sema_wait";
    if (NULL == wait_thrd_hdl) {
        TUYA_CALL_ERR_GOTO(tal_thread_create_and_start(&wait_thrd_hdl, NULL, NULL, __sema_wait_task, NULL, &thread_cfg), __EXIT);
    }

__EXIT:
    return;
}

/**
 * @brief stop semaphore example, delete thread semaphore
 *
 * @param[in] : 
 *
 * @return none
 */
VOID example_semaphore_stop(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;

    if (NULL != wait_thrd_hdl) {
        TUYA_CALL_ERR_LOG(tal_thread_delete(wait_thrd_hdl));
    }

    if (NULL != post_thrd_hdl) {
        TUYA_CALL_ERR_LOG(tal_thread_delete(post_thrd_hdl));
    }

    /* 等待线程删除完成，然后在释放队列资源。避免线程还未删除，队列已被释放出现错误 */
    while (NULL != post_thrd_hdl || NULL != wait_thrd_hdl) {
        tal_system_sleep(500);
    }

    if(NULL != example_sem_hdl) {
        tal_semaphore_release(example_sem_hdl);
        example_sem_hdl = NULL;
        TAL_PR_DEBUG("example_sem_hdl free finish");
    }

    return;
}
