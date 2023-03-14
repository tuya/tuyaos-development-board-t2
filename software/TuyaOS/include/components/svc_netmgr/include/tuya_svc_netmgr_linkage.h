/**
 * @file tuya_svc_netmgr_linkage.h
 * @brief TUYA network linkage abstract
 * @version 0.1
 * @date 2021-03-10
 *
 * @copyright Copyright 2021 Tuya Inc. All Rights Reserved.
 */

#ifndef __TUYA_SVC_NETMGR_LINKAGE_H__
#define __TUYA_SVC_NETMGR_LINKAGE_H__

#include "tuya_os_adapter.h"
#include "tuya_cloud_com_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_ACTIVATE_RETRY_CNT  (10)

typedef UINT_T LINKAGE_CAP_E;
#define LINKAGE_CAP_LINK_EVENT      BIT(0)
#define LINKAGE_CAP_ACTIVATE_TOKEN  BIT(1)
#define LINKAGE_CAP_ACTIVATE_META   BIT(2)
#define LINKAGE_CAP_ACTIVATE    (LINKAGE_CAP_ACTIVATE_TOKEN | LINKAGE_CAP_ACTIVATE_META)

typedef enum {
    LINKAGE_TYPE_DEFAULT,   // keep it first(current active linkage)
    LINKAGE_TYPE_WIRED,     // Wired
    LINKAGE_TYPE_WIFI,      // Wi-Fi
    LINKAGE_TYPE_BT,        // BLE
    LINKAGE_TYPE_CAT1,      // CN/4G
    LINKAGE_TYPE_NB,        // NB-IoT
    LINKAGE_TYPE_EXT1,      // reserved
    LINKAGE_TYPE_EXT2,      // reserved
    LINKAGE_TYPE_EXT3,      // reserved
    LINKAGE_TYPE_VIRTUAL,   // keep it last

    LINKAGE_TYPE_MAX
} LINKAGE_TYPE_E;

typedef enum {
    LINKAGE_CFG_LOWPOWER,   // BOOL_T
    LINKAGE_CFG_IP,         // NW_IP_S
    LINKAGE_CFG_MAC,        // NW_MAC_S
    LINKAGE_CFG_RSSI,       // SCHAR_T
    LINKAGE_CFG_STATUS,     // BYTE_T (linkage customized status)

    LINKAGE_CFG_MAX
} LINKAGE_CFG_E;

typedef struct {
    LINKAGE_TYPE_E type;
    LINKAGE_CAP_E capability;
    OPERATE_RET(*open)(LINKAGE_CAP_E cap);
    OPERATE_RET(*close)(VOID);
    OPERATE_RET(*reset)(IN GW_RESET_TYPE_E reset_type);
    OPERATE_RET(*set)(IN LINKAGE_CFG_E cfg, IN VOID* data);
    OPERATE_RET(*get)(IN LINKAGE_CFG_E cfg, OUT VOID* data);
} netmgr_linkage_t;

/**
 * @brief Definition of linkage activate stage
 */
typedef BYTE_T ACTIVATE_STAGE_E;
#define ACTIVATE_STAGE_START        0  // new activate
#define ACTIVATE_STAGE_SUCCESS      1  // activate success
#define ACTIVATE_STAGE_FAIL_URL     2  // activate fail(get url from iot dns)
#define ACTIVATE_STAGE_FAIL_ACTIVE  3  // activate fail(active error)
#define ACTIVATE_STAGE_FAIL_RESULT  4  // activate fail(meta invalid)
#define ACTIVATE_STAGE_FAIL_UNKNOWN 5  // activate fail(unknown reason)
#define ACTIVATE_STAGE_STOP      0xFF  // stop current activate


/**
 * @brief Definition of activate type
 */
typedef BYTE_T ACTIVATE_TYPE_E;
#define ACTIVATE_TYPE_TOKEN  0   // activate via token
#define ACTIVATE_TYPE_META   1   // activate via meta data
#define ACTIVATE_TYPE_3RD    255 // 3rd party cloud

/**
 * @brief Definition of activate data
 */
typedef struct {
    /**
     * see ACTIVATE_STAGE_E
     */
    ACTIVATE_STAGE_E stage;

    //the following items are valid only when stage == ACTIVATE_STAGE_START

    /**
     * see ACTIVATE_TYPE_E
     */
    ACTIVATE_TYPE_E type;
    /**
     * private data for activate
     */
    VOID *data;
    /**
     * private data length
     */
    UINT_T len;
    /**
     * see LINKAGE_TYPE_E, rely on linkage up.
     */
    LINKAGE_TYPE_E linkage;
    /**
     * max retry count when failed, 0 indicates DEFAULT_ACTIVATE_RETRY_CNT
     */
    UINT_T retry_cnt;
} activate_info_t;

/**
 * @brief Register a linkage to network manager
 *
 * @param[in] linkage the linkage(const) to be registered
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_netmgr_linkage_register(IN CONST netmgr_linkage_t *linkage);

/**
 * @brief Send linkage status to network manager
 *
 * @param[in] linkage the linkage
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_netmgr_linkage_event(IN CONST netmgr_linkage_t *linkage, IN BOOL_T is_link_up);

/**
 * @brief Send linkage activation to network manager
 *
 * @param[in] linkage the linkage
 * @param[in] info actiavte information, see activate_info_t
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_netmgr_linkage_activate(IN CONST netmgr_linkage_t *linkage, IN activate_info_t *info);

/**
 * @brief Get a linkage with specific type
 *
 * @param[in] type see LINKAGE_TYPE_E
 *
 * @return linkage on success, NULL on error
 */
CONST netmgr_linkage_t * tuya_svc_netmgr_linkage_get(IN LINKAGE_TYPE_E type);

/**
 * @brief Set and keep the default linkage 
 *
 * @param[in] type see LINKAGE_TYPE_E
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_netmgr_linkage_set_default(IN LINKAGE_TYPE_E type);

/**
 * @brief Switch to next linkage
 *
 * @note If current linkage is not available(mqtt disconnect), switch to and try next linkage
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_netmgr_linkage_switch(VOID);

/**
 * @brief Set priority for all linkages
 *
 * @param[in] types see LINKAGE_TYPE_E
 * @param[in] count number of linkages
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_netmgr_linkage_set_priority(IN LINKAGE_TYPE_E *types, IN UINT_T count);

/**
 * @brief Is the linkage link-up or down
 *
 * @param[in] type see LINKAGE_TYPE_E
 *
 * @return TRUE on link-up, FALSE on link-down
 */
BOOL_T tuya_svc_netmgr_linkage_is_up(IN LINKAGE_TYPE_E type);

/**
 * @brief Set retry count for activation
 *
 * @param[in] count retry count
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_netmgr_linkage_set_retry_cnt(IN UINT_T count);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __TUYA_SVC_NETMGR_LINKAGE_H__

