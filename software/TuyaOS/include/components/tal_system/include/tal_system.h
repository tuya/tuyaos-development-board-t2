/**
* @file tal_system.h
* @brief Common process - adpater some api which provide by OS
* @version 0.1
* @date 2021-08-24
*
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
*
*/
#ifndef __TAL_SYSTEM_H__
#define __TAL_SYSTEM_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief system enter critical
 *
 * @param[in]   none
 * @return  irq mask
 */
UINT_T tal_system_enter_critical(VOID_T);

/**
 * @brief system exit critical
 *
 * @param[in]   irq_mask: irq mask 
 * @return  none
 */
VOID_T tal_system_exit_critical(UINT_T irq_mask);

/**
 * @brief enter critical macro
 */
#define TAL_ENTER_CRITICAL()        \
    UINT_T __irq_mask;              \
    __irq_mask = tal_system_enter_critical()

/**
 * @brief exit critical macro
 */
#define TAL_EXIT_CRITICAL()         \
    tal_system_exit_critical(__irq_mask)



/**
* @brief System sleep
*
* @param[in] time_ms: time in MS
*
* @note This API is used for system sleep.
*
* @return VOID
*/
VOID_T tal_system_sleep(UINT_T time_ms);


/**
* @brief system reset
*
* @param none
*
* @return none
*/
VOID_T tal_system_reset(VOID_T);

/**
* @brief Get system free heap size
*
* @param none
*
* @return heap size
*/
INT_T tal_system_get_free_heap_size(VOID_T);

/**
* @brief Get system tick count
*
* @param none
*
* @return system tick count
*/
SYS_TICK_T tal_system_get_tick_count(VOID_T);

/**
* @brief Get system millisecond
*
* @param none
*
* @return system millisecond
*/
SYS_TIME_T tal_system_get_millisecond(VOID_T);

/**
* @brief Get system random data
*
* @param[in] range: random from 0  to range
*
* @return random value
*/
INT_T tal_system_get_random(UINT_T range);

/**
* @brief Get system reset reason
*
* @param[in] describe: point to reset reason describe
*
* @return reset reason
*/
TUYA_RESET_REASON_E tal_system_get_reset_reason(CHAR_T** describe);


/**
* @brief system delay
*
* @param[in] time_ms: time in MS
*
* @note This API is used for system delay.
*
* @return VOID
*/
VOID_T tal_system_delay(UINT_T time_ms);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif



