/**
* @file tuya_hal_semaphore.h
* @brief Common process - Initialization
* @version 0.1
* @date 2020-11-09
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/
#ifndef _TUYA_HAL_SEMAPHORE_H
#define _TUYA_HAL_SEMAPHORE_H

#include "tal_semaphore.h"


#ifdef __cplusplus
extern "C" {
#endif

#define tuya_hal_semaphore_create_init(handle, sem_cnt, sem_max)    \
    tal_semaphore_create_init(handle, sem_cnt, sem_max)

#define tuya_hal_semaphore_wait(handle)                          \
    tal_semaphore_wait(handle, SEM_WAIT_FOREVER)

#define tuya_hal_semaphore_waittimeout(handle, timeout)          \
    tal_semaphore_wait(handle, timeout)

#define tuya_hal_semaphore_post(handle)                          \
    tal_semaphore_post(handle)

#define tuya_hal_semaphore_release(handle)                       \
    tal_semaphore_release(handle)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

