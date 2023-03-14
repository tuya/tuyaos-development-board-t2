/**
* @file tal_watchdog.h
* @brief Common process - adapter the watchdog api
* @version 0.1
* @date 2021-08-25
*
* @copyright Copyright 2021-2025 Tuya Inc. All Rights Reserved.
*
*/
#ifndef __TAL_WATCHDOG_H__
#define __TAL_WATCHDOG_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief watchdog start
 * 
 * @param[in] cfg: watchdog config
 *
 * @return >0 on success. Others on error
 */
UINT32_T tal_watchdog_start(TUYA_WDOG_BASE_CFG_T *cfg);


/**
 * @brief watchdog stop
 *
 * @param[in] none
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_watchdog_stop(VOID_T);


/**
 * @brief refresh watch dog
 * 
 * @param[in] none
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_watchdog_refresh(VOID_T);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

