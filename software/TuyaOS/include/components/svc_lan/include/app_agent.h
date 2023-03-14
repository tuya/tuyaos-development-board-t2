/**
 * @file app_agent.h
 * @brief APIs of TUYA LAN service
 * @version 0.1
 * @date 2015-06-18
 *
 * @note This file is deprecated, please use tuya_svc_lan.h
 *
 * @copyright Copyright 2015-2021 Tuya Inc. All Rights Reserved.
 */

#ifndef _APP_AGENT_H
#define _APP_AGENT_H

#include "tuya_svc_lan.h" // new LAN header file

#ifdef __cplusplus
extern "C" {
#endif

//group test
#define FRM_GRP_OPER_ENGR 0xd0
#define FRM_GRP_CMD 0xd1

/**
 * @brief lan protocol init
 *
 * @param[in] wechat true/false
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define lan_pro_cntl_init(wechat) tuya_svc_lan_init()

/**
 * @brief lan protocol exit
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define lan_pro_cntl_exit() tuya_svc_lan_exit()

/**
 * @brief lan protocol diable
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define lan_pro_cntl_disable() tuya_svc_lan_disable()

/**
 * @brief lan protocol enable
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define lan_pro_cntl_enable() tuya_svc_lan_enable()

/**
 * @brief lan dp report
 *
 * @param[in] data data buf
 * @param[in] len buf length
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define lan_dp_sata_report(data,len) tuya_svc_lan_dp_report(data,len)

/**
 * @brief lan dp report callback
 *
 * @param[in] fr_type refer to LAN_PRO_HEAD_APP_S
 * @param[in] ret_code refer to LAN_PRO_HEAD_APP_S
 * @param[in] data refer to LAN_PRO_HEAD_APP_S
 * @param[in] len refer to LAN_PRO_HEAD_APP_S
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define lan_data_report_cb(fr_type,ret_code,data,len) tuya_svc_lan_data_report(fr_type,ret_code,data,len)

/**
 * @brief get vaild connect count
 *
 * @return vaild count
 */
#define lan_pro_cntl_get_valid_connect_cnt() tuya_svc_lan_get_valid_connections()

/**
 * @brief disconnect all sockets
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define lan_disconnect_all_sockets() tuya_svc_lan_disconnect_all()

/**
 * @brief lan configure
 *
 * @param[in] cfg refer to Lan_Cfg_e
 * @param[in] data buf
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define lan_pro_cntl_cfg(cfg,data) tuya_svc_lan_cfg(cfg,data)

/**
 * @brief register callback
 *
 * @param[in] frame_type refer to LAN_PRO_HEAD_APP_S
 * @param[in] frame_type refer to lan_cmd_handler_cb
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define lan_pro_cntl_register_cb(frame_type,handler) tuya_svc_lan_register_cb(frame_type,handler)

/**
 * @brief unregister callback
 *
 * @param[in] frame_type refer to LAN_PRO_HEAD_APP_S
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define lan_pro_cntl_unregister_cb(frame_type) tuya_svc_lan_unregister_cb(frame_type)

/**
 * @brief judge if lan connect
 *
 * @return TRUE/FALSE
 */
#define is_lan_connected() (tuya_svc_lan_get_valid_connections() != 0)

#ifdef __cplusplus
}
#endif
#endif

