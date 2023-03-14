/**
 * @file example_queue.c
 * @author www.tuya.com
 * @brief example_queue module is used to 
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
#include "tal_queue.h"

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
STATIC THREAD_HANDLE fetch_thrd_hdl = NULL;
STATIC QUEUE_HANDLE queue_hdl = NULL;

/***********************************************************
***********************function define**********************
***********************************************************/

/**
 * @brief queue post task
 *
 * @param[in] : 
 *
 * @return none
 */
STATIC VOID __queue_post_task(PVOID_T args)
{
    OPERATE_RET rt = OPRT_OK;
    CHAR_T post_data[] = {"hello tuya\r\n"};

    for (;;) {
        TUYA_CALL_ERR_LOG(tal_queue_post(queue_hdl, post_data, 0xFFFFFFFF));
        TAL_PR_DEBUG("post queue");

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
 * @brief queue fetch task
 *
 * @param[in] : 
 *
 * @return none
 */
STATIC VOID __queue_fetch_task(PVOID_T args)
{
    OPERATE_RET rt = OPRT_OK;
    CHAR_T fetch_data[16] = {0};

    for (;;) {
        TUYA_CALL_ERR_LOG(tal_queue_fetch(queue_hdl, fetch_data, 0xFFFFFFFF));
        TAL_PR_DEBUG("get queue, data: %s", fetch_data);

        if (THREAD_STATE_STOP == tal_thread_get_state(fetch_thrd_hdl)) {
            break;
        }
    }

    fetch_thrd_hdl=NULL;
    TAL_PR_DEBUG("thread fetch_thrd_hdl will delete");

    return;
}

/**
 * @brief queue example
 *
 * @param[in] : 
 *
 * @return none
 */
VOID example_queue(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;

    if (NULL == queue_hdl) {
        TUYA_CALL_ERR_GOTO(tal_queue_create_init(&queue_hdl, 16*SIZEOF(CHAR_T), 2), __EXIT);
    }

    THREAD_CFG_T thread_cfg = {
        .thrdname = "queue_post",
        .stackDepth = 1024,
        .priority = THREAD_PRIO_2,
    };
    if (NULL == post_thrd_hdl) {
        TUYA_CALL_ERR_GOTO(tal_thread_create_and_start(&post_thrd_hdl, NULL, NULL, \
                                                    __queue_post_task, NULL, &thread_cfg), __EXIT);
    }
    
    thread_cfg.thrdname = "queue_fetch";
    if (NULL == fetch_thrd_hdl) {
        TUYA_CALL_ERR_GOTO(tal_thread_create_and_start(&fetch_thrd_hdl, NULL, NULL, \
                                                    __queue_fetch_task, NULL, &thread_cfg), __EXIT);
    }

__EXIT:
    return;
}

/**
 * @brief stop queue example, delete thread, queue
 *
 * @param[in] : 
 *
 * @return none
 */
VOID example_queue_stop(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;

    if (NULL != fetch_thrd_hdl) {
        TUYA_CALL_ERR_LOG(tal_thread_delete(fetch_thrd_hdl));
    }

    if (NULL != post_thrd_hdl) {
        TUYA_CALL_ERR_LOG(tal_thread_delete(post_thrd_hdl));
    }

    /* 等待线程删除完成，然后在释放队列资源。避免线程还未删除，队列已被释放出现错误 */
    while (NULL != post_thrd_hdl || NULL != fetch_thrd_hdl) {
        tal_system_sleep(500);
    }

    if(NULL != queue_hdl) {
        tal_queue_free(queue_hdl);
        queue_hdl = NULL;
        TAL_PR_DEBUG("queue_hdl free finish");
    }

    return;
}
