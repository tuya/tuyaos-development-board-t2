/**
* @file sys_timer.h
* @author nzy@tuya.com
* @brief Common process - Initialization
* @version 0.1
* @date 2020-11-09
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/
#ifndef _SYS_TIMER_H
#define _SYS_TIMER_H

#include "tuya_cloud_types.h"
#include "uni_time_queue.h"
#include "tal_sw_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define system_timer_init() \
    tal_sw_timer_init()

#define sys_add_timer(pTimerFunc, pTimerArg, p_timerID) \
    tal_sw_timer_create(pTimerFunc, pTimerArg, p_timerID)

#define sys_delete_timer(timerID) \
    tal_sw_timer_delete(timerID)

#define sys_stop_timer(timerID) \
    tal_sw_timer_stop(timerID)

#define IsThisSysTimerRun(timer_id) \
    tal_sw_timer_is_running(timer_id)

#define sys_start_timer(timerID, timeCycle, timer_type) \
    tal_sw_timer_start(timerID, timeCycle, timer_type)

#define sys_trigger_timer(timerID)  \
    tal_sw_timer_trigger(timerID)

#define system_timer_release()  \
    tal_sw_timer_release()

#define sys_get_timer_num() \
    tal_sw_timer_get_num()

#define system_timer_set_sleep_interval(interval)
#define system_timer_get_sleep_interval()   0

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

