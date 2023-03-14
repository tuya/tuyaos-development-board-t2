/**
* @file tuya_hal_system.h
* @brief Common process - Initialization
* @version 0.1
* @date 2020-11-09
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/
#ifndef _TUYA_HAL_SYSTEM_H
#define _TUYA_HAL_SYSTEM_H

#include "tal_system.h"
#include "tal_sleep.h"
#include "tal_watchdog.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TY_CPU_SLEEP        TUYA_CPU_SLEEP
#define TY_CPU_DEEP_SLEEP   TUYA_CPU_DEEP_SLEEP


/**
* @brief Get system ticket count
*
* @param VOID
*
* @note This API is used to get system ticket count.
*
* @return system ticket count
*/
#define tuya_hal_get_systemtickcount() tal_system_get_millisecond()

/**
* @brief Get time in MS per system ticket
*
* @param VOID
*
* @note This API is used to get time in MS per system ticket.
*
* @return time in MS
*/
#define tuya_hal_get_tickratems() 1
/**
* @brief System sleep
*
* @param[in] msTime: time in MS
*
* @note This API is used for system sleep.
*
* @return VOID
*/
#define tuya_hal_system_sleep(msTime) tal_system_sleep(msTime)

/**
* @brief System reset
*
* @param VOID
*
* @note This API is used for system reset.
*
* @return VOID
*/
void tuya_hal_system_reset(void);

/**
* @brief Watch dog refresh
*
* @param VOID
*
* @note This API is used to refresh watch dog.
*
* @return VOID
*/
#define tuya_hal_watchdog_refresh() tal_watchdog_refresh()

/**
* @brief Watch dog stop
*
* @param VOID
*
* @note This API is used to stop watch dog.
*
* @return VOID
*/
#define tuya_hal_watchdog_stop() tal_watchdog_stop()

/**
* @brief Get free heap size
*
* @param VOID
*
* @note This API is used for getting free heap size.
*
* @return size of free heap
*/
#define tuya_hal_system_getheapsize() tal_system_get_free_heap_size()

/**
* @brief Get system reset reason
*
* @param VOID
*
* @note This API is used for getting system reset reason.
*
* @return reset reason of system
*/
#define tuya_hal_system_get_rst_info() tal_system_get_reset_reason(NULL)

/**
* @brief Get system extend reset reason
*
* @param[out] ext_info: extend reset reason information
*
* @note This API is used for getting system extend reset reason.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
#define tuya_hal_system_get_rst_ext_info(ext_info) tal_system_get_reset_reason(ext_info)

/**
* @brief Get a random number in the specified range
*
* @param[in] range: range
*
* @note This API is used for getting a random number in the specified range
*
* @return a random number in the specified range
*/
#define tuya_hal_get_trandom_data(range) tal_system_get_random(range)

/**
* @brief Set the low power mode of CPU
*
* @param[in] en: enable switch
* @param[in] en: cpu sleep mode
*
* @note This API is used for setting the low power mode of CPU.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
#define tuya_hal_set_cpu_lp_mode(en, mode) tal_cpu_sleep_mode_set(en, mode)

/**
* @brief Watch dog init and start
*
* @param[in] timeval: interval time for watch dog detection
*
* @note This API is used to init and start watch dog.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
UINT_T tuya_hal_watchdog_init_start(const unsigned int timeval);

int tuya_hal_get_random_data(const unsigned int range);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

