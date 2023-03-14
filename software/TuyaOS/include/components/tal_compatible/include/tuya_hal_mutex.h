/**
* @file tuya_hal_mutex.h
* @brief Common process - Initialization
* @version 0.1
* @date 2021-09-09
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/
#ifndef _TUYA_HAL_MUTEX_H
#define _TUYA_HAL_MUTEX_H

#include "tal_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

#define tuya_hal_mutex_create_init(handle)            \
    tal_mutex_create_init(handle)

#define tuya_hal_mutex_lock(handle)                    \
    tal_mutex_lock(handle)

#define tuya_hal_mutex_unlock(handle)                  \
    tal_mutex_unlock(handle)

#define tuya_hal_mutex_release(handle)                 \
    tal_mutex_release(handle)


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
