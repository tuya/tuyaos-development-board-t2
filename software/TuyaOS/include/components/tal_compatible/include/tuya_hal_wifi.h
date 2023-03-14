/**
* @file tuya_hal_wired.h
* @brief Common process - compatible the tuya_hal_wifi.h
* @version 0.1
* @date 2020-11-09
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/
#ifndef __TUYA_HAL_WIFI_H__
#define __TUYA_HAL_WIFI_H__

#include "tuya_os_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief scan current environment and obtain all the ap
 *        infos in current environment
 * 
 * @param[out]      ap_ary      current ap info array
 * @param[out]      num         the num of ar_ary
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
#define tuya_hal_wifi_all_ap_scan(ap_ary, num)  tal_wifi_all_ap_scan(ap_ary, num)

/**
 * @brief scan current environment and obtain the specific
 *        ap info.
 * 
 * @param[in]       ssid        the specific ssid
 * @param[out]      ap          the ap info
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
#define tuya_hal_wifi_assign_ap_scan(ssid, ap)  tal_wifi_assign_ap_scan(ssid, ap)

/**
 * @brief release the memory malloced in <tuya_hal_wifi_all_ap_scan>
 *        and <tuya_hal_wifi_assign_ap_scan> if needed. tuya-sdk
 *        will call this function when the ap info is no use.
 * 
 * @param[in]       ap          the ap info
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
#define tuya_hal_wifi_release_ap(ap)            tal_wifi_release_ap(ap)

/**
 * @brief set wifi interface work channel
 * 
 * @param[in]       chan        the channel to set
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
#define tuya_hal_wifi_set_cur_channel(chan)     tal_wifi_set_cur_channel(chan) 

/**
 * @brief get wifi interface work channel
 * 
 * @param[out]      chan        the channel wifi works
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
#define tuya_hal_wifi_get_cur_channel(chan)     tal_wifi_get_cur_channel(chan)

/**
 * @brief enable / disable wifi sniffer mode.
 *        if wifi sniffer mode is enabled, wifi recv from
 *        packages from the air, and user shoud send these
 *        packages to tuya-sdk with callback <cb>.
 * 
 * @param[in]       en          enable or disable
 * @param[in]       cb          notify callback
 * @return  OPRT_OK: success  Other: fail
 */
#define tuya_hal_wifi_sniffer_set(en, cb)       tal_wifi_sniffer_set(en, cb)

