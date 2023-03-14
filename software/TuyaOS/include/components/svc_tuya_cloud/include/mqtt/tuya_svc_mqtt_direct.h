/**
 * @file tuya_svc_mqtt_direct.h
 * @brief TUYA mqtt direct service
 * @version 0.1
 * @date 2021-04-06
 *
 * @copyright Copyright 2021 Tuya Inc. All Rights Reserved.
 */

#ifndef __TUYA_SVC_MQTT_DIRECT_H__
#define __TUYA_SVC_MQTT_DIRECT_H__

#include "tuya_os_adapter.h"
#include "base_event.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EVENT_MQTT_DIRECT_CONNECTED    "mqtt.dir.con"  // mqtt connect
#define EVENT_MQTT_DIRECT_DISCONNECTED "mqtt.dir.disc" // mqtt disconnect

/**
 * @brief callback when received token
 *
 * @param[in] token send from cloud
 */
typedef VOID (*REV_BIND_TOKEN_CB)(IN CONST CHAR_T *token);

/**
 * @brief Register token callback
 *
 * @param[in] cb callback when token is got from tuya cloud
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_mqtt_direct_register_cb(REV_BIND_TOKEN_CB cb);

/**
 * @brief Start mqtt direct service
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_mqtt_direct_start(VOID);

/**
 * @brief Close mqtt direct service
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_mqtt_direct_stop(VOID);

/**
 * @brief Whether mqtt direct is connected or not
 *
 * @return TRUE on connected, False on disconnected
 */
BOOL_T tuya_svc_mqtt_direct_is_connected(VOID);

/**
 * @brief Set environment for direct connect
 *
 * @param[in] env the environment(default: pro)
 */
VOID tuya_svc_mqtt_direct_set_env(CHAR_T *env);

#ifdef __cplusplus
}
#endif

#endif //__TUYA_SVC_MQTT_DIRECT_H__