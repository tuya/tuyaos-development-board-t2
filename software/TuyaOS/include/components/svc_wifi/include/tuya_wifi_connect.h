#ifndef __TUYA_WIFI_CONNECT_H__
#define __TUYA_WIFI_CONNECT_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief wifi reconnect callback function.
 *
 * @param[in] local_disconnect_seconds time of local network disconnection
 * @param[in] cloud_disconnect_seconds time of cloud(mqtt) disconnection
 *
 * @note called after tuya_svc_wifi_init.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

typedef OPERATE_RET(* reconnect_cb_fn)(INT_T local_disconnect_seconds, INT_T cloud_disconnect_seconds);

/**
 * @brief reconnect configuration.
 */
typedef struct {
    reconnect_cb_fn              reconnect_fn;
} reconnect_config_t, *ptr_reconnect_config_t;

/**
* @brief wifi prepare before reconnect .
*
*
* @note called before reconnect func start.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_wifi_reconnect_prepare(VOID_T);

/**
 * @brief register wifi reconnect func .
 *
 * @param[in] reconnect_fn callback function when wifi connection is lost
 *
 * @note called when wifi monitor found connection is lost.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

OPERATE_RET tuya_wifi_register_reconnect_config(reconnect_cb_fn  reconnect_fn);


/**
 * @brief wifi connect to ap .
 *
 *
 * @note called after reconnect func got ssid&passwd
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

OPERATE_RET tuya_wifi_reconnect_to_ap(VOID_T);

/**
 * @brief Start wifi switch
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_wifi_switch_start(IN CHAR_T *ssid, IN CHAR_T *passwd);


#ifdef __cplusplus
}
#endif

#endif

