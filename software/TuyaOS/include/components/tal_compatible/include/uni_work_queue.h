#ifndef __UNI_WORK_QUEUE_H__
#define __UNI_WORK_QUEUE_H__

#include "ty_work_queue.h"
#include "tal_thread.h"
#include "tal_workqueue.h"

typedef PVOID_T WORK_QUEUE_HANDLE;
typedef VOID (*WK_CALLBACK)(VOID *data);

STATIC INLINE OPERATE_RET work_queue_create(WORK_QUEUE_HANDLE *phand, UINT_T stack_size, INT_T pri, UINT_T work_num)
{
    THREAD_CFG_T thread_cfg = {
        .stackDepth = stack_size,
        .priority = pri,
        .thrdname = "wk_th-u"
    };
    return tal_workqueue_create(work_num, &thread_cfg, phand);
}


STATIC INLINE OPERATE_RET work_queue_add(WORK_QUEUE_HANDLE hand, WK_CALLBACK cb, VOID *data)
{
    return tal_workqueue_schedule(hand, (TY_WK_CB)cb, (VOID *)data);
}

#endif
