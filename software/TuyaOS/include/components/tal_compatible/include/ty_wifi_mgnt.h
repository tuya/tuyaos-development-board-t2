/**
* @file ty_wifi_mgnt.h
* @brief wifi interface 
* @version 0.1
* @date 2021-01-28
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/

#ifndef _WIFI_MGNT_H
#define _WIFI_MGNT_H

#include "tuya_cloud_types.h"
#include "tal_wifi.h"
#include "tal_wifi_mgnt.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief send wifi management packets probe request.
 *
 * @param[in] in_buf data to be sent in tuya vsie 
 * @param[in] in_len data length to be sent
 * @param[in] ssid the ssid of the probe request
 * @param[in] source mac address of probe request
 * @param[in] destination mac address of probe request
 *
 * @note send probe request packets.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define ty_wf_send_probe_request_mgnt           tal_wifi_send_probe_request_mgnt

/**
 * @brief send wifi management packets probe response.
 *
 * @param[in] in_buf data to be sent in tuya vsie 
 * @param[in] in_len data length to be sent
 * @param[in] ssid the ssid of the probe request
 * @param[in] source mac address of probe request
 * @param[in] destination mac address of probe request
 *
 * @note send probe request packets.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define ty_wf_send_probe_response_mgnt          tal_wifi_send_probe_response_mgnt

/**
 * @brief enable wifi management packets reception in wifi station mode.
 *
 * @param[in] enbale disable or enable wifi management packets reception 
 * @param[in] recv_cb callback function to receive management packets
 *
 * @note called in wifi station mode, register callback function to process wifi management packets.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define ty_wf_register_recv_mgnt_callback       tal_wifi_register_recv_mgnt_callback

/**
 * @brief send wifi beacon packets.
 *
 * @param[in] ssid the beacon ssid 
 * @param[in] chan the beacon's wifi channel
 * @param[in] src_mac the beacon's bssid
 * @param[in] vsie_data tuya vsie data to be inserted into beacon
 * @param[in] vsie_data_len length of vsie data
 *
 * @note called to send beacon frame.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define ty_wf_send_beacon                       tal_wifi_send_beacon

/**
 * @brief register wifi recv mng frame cb
 *
 * @param[in] mng_tp mng type
 * @param[in] cb callback
 *
 * @note called to register mng recv.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define tuya_wf_recv_mng_frame_reg              tal_wifi_recv_mng_frame_reg

/**
 * @brief unregister wifi recv mng frame cb
 *
 * @param[in] cb callback
 *
 * @note called to unregister mng recv.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define tuya_wf_recv_mng_frame_unreg            tal_wifi_recv_mng_frame_unreg

/**
 * @brief recv mng frame dump reg
 *
 * @param[in] cb cb
 *
 * @return
 */
#define tuya_recv_mng_dump_reg                  tal_wifi_recv_mng_dump_reg

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif



