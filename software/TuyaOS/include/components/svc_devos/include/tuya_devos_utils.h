/**
* @file tuya_devos_utils.h
* @brief Utilities of DevOS
* @version 0.1
* @date 2020-11-09
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*/

#ifndef TUYA_DEVOS_UTILS_H
#define TUYA_DEVOS_UTILS_H

#include "tuya_cloud_com_defs.h"
#include "ty_cJSON.h"
#include "gw_intf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Load summer timezone talbe
 *
 * @param[in] tm_zone Summer timezone table
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET gw_load_sum_time_zone_tbl(IN CONST CHAR_T *tm_zone);

/**
 * @brief Get DP flow control rules
 *
 * @return See DP_REPT_FLOW_CTRL_UNIT, NULL on error
 */
DP_REPT_FLOW_CTRL_UNIT* get_all_dp_flow_ctrl_rule(VOID);

/**
 * @brief Parse DP flow control rules
 *
 * @param[in] result Json encoded flow control rules
 *
 * @return Buffer of flow control rules, NULL on error
 */
CHAR_T* gw_dp_flow_ctrl_cfg_proc(ty_cJSON* result);

/**
 * @brief Get serial number
 *
 * @return Buffer of serial number, NULL on error
 */
CHAR_T *tuya_get_serialno(VOID);

/**
 * @brief Initialize rtc module
 *
 * @param[in] timeval Time interval to update local time
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET ty_init_rtc(INT_T timeval);

/**
 * @brief Set DevOS extension state
 *
 * @param[in] ext_stat Extension state, see GW_EXT_STAT_T
 */
VOID set_gw_ext_stat(IN CONST GW_EXT_STAT_T ext_stat);

/**
 * @brief Get DevOS extension state
 *
 * @return Extension state, see GW_EXT_STAT_T
 */
GW_EXT_STAT_T get_gw_ext_stat(VOID);

/**
 * @brief Set DevOS event handlers
 *
 * @param[in] cbs Handler of event, see TY_IOT_CBS_S
 */
VOID gw_register_cbs(IN CONST TY_IOT_CBS_S *cbs);

/**
 * @brief Set network configuration token
 *
 * @param[in] token Token got from network configuration
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET gw_user_token_bind(IN CONST CHAR_T *token);

/**
 * @brief Save network configuration token
 *
 * @param[in] token Token got from network configuration
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET gw_save_auth_token(IN CONST CHAR_T *token, IN CONST CHAR_T *region, IN CONST CHAR_T *regist_key);

/**
 * @brief Get device network linkage state
 *
 * @param[out] nw_stat Linkage state, see GW_NW_STAT_T
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
GW_NW_STAT_T get_gw_nw_status(VOID);

/**
 * @brief Set activation state
 *
 * @param[in] stat Work state, see GW_WORK_STAT_T
 */
VOID set_gw_active(GW_WORK_STAT_T stat);

/**
 * @brief Get activation state
 *
 * @return Work state, see GW_WORK_STAT_T
 */
GW_WORK_STAT_T get_gw_active(VOID);

/**
 * @brief Get gateway's device id
 *
 * @return Device id as a string, return NULL if not exist
 */
CONST CHAR_T *get_gw_dev_id(VOID);

/**
 * @brief Get gateway's mcu software version
 *
 * @param[in] tp device type
 *
 * @return Version as a string, return NULL if not exist
 */
CONST CHAR_T *get_gw_sw_ver(GW_PERMIT_DEV_TP_T tp);

/**
 * @brief Get gateway's dp bind status
 *
 * @return Dp bind status, return FALSE if not exist
 */
BOOL_T get_gw_bind_status(VOID);

/**
 * @brief Get gateway's ssid
 *
 * @return SSID as a string, return NULL if not exist
 */
CONST CHAR_T *get_gw_ssid(VOID);

/**
 * @brief Get gateway region
 *
 * @return Region as a string, return NULL if not exist
 */
CONST CHAR_T *get_gw_region(VOID);

/**
 * @brief Get gateway country code
 *
 * @return Country code as a string, return NULL if not exist
 */
CONST CHAR_T *get_gw_country_code(VOID);

/**
 * @brief Get gateway's authentication status
 *
 * @return TRUE if authorized, otherwise return FALSE
 */
BOOL_T get_gw_auth_status(VOID);

/**
 * @brief Set user defined features(udf) for activation
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET gw_set_udf(IN CONST CHAR_T *udf);

/**
 * @brief Get user defined features(udf) for activation
 *
 * @return NULL on error
 */
CHAR_T * gw_get_udf(VOID);

/**
 * @brief Set election enable or not
 *
 * @param[in] enable election is supported or not
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET gw_active_set_support_election(BOOL_T enable);

/**
 * @brief Get election enable or not
 *
 * @return TRUE on enable, False on disable
 */
BOOL_T gw_active_get_support_election(VOID);

/**
 * @brief Set extension params for activation
 *
 * @param[in] param Extension params
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET gw_active_set_ext_param(IN CHAR_T *param);

/**
 * @brief Get extension params for activation
 *
 * @return NULL on error
 */
CHAR_T * gw_active_get_ext_param(VOID);

/**
 * @brief Get device IP
 *
 * @param[out] p_ip Device IP, see NW_IP_S
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET gw_get_ip(NW_IP_S *p_ip);

/**
 * @brief Get device MAC
 *
 * @param[out] p_mac Device MAC, see NW_MAC_S
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET gw_get_mac(NW_MAC_S *p_mac);

/**
 * @brief Get device RSSI level
 *
 * @param[out] p_rssi Device RSSI level, if(*p_rssi == 99) then do't support get RSSI
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET gw_get_rssi(SCHAR_T *p_rssi);

/**
 * @brief Update versions of all attach modules.
 *
 * @param[in] attr_num Count of attach modules
 * @param[in] attrs Attributes of attach modules
 *
 * @note This API is used for attach modules updating in case of dynamic add/remove/update.
 * Info of main netlink module will also be included when upload.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_iot_dev_update_attachs(UINT_T attr_num, GW_ATTACH_ATTR_T *attrs);
OPERATE_RET tuya_iot_dev_set_attach_attr(GW_PERMIT_DEV_TP_T tp, GW_ATTACH_ATTR_T * attr);
OPERATE_RET tuya_iot_dev_get_attach_attr(GW_PERMIT_DEV_TP_T tp, GW_ATTACH_ATTR_T * attr);
// enable or disable attach update to cloud
OPERATE_RET tuya_iot_dev_enable_attach_update(BOOL_T enable);
BOOL_T tuya_iot_dev_get_attach_update_enable(VOID);

/**
 * @brief Enable reset log upload or not
 *
 * @param[in] upload Enable or not
 */
VOID gw_upload_rst_log_set(IN CONST BOOL_T upload);

/**
 * @brief Enable reset log upload or not
 *
 */
BOOL_T gw_upload_rst_log_get(VOID);

/**
 * @brief Enable oem or not
 *
 * @param[in] oem Enable or not
 */
VOID gw_oem_set(IN CONST BOOL_T oem);

/**
 * @brief Whether oem is enabled or not
 *
 * @return TRUE on positive, FALSE on negative
 */
BOOL_T is_gw_oem(VOID);

/**
 * @brief Force to upload all versions
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET gw_update_versions(VOID);



OPERATE_RET gw_cad_ver_update(CHAR_T* old_ver, CHAR_T* new_ver,
                              IN CONST DEV_TYPE_T tp, IN CONST GW_ATTACH_ATTR_T *attr, IN CONST UINT_T attr_num);

/**
 * @brief Get stride info
 *
 * @return NULL on faile, others see GW_STRIDE_UPDATE_S
 */
GW_STRIDE_UPDATE_S* gw_get_new_pid_key(VOID);

/**
 * @brief Set gw active option
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET gw_active_set_options(CHAR_T *op);

/**
 * @brief Get gw active options
 * 
 * @param[out] len active options len
 *
 * @return buffer of options array
 */
CHAR_T ** gw_active_get_options(INT_T *len);

/**
 * @brief Free all saved gw active options(internal api)
 * 
 */
VOID_T gw_active_free_options(VOID_T);
#ifdef __cplusplus
}
#endif

#endif //TUYA_DEVOS_UTILS_H

