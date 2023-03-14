/**
* @file tkl_init_media.h
* @brief Common process - tkl init media description
* @version 0.1
* @date 2021-08-06
*
* @copyright Copyright 2021-2030 Tuya Inc. All Rights Reserved.
*
*/
#ifndef __TKL_INIT_MEDIA_H__
#define __TKL_INIT_MEDIA_H__

#include "tkl_media.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief register media description to tuya object manage
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_media_desc_get(VOID_T);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __TKL_INIT_MEDIA_H__

