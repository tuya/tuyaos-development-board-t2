/**
* @file uni_time_queue.h
* @author nzy@tuya.com
* @brief Common process - Initialization
* @version 0.1
* @date 2020-11-09
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/
#ifndef _UNI_TIME_QUEUE_H
#define _UNI_TIME_QUEUE_H


#include "tuya_cloud_types.h"
#include "tal_sw_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

//the handle of the timer queue
typedef PVOID_T TIMER_QUE_HANDLE;

//the timer's processing function
typedef VOID (* P_TIMER_FUNC)(TIMER_ID timerID,PVOID_T pTimerArg);

#define TIMER_ONCE TAL_TIMER_ONCE
#define TIMER_CYCLE TAL_TIMER_CYCLE

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