/**
 * @brief get wifi ip info.when wifi works in
 *        ap+station mode, wifi has two ips.
 * 
 * @param[in]       wf          wifi function type
 * @param[out]      ip          the ip addr info
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
#define tuya_hal_wifi_get_ip(wf, ip)            tal_wifi_get_ip(wf, ip)

/**
 * @brief set wifi mac info.when wifi works in
 *        ap+station mode, wifi has two macs.
 * 
 * @param[in]       wf          wifi function type
 * @param[in]       mac         the mac info
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
#define tuya_hal_wifi_set_mac(wf, mac)          tal_wifi_set_mac(wf, mac)

/**
 * @brief get wifi mac info.when wifi works in
 *        ap+station mode, wifi has two macs.
 * 
 * @param[in]       wf          wifi function type
 * @param[out]      mac         the mac info
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
#define tuya_hal_wifi_get_mac(wf, mac)          tal_wifi_get_mac(wf, mac)

/**
 * @brief set wifi work mode
 * 
 * @param[in]       mode        wifi work mode
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
#define tuya_hal_wifi_set_work_mode(mode)       tal_wifi_set_work_mode(mode)

/**
 * @brief get wifi work mode
 * 
 * @param[out]      mode        wifi work mode
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
#define tuya_hal_wifi_get_work_mode(mode)       tal_wifi_get_work_mode(mode)

/**
 * @brief start a soft ap prevent
 * 
 * @param[in]       cfg         the soft ap config
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
// #define tuya_hal_wifi_ap_start_prev(CONST WF_AP_CFG_IF_S *cfg)

/**
 * @brief start a soft ap
 * 
 * @param[in]       cfg         the soft ap config
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
#define tuya_hal_wifi_ap_start(cfg)             tal_wifi_ap_start(cfg)

/**
 * @brief start a soft ap post
 * 
 * @param[in]       cfg         the soft ap config
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
// #define tuya_hal_wifi_ap_start_post(CONST WF_AP_CFG_IF_S *cfg)

/**
 * @brief stop a soft ap prevent
 * 
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
// #define tuya_hal_wifi_ap_stop_prev(VOID_T)

/**
 * @brief stop a soft ap
 * 
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
#define tuya_hal_wifi_ap_stop()                 tal_wifi_ap_stop()

/**
 * @brief stop a soft ap post
 * 
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
// #define tuya_hal_wifi_ap_stop_post(VOID_T)

/**
 * @brief : get ap info for fast connect
 * @param[out]      fast_ap_info
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
#define tuya_hal_wifi_get_connected_ap_info_v2(fast_ap_info)       tal_wifi_get_connected_ap_info(fast_ap_info)

/**
 * @brief : fast connect prev
 * @param[in]      fast_ap_info
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
// #define tuya_hal_fast_station_connect_v2_prev(CONST FAST_WF_CONNECTED_AP_INFO_V2_S *fast_ap_info)

/**
 * @brief : fast connect
 * @param[in]      fast_ap_info
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
#define tuya_hal_fast_station_connect_v2(fast_ap_info)    tal_fast_station_connect(fast_ap_info)

/**
 * @brief : fast connect post
 * @param[in]      fast_ap_info
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
// #define tuya_hal_fast_station_connect_v2_post(CONST FAST_WF_CONNECTED_AP_INFO_V2_S *fast_ap_info)

/**
 * @brief connect wifi with ssid and passwd prev
 * 
 * @param[in]       ssid
 * @param[in]       passwd
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
// #define tuya_hal_wifi_station_connect_prev(CONST SCHAR_T *ssid, CONST SCHAR_T *passwd)

/**
 * @brief connect wifi with ssid and passwd
 * 
 * @param[in]       ssid
 * @param[in]       passwd
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
#define tuya_hal_wifi_station_connect(ssid, passwd)     tal_wifi_station_connect(ssid, passwd)

/**
 * @brief connect wifi with ssid and passwd post
 * 
 * @param[in]       ssid
 * @param[in]       passwd
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
// #define tuya_hal_wifi_station_connect_post(CONST SCHAR_T *ssid, CONST SCHAR_T *passwd)

/**
 * @brief disconnect wifi from connect ap prev
 * 
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
// #define tuya_hal_wifi_station_disconnect_prev(VOID_T)

/**
 * @brief disconnect wifi from connect ap
 * 
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
#define tuya_hal_wifi_station_disconnect()              tal_wifi_station_disconnect()

/**
 * @brief disconnect wifi from connect ap post
 * 
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
// #define tuya_hal_wifi_station_disconnect_post(VOID_T)

/**
 * @brief get wifi connect rssi
 * 
 * @param[out]      rssi        the return rssi
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
#define tuya_hal_wifi_station_get_conn_ap_rssi(rssi)    tal_wifi_station_get_conn_ap_rssi(rssi)

/**
 * @brief get wifi bssid
 * 
 * @param[out]      mac         uplink mac
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
#define tuya_hal_wifi_get_bssid(mac)                    tal_wifi_get_bssid(mac)

/**
 * @brief get wifi station work status
 * 
 * @param[out]      stat        the wifi station work status
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
#define tuya_hal_wifi_station_get_status(stat)          tal_wifi_station_get_status(stat)

/**
 * @brief set wifi country code
 * 
 * @param[in]       ccode  country code
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */ 
#define tuya_hal_wifi_set_country_code(ccode)           tal_wifi_set_country_code(ccode)

/**
 * @brief send wifi management
 * 
 * @param[in]       buf         pointer to buffer
 * @param[in]       len         length of buffer
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
#define tuya_hal_wifi_send_mgnt(buf, len)               tal_wifi_send_mgnt(buf, len)

/**
 * @brief register receive wifi management callback
 * 
 * @param[in]       enable
 * @param[in]       recv_cb     receive callback
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
#define tuya_hal_wifi_register_recv_mgnt_callback(enable, recv_cb)      tal_wifi_register_recv_mgnt_callback(enable, recv_cb)

/**
 * @brief Wifi interface initialization
 *
 * @param VOID
 *
 * @note This API is used for initialization of wifi interface.
 *
 * @return VOID
 */
#define tuya_os_wifi_intf_init()


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif




