/**
* @file tal_sw_timer.h
* @brief Common process - Initialization
* @version 0.1
* @date 2021-08-24
*
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
*
*/
#ifndef __TAL_SW_TIMER_H__
#define __TAL_SW_TIMER_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef PVOID_T TIMER_ID;  // 定时器ID
typedef VOID_T (* TAL_TIMER_CB)(TIMER_ID timer_id, VOID_T *arg);

/**
 * @brief the type of timer
 */
typedef enum {
    TAL_TIMER_ONCE = 0,
    TAL_TIMER_CYCLE,
}TIMER_TYPE;

/**
 * @brief Initializing the software timer
 *
 * @param VOID
 *
 * @note This API is used for initializing the software timer
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_sw_timer_init(VOID_T);

/**
 * @brief create a software timer
 *
 * @param[in] func: the processing function of the timer
 * @param[in] arg: the parameater of the timer function
 * @param[out] timer_id: timer id
 *
 * @note This API is used for create a software timer
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_sw_timer_create(TAL_TIMER_CB func, VOID_T *arg, TIMER_ID *timer_id);

/**
 * @brief Delete the software timer
 *
 * @param[in] timer_id: timer id
 *
 * @note This API is used for deleting the software timer
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_sw_timer_delete(TIMER_ID timer_id);

/**
 * @brief Stop the software timer
 *
 * @param[in] timer_id: timer id
 *
 * @note This API is used for stopping the software timer
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_sw_timer_stop(TIMER_ID timer_id);

/**
 * @brief Identify the software timer is running
 *
 * @param[in] timer_id: timer id
 *
 * @note This API is used to identify wheather the software timer is running
 *
 * @return TRUE or FALSE
 */
BOOL_T tal_sw_timer_is_running(TIMER_ID timer_id);

/**
 * @brief Identify the software timer is running
 *
 * @param[in] timer_id: timer id
 * @param[in] remain_time: ms
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_sw_timer_remain_time_get(TIMER_ID timer_id, UINT_T *remain_time);

/**
 * @brief Start the software timer
 *
 * @param[in] timer_id: timer id
 * @param[in] time_ms: timer running cycle
 * @param[in] timer_type: timer type
 *
 * @note This API is used for starting the software timer
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_sw_timer_start(TIMER_ID timer_id, TIME_MS time_ms, TIMER_TYPE timer_type);


/**
 * @brief Trigger the software timer
 *
 * @param[in] timer_id: timer id
 *
 * @note This API is used for triggering the software timer instantly.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_sw_timer_trigger(TIMER_ID timer_id);


/**
 * @brief Release all resource of the software timer
 *
 * @param VOID
 *
 * @note This API is used for releasing all resource of the software timer
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_sw_timer_release(VOID_T);


/**
 * @brief Get timer node currently
 *
 * @param VOID
 *
 * @note This API is used for getting the timer node currently.
 *
 * @return the timer node count.
 */
INT_T tal_sw_timer_get_num(VOID_T);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif








