/**
* @file tuya_hal_output.h
* @brief Common process - Initialization
* @version 0.1
* @date 2020-11-09
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/
#ifndef _TUYA_HAL_OUTPUT_H
#define _TUYA_HAL_OUTPUT_H

#include "tkl_output.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief Output log information
*
* @param[in] str: log information
*
* @note This API is used for outputing log information
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
#define tuya_hal_output_log tkl_log_output

/**
* @brief Close log port
*
* @param VOID
*
* @note This API is used for closing log port.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
#define tuya_hal_log_close() tkl_log_close()

/**
* @brief Open log port
*
* @param VOID
*
* @note This API is used for openning log port.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
#define  tuya_hal_log_open() tkl_log_open()

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

