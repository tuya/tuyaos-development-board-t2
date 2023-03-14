/**
* @file tkl_init_display.h
* @brief Common process - tkl init display description
* @version 0.1
* @date 2021-08-06
*
* @copyright Copyright 2021-2030 Tuya Inc. All Rights Reserved.
*
*/
#ifndef __TKL_INIT_DISPLAY_H__
#define __TKL_INIT_DISPLAY_H__

#include "tkl_display.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief register display description to tuya object manage
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_display_desc_get();

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __TKL_INIT_DISPLAY_H__

