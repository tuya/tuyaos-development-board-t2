/**
* @file wf_basic_intf.h
* @brief basic wifi interface 
* @version 0.1
* @date 2021-01-28
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/

#ifndef _WF_BASIC_INTF_H
#define _WF_BASIC_INTF_H
#ifdef __cplusplus
	extern "C" {
#endif

#include "tuya_cloud_types.h"
#include "tal_sleep.h"
#include "tuya_hal_wifi.h"

/**
 * @brief do wifi Scan in all channels.
 *
 * @param[out] ap_ary array of ap infomation 
 * @param[out] num number of array members
 *
 * @note do all channel scan, must set station mode first.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

#define wf_all_ap_scan                      tal_wifi_all_ap_scan

/**
 * @brief do wifi Scan with specificed SSID.
 *
 * @param[in] ssid the ssid to be scaned 
 * @param[out] ap ap info of the ssid
 *
 * @note must set station mode first.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

#define wf_assign_ap_scan                   tal_wifi_assign_ap_scan


/**
 * @brief free ap memory.
 *
 * @param[in] ap the ap info to be free 
 *
 * @note called after wf_all_ap_scan or wf_assign_ap_scan
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

#define wf_release_ap                       tal_wifi_release_ap

/**
 * @brief set wifi channel.
 *
 * @param[in] chan the chan to be set 
 *
 * @note called after set station mode
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */


#define wf_set_cur_channel                  tal_wifi_set_cur_channel

/**
 * @brief get wifi channel.
 *
 * @param[out] chan pointer that current wifi chan  
 *
 * @note called after set station mode
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */


#define wf_get_cur_channel                  tal_wifi_get_cur_channel

/**
 * @brief enable or disable wifi sniffer.
 *
 * @param[in] en enable or disable wifi sniffer  
 *
 * @param[in] cb the callback function that will get sniffer data
 *
 * @note called after set sniffer mode
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */


#define wf_sniffer_set                      tal_wifi_sniffer_set

/**
 * @brief get wifi ip.
 *
 * @param[in] wf station or ap  
 *
 * @param[out] ip ip address of station or ap
 *
 * @note called in station mode or ap mode
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

#define wf_get_ip                           tal_wifi_get_ip

/**
 * @brief get wifi mac address.
 *
 * @param[in] wf station or ap  
 *
 * @param[out] mac mac address of station or ap
 *
 * @note called in station mode or ap mode
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

#define wf_get_mac                          tal_wifi_get_mac

/**
 * @brief set wifi mac address.
 *
 * @param[in] wf station or ap  
 *
 * @param[in] mac mac address of station or ap
 *
 * @note called in station mode or ap mode
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */


#define wf_set_mac                          tal_wifi_set_mac

/**
 * @brief set wifi work mode.
 *
 * @param[in] mode wifi work mode,such as station, soft ap...
 *
 * @note called before other wifi operation
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

#define wf_wk_mode_set                      tal_wifi_set_work_mode

/**
 * @brief get wifi work mode.
 *
 * @param[out] mode current wifi work mode
 *
 * @note 
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

#define wf_wk_mode_get                      tal_wifi_get_work_mode

/**
 * @brief get connect ap info.
 *
 * @param[out] fast_ap_info the ap infomation of the connected ap
 *
 * @note called after wifi connection has established in station mode
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

#define wf_get_connected_ap_info            tal_wifi_get_connected_ap_info
/**
 * @brief do fast wifi connect.
 *
 * @param[in] fast_ap_info the ap infomation of the ap to be connected
 *
 * @note called after wf_get_connected_ap_info is called
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

#define wf_fast_station_connect             tal_fast_station_connect

/**
 * @brief do wifi connect in station mode.
 *
 * @param[in] ssid the ssid of the ap to be connected
 *
 * @param[in] passwd the passwd of the ap to be connected
 *
 * @note called in station mode
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

#define wf_station_connect                  tal_wifi_station_connect

/**
 * @brief do wifi disconnect in station mode.
 *
 * @note called in station mode after wf_station_connect
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */


#define wf_station_disconnect               tal_wifi_station_disconnect

/**
 * @brief get rssi of the connected ap.
 *
 * @param[out] rssi the rssi of the connected ap
 *
 * @note called after connection has established in station mode
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

#define wf_station_get_conn_ap_rssi         tal_wifi_station_get_conn_ap_rssi

/**
 * @brief get wifi station stat.
 *
 * @param[out] stat the stat in the station mode,refer to WF_STATION_STAT_E 
 *
 * @note called after wf_station_connect
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

#define wf_station_stat_get                 tal_wifi_station_get_status

/**
 * @brief start ap in ap mode.
 *
 * @param[in] cfg the configuration of the ap 
 *
 * @note called in wifi ap mode
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

#define wf_ap_start                         tal_wifi_ap_start

/**
 * @brief stop ap in ap mode.
 *
 * @note called in wifi ap mode
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

#define wf_ap_stop                          tal_wifi_ap_stop

/**
 * @brief get the ap mac address.
 *
 * @note called in wifi ap mode
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
 

#define wf_get_ap_mac                       tal_wifi_get_ap_mac

/**
 * @brief set the wifi country code.
 *
 * @note called in wifi ap mode
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

#define wf_set_country_code                 tal_wifi_set_country_code

/**
 * @brief get the wifi country code.
 *
 * @note called after wf_set_country_code
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

OPERATE_RET wf_get_country_code(IN CONST CHAR_T *p_country_code);

/**
 * @brief set the wifi low power dtim.
 *
 * @note called before enter wifi low power mode
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

#define tuya_wifi_set_lps_dtim              tal_wifi_set_lps_dtim

/**
 * @brief disable the wifi low power mode.
 *
 * @note called when exiting wifi low power mode
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

#define tuya_wifi_lp_disable                tal_wifi_lp_disable

/**
 * @brief enable the wifi low power mode.
 *
 * @note called when entering wifi low power mode
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

#define tuya_wifi_lp_enable                tal_wifi_lp_enable 

/**
 * @brief disable the cpu low power mode.
 *
 * @note called when exiting wifi low power mode
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

#define tuya_cpu_lp_disable                 tal_cpu_lp_disable

/**
 * @brief enable the cpu low power mode.
 *
 * @note called when entering wifi low power mode
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

#define tuya_cpu_lp_enable                  tal_cpu_lp_enable


/**
 * @brief Get CPU lowpower mode
 * 
 * @return TRUE on lowpower, FALSE on normal
 */
#define tuya_cpu_get_lp_mode				tal_cpu_get_lp_mode

/**
 * @brief do wifi calibration
 *
 * @note called when test wifi
 *
 * @return true on success. faile on failure
 */
#define wf_rf_calibrated                   tal_wifi_rf_calibrated


#ifdef __cplusplus
}
#endif
#endif

