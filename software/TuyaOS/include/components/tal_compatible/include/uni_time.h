/**
 * @file uni_time.h
 * @brief tuya time module
 * @version 1.0
 * @date 2019-10-30
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */

#ifndef _UNI_TIME_H
#define _UNI_TIME_H

#include "tal_time_service.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief time-management module initialization
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h    
 */
#define uni_time_init() tal_time_service_init()

/**
 * @brief change posix time to TIME_T, redefine the std C func mktime
 * 
 * @param[in] tm the time in posix time format
 * @return the time in TIME_T format 
 */
#define uni_mktime(tm) tal_time_mktime(tm)

/**
 * @brief change TIME_T to posix time, redefine the std C func gmtime_r
 * 
 * @param[in] tm the time in TIME_T format
 * @param[out] result the time in posix time format
 * @return the time in posix time format
 */
#define uni_gmtime_r(tm, result) tal_time_gmtime_r(tm, result)

/**
 * @brief change http-GMT format time to TIME_T
 * 
 * @param[in] date http-GMT format time
 * @return the time in TIME_T format
 */
//TIME_T uni_http_date_to_time(IN CONST CHAR_T *date);
//废弃

/**
 * @brief check IoTOS time synchronize status
 * 
 * @return OPRT_OK on synchronized. Others on not
 */
#define uni_time_check_time_sync() tal_time_check_time_sync()

/**
 * @brief check IoTOS time zone synchronized status
 * 
 * @return OPRT_OK on synchronized. Others on not 
 */
#define uni_time_check_time_zone_sync() tal_time_check_time_zone_sync()

/**
 * @brief set IoTOS UTC time
 * 
 * @param[in] tm the new time in posix time format
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h   
 */
#define uni_time_set(tm) tal_time_set(tm)

/**
 * @brief set IoTOS UTC time in time_t format
 * 
 * @param[in] time the new time in time_t format
 * @param[in] update_source the source of time (0:rtc 1:cloud 2:other)
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h    
 */
#define uni_time_set_posix(time, update_source) tal_time_set_posix(time, update_source)

/**
 * @brief get IoTOS UTC time in posix time format
 * 
 * @param[out] tm the IoTOS UTC time in posix time format
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h   
 */
#define uni_time_get(tm) tal_time_get(tm)

/**
 * @brief get IoTOS UTC time in TIME_T format
 * 
 * @return the current second time in TIME_T format 
 */
#define uni_time_get_posix() tal_time_get_posix()

/**
 * @brief get IoTOS UTC time in SYS_TICK_T format
 * 
 * @return the current micro-second time in SYS_TICK_T format 
 */
#define uni_time_get_posix_ms() tal_time_get_posix_ms()

/**
 * @brief get IoTOS last synchronized UTC time in TIME_T format
 * 
 * @return the time in TIME_T format 
 */
#define uni_time_get_cur_posix() tal_time_get_cur_posix()

/**
 * @brief get IoTOS UTC time remain micro-second time
 * 
 * @return the remain micro-second time
 * 
 * @note in 32bit process, cannot return 8byte, because the high 4byte will be cut by cpu, we should use:
 * "uint64_t ms = uni_time_get_posix()*1000ULL + uni_time_get_posix_ms_remain_tick()" to get the micro-seconds
 */
//#define uni_time_get_posix_ms_remain_tick() uni_time_get_posix_ms_remain_tick()
// 废弃

/**
 * @brief get IoTOS UTC time, both second and micro-second
 * 
 * @param[out] pSecTime the current time in second
 * @param[out] pMsTime the current time in micro-second
 * @return VOID 
 */
#define uni_get_system_time(pSecTime, pMsTime) tal_time_get_system_time(pSecTime, pMsTime)

/**
 * @brief set IoTOS time zone in "+/-hh:mm" format
 * 
 * @param[in] time_zone the new time zone in "+/-hh:mm" format
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h     
 */
#define uni_set_time_zone(time_zone) tal_time_set_time_zone(time_zone)

/**
 * @brief set IoTOS time zone in second format
 * 
 * @param[in] time_zone the new time zone in second format
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h     
 */
#define uni_set_time_zone_seconds(time_zone_sec) tal_time_set_time_zone_seconds(time_zone_sec)

/**
 * @brief get IoTOS time zone in second format
 * 
 * @param[out] time_zone the current time zone in second format
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h     
 */
#define uni_get_time_zone_seconds(time_zone) tal_time_get_time_zone_seconds(time_zone)

/**
 * @brief set IoTOS summer time zone
 * 
 * @param[in] zone the summer time zone table
 * @param[in] cnt the summer time counts
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define uni_set_sum_zone_tbl(zone, cnt) tal_time_set_sum_zone_tbl(zone, cnt)

/**
 * @brief get IoTOS UTC summer time in posix time format
 * 
 * @param[out] tm the summer time in posix format
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define uni_sum_time_get(tm) tal_time_get_sum_time(tm)

/**
 * @brief get IoTOS UTC summer time int TIME_T format 
 * 
 * @return the summer time in TIME_T format
 */
#define uni_sum_time_get_posix() tal_time_get_sum_time_posix()

/**
 * @brief get IoTOS UTC summer time int TIME_T format 
 * 
 * @return the summer time in TIME_T format
 */
#define uni_is_in_sum_zone(time) tal_time_is_in_sum_zone(time)


/**
 * @brief get IoTOS local time (local, contains the time zone and summer time zone)
 * 
 * @param[out] tm the current local time in posix format
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h     
 */
#define uni_local_time_get(tm) tal_time_get_local_time_custom(0, tm)

/**
 * @brief get IoTOS local time (local, contains the time zone and summer time zone)
 * 
 * @param[in] in_time the time need translate
 * @param[out] tm the local time in posix format
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 *
 * @note if in_time is 0, return the IoTOS local time, otherwise, translate the in_time to
 * local time
 */
#define uni_local_time_get_custom(in_time, tm) tal_time_get_local_time_custom(in_time, tm)

/**
 * @brief get sum zone info
 * 
 * @param[out] sum zone info
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h     
 */
#define tuya_uni_get_sum_zone(sum_zone) tal_time_get_sum_zone(sum_zone)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _UNI_TIME_H */


