/**
* @file tuya_hal_wired.h
* @brief Common process - compatible the tuya_hal_wired.h
* @version 0.1
* @date 2020-11-09
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/
#ifndef __TUYA_HAL_WIRED_H__
#define __TUYA_HAL_WIRED_H__

#include "tal_wired.h"
#include "tuya_svc_wired.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
* @brief Get wired ethernet ip info
*
* @param[in] ip: the ip addr info
*
* @note This API is used for getting wired ethernet ip info.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
//OPERATE_RET tuya_hal_wired_get_ip(NW_IP_S *ip);
#define tuya_hal_wired_get_ip(ip)   tal_wired_get_ip(ip)

/**
* @brief Check whether the hardware is connected to internet
*
* @param VOID
*
* @note This API is used to check whether the hardware is connected to internet.
*
* @return true on connected, false on disconnected.
*/
//BOOL_T tuya_hal_wired_station_conn(VOID);
//废弃

/**
* @brief Get wired ethernet mac information
*
* @param[out] mac: the mac information
*
* @note This API is used for getting wired ethernet mac information
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
//OPERATE_RET tuya_hal_wired_get_mac(NW_MAC_S *mac);
#define tuya_hal_wired_get_mac(mac)   tal_wired_get_mac(mac)

/**
* @brief Set wired ethernet mac information
*
* @param[in] mac: the mac information
*
* @note This API is used for setting wired ethernet mac information
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
//OPERATE_RET tuya_hal_wired_set_mac(CONST NW_MAC_S *mac);
#define tuya_hal_wired_set_mac(mac)   tal_wired_set_mac(mac)

/**
* @brief If the hardware has a wifi interface, and user want to connect wifi, tuya-app will notify current wifi ssid and passwd,
* and tuya-sdk will nofity user with this function. user should implement wifi connect process in this function.
*
* @param[in] ssid: ssid of router
* @param[in] passwd: password of router
*
* @note This API is used for setting ssid and password of the router.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
//OPERATE_RET tuya_hal_wired_wifi_set_station_connect(CONST CHAR_T *ssid, CONST CHAR_T *passwd);
//废弃

/**
* @brief Check whether the hardware need to config
*
* @param VOID
*
* @note This API is used for checking whether the hardware need to config.
* if the hardware has a wifi interface, and user want to connect wifi, then return true.
* if the hardware does not have a wifi interface, or user do not want to connect wifi, return false.
*
* @return TRUE or FALSE
*/
//BOOL_T tuya_hal_wired_wifi_need_cfg(VOID);
//废弃

/**
* @brief Get wifi rssi
*
* @param[out] rssi: wifi rssi
*
* @note This API is used for getting wifi rssi.
* if the hardware has a wifi interface and wifi is connected, return wifi rssi, or return null.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
//OPERATE_RET tuya_hal_wired_wifi_station_get_conn_ap_rssi(SCHAR_T *rssi);
//废弃

/**
* @brief Get network status
*
* @param[out] stat: the status of network
*
* @note This API is used for getting network status.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
//OPERATE_RET tuya_hal_wired_get_nw_stat(GW_BASE_NW_STAT_T *stat);
#define tuya_hal_wired_get_nw_stat(stat)        tal_wired_get_status(stat)
//废弃

#define tuya_hal_wired_set_nw_stat_cb(cb)       tal_wired_set_status_cb(cb)

/**
* @brief Get wired connect status
*
* @param[out] stat: the status of network
*
* @note This API is used for getting wired connect status.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
//OPERATE_RET tuya_hal_wired_if_connect_internet(BOOL_T *stat);
//废弃

/**
 * @brief Wired interface initialization
 *
 * @param VOID
 *
 * @note This API is used for initialization of wired interface.
 *
 * @return VOID
 */
//VOID_T tuya_os_wired_intf_init(VOID_T);
//废弃

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif



