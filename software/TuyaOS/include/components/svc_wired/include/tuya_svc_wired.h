/**
 * @file tuya_svc_wired.h
 * @brief TUYA wired service
 * @version 0.1
 * @date 2021-03-18
 *
 * @copyright Copyright 2021 Tuya Inc. All Rights Reserved.
 */

#ifndef __TUYA_SVC_WIRED_H__
#define __TUYA_SVC_WIRED_H__

#include "tuya_hal_wired.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef BYTE_T GW_BASE_NW_STAT_T;
#define GB_STAT_LAN_UNCONN 0 // offline in LAN
#define GB_STAT_LAN_CONN 1   // online in LAN, offline in WAN
#define GB_STAT_CLOUD_CONN 2 // online in WAN

/**
 * @brief Definition of product info(wired)
 */
typedef struct {
    CHAR_T *uuid;        // strlen(uuid) <= 16,must not be null
    CHAR_T *auth_key;    // strlen(auth_key) <= 32,must not be null
} GW_PROD_INFO_S;

/**
 * @brief Handler when network state error happens(wired)
 *
 * @param[in] stat State code, see GW_BASE_NW_STAT_T
 */
typedef VOID (*GET_NW_STAT_CB)(IN CONST GW_BASE_NW_STAT_T stat);

/**
 * @brief Init wired network, and register a linkage to network manager
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_wired_init(VOID);

/**
 * @brief Register wired network state callback
 *
 * @param[in] cb callback when network state changed
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_wired_register_netstat_cb(GET_NW_STAT_CB cb);

#ifdef __cplusplus
}
#endif

#endif //__TUYA_SVC_WIRED_H__
