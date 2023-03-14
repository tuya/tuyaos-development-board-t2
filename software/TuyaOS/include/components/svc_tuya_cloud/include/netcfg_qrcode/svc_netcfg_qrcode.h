/**
 * @file tuya_svc_netcfg_qrcode.h
 * @brief QR code netcfg
 * @version 0.1
 * @date 2021-04-23
 *
 * @copyright Copyright 2021 Tuya Inc. All Rights Reserved.
 */

#ifndef __TUYA_SVC_NETCFG_QRCODE_H__
#define __TUYA_SVC_NETCFG_QRCODE_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Init QR code netcfg
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_netcfg_qrcode_init(VOID);

/**
 * @brief Set appid of QR code
 *
 * @param[in] appid app id
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_netcfg_qrcode_set_appid(CHAR_T *appid);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __TUYA_SVC_NETCFG_QRCODE_H__
