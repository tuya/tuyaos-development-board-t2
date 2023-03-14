/**
* @file tuya_rtc.h
* @brief Common process - driver rtc
* @version 0.1
* @date 2019-08-20
*
* @copyright Copyright 2019-2021 Tuya Inc. All Rights Reserved.
*
*/

#ifndef __TUYA_RTC_H__
#define __TUYA_RTC_H__

#include "tal_rtc.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief rtc init
 *
 */
#define  tuya_rtc_init()     tal_rtc_init()

/**
 * @brief time set
 * 
 * @param[in] timestamp time
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define  tuya_rtc_time_set(timestamp)   tal_rtc_time_set(timestamp)

/**
 * @brief time get
 * 
 * @param[in] timestamp time
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define  tuya_rtc_time_get(timestamp)   tal_rtc_time_get(timestamp)

/**
 * @brief rtc deinit
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define  tuya_rtc_deinit() tal_rtc_deinit()

#ifdef __cplusplus
}
#endif

#endif
