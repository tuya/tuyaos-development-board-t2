
/**
 * @file uni_thread.h
 * @brief tuya thread module
 * @version 1.0
 * @date 2021-09-09
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */


#ifndef _UNI_THREAD_H
#define _UNI_THREAD_H

#include "tuya_cloud_types.h"
#include "tal_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_THREAD_NAME_LEN     16

typedef PVOID_T THRD_HANDLE;

typedef INT_T THRD_STA;
#define STATE_EMPTY 0       // thread is inited
#define STATE_RUNNING 1     // thread is running
#define STATE_STOP 2        // thread is stop
#define STATE_DELETE 3      // thread delete status 

typedef unsigned int TRD_PRI;
#define TRD_PRIO_0     5    // high
#define TRD_PRIO_1     4
#define TRD_PRIO_2     3
#define TRD_PRIO_3     2
#define TRD_PRIO_4     1
#define TRD_PRIO_5     0
#define TRD_PRIO_6     0    // low

typedef THREAD_CFG_T THRD_PARAM_S;
typedef THREAD_ENTER_CB P_CONSTRUCT_FUNC;
typedef THREAD_EXIT_CB P_EXTRACT_FUNC;

#define CreateAndStart(pThrdHandle, enter, exit, pThrdFunc, pThrdFuncArg, thrd_param)   \
    tal_thread_create_and_start(pThrdHandle, enter, exit, pThrdFunc, pThrdFuncArg, thrd_param)

#define DeleteThrdHandle(thrdHandle)                    \
    tal_thread_delete(thrdHandle)

#define ThreadRunSelfSpace(thrdHandle, bl)              \
    tal_thread_is_self(thrdHandle, bl)

#define GetThrdSta(thrdHandle)                          \
    (THRD_STA)tal_thread_get_state(thrdHandle)

#define DumpAllThrdWatermark                            \
    tal_thread_dump_stack

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

