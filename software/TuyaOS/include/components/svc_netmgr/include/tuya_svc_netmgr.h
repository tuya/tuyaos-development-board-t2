/**
 * @file tuya_svc_netmgr.h
 * @brief TUYA network manager service
 * @version 0.1
 * @date 2021-03-10
 *
 * @copyright Copyright 2021 Tuya Inc. All Rights Reserved.
 */

#ifndef __TUYA_SVC_NETMGR_H__
#define __TUYA_SVC_NETMGR_H__

#include "tuya_os_adapter.h"
#include "base_event.h"
#include "tuya_svc_netmgr_linkage.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Definition of linkage event
 */
#define EVENT_LINK_UP       "link.up"       // linkage is up
#define EVENT_LINK_DOWN     "link.down"     // linkage is down
#define EVENT_LINK_ACTIVATE "link.activate" // linkage got activate info
#define EVENT_LINK_SWITCH   "link.switch"   // active linkage is changed

/**
 * @brief Definition of network status
 */
typedef BYTE_T NETWORK_STATUS_E;
#define NETWORK_STATUS_OFFLINE  0  // all linkages are down
#define NETWORK_STATUS_LOCAL    1  // any linkage is up
#define NETWORK_STATUS_MQTT     2  // MQTT is connected

/**
 * @brief Init network manager
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_netmgr_init(LINKAGE_CAP_E cap);

/**
 * @brief Reset all linkage config
 *
 * @param[in] reset_type see GW_RESET_TYPE_E
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_netmgr_reset(IN GW_RESET_TYPE_E reset_type);

/**
 * @brief Get current activte linkage cfg
 *
 * @param[in] cfg see LINKAGE_CFG_E for details
 * @param[in] data see LINKAGE_CFG_E for details
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_netmgr_cfg_get(IN LINKAGE_CFG_E cfg, IN VOID* data);

/**
 * @brief Set current activte linkage cfg
 *
 * @param[in] cfg see LINKAGE_CFG_E for details
 * @param[in] data see LINKAGE_CFG_E for details
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_netmgr_cfg_set(IN LINKAGE_CFG_E cfg, IN VOID* data);

/**
 * @brief Get network status
 *
 * @return see NETWORK_STATUS_E for details
 */
NETWORK_STATUS_E tuya_svc_netmgr_get_status(VOID);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __TUYA_SVC_NETMGR_H__

