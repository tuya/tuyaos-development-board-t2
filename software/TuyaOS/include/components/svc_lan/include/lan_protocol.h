/**
* @file lan_protocol.h
* @brief Protocol of TUYA LAN service
* @version 0.1
* @date 2019-08-20
*
* @note This file is deprecated, please use tuya_svc_lan.h
*
* @copyright Copyright 2019-2021 Tuya Inc. All Rights Reserved.
*
*/

#ifndef  __LAN_PROTOCOL__
#define __LAN_PROTOCOL__

#include "tuya_svc_lan.h" // new LAN header file

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief decrypt data
 *
 * @param[in] data encry data
 * @param[in] len encry data length
 * @param[in] key decrypt key
 * @param[out] out_data origin data
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define __lan_parse_data(data, len, key, out_data) tuya_svc_lan_decrypt_data(data, len, key, out_data)

/**
 * @brief encry lan message
 *
 * @param[in] data origin data
 * @param[in] len origin data length
 * @param[in] ec_data encode data
 * @param[in] ec_len encode data length
 * @param[in] key encry key
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define encrpt_lan_msg(data, len, ec_data, ec_len, key) tuya_svc_lan_encrypt_data(data, len, ec_data, ec_len, key)

/**
 * @brief lan protocol send data, the date will package 0x55aa
 *
 * @param[in] fr_num refer to LAN_PRO_HEAD_GW_S
 * @param[in] fr_type refer to LAN_PRO_HEAD_GW_S
 * @param[in] ret_code refer to LAN_PRO_HEAD_GW_S
 * @param[in] data refer to LAN_PRO_HEAD_GW_S
 * @param[in] len refer to LAN_PRO_HEAD_GW_S
 * @param[in] s_len send data length
 *
 * @return send data buf point
 */
#define __mlp_gw_send_da(fr_num,fr_type,ret_code,data,len,s_len) tuya_svc_lan_package_data(fr_num,fr_type,ret_code,data,len,s_len)

#ifdef __cplusplus
}
#endif
#endif
