#ifndef __TUYA_WIFI_RESET_H__
#define __TUYA_WIFI_RESET_H__

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief save wifi start mode flag.
 *
 * @param[in] wifi_start_mode
 *
 * @note only called for custom mode
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

VOID_T wifi_save_start_mode(GW_WF_START_MODE wf_start_mode);

/**
 * @brief update nc_tp when wifi_start_mode changes.
 *
 * @note only called for custom mode
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
VOID_T wifi_update_nc_tp(VOID_T);


/**
 * @brief factory reset nc_tp according to wifi_start_mode and GW_WF_CFG_MTHD_SEL.
 *
 * @note called when wifi params is messed up
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
VOID  wifi_factory_reset_nc_type(P_TUYA_WIFI_CFG_PARAM p_config_params, INOUT GW_WORK_STAT_MAG_S *gw_wsm);

/**
 * @brief set a flag to indicate that iot_wf_gw_unactive_custom_mode is called
 *
 * @param[in] bcustom iot_wf_gw_unactive_custom_mode is called
 *
 * @note if bcustom is set, nc_tp will not updated by wifi start mode when device powered on
 *
 * @return void
 */

VOID_T tuya_wifi_link_set_custom_mode(BOOL_T bcustom);


/**
 * @brief Set wifi netcfg timeout value in seconds
 *
 * @param[in] timeout_s time out value of netcfg.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

VOID set_wf_netcfg_timeout(UINT_T timeout_s);

/**
 * @brief start wifi link reset short and long timer.
 *
 * @note according to mthd, timer will start accordingly.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

OPERATE_RET tuya_wifi_reset_start_timer(VOID_T);

/**
 * @brief stop wifi link reset short and long timer.
 *
 * @note according to mthd, timer will stop accordingly.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

OPERATE_RET tuya_wifi_reset_stop_timer(VOID_T);

/**
 * @brief do wifi link reset.
 *
 * @param[in] mthd
 *
 * @param[in] wifi_start_mode
 *
 * @param[in] force_clean clean wifi status
 *
 * @param[out] p_nc_tp nc_tp after reset
 *
 * @param[out] p_md md after reset
 *
 * @note according to mthd and wifi start mode, nc_tp and md will be updated and saved
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

OPERATE_RET tuya_wifi_reset_config(IN CONST GW_WF_START_MODE wifi_start_mode, IN CONST BOOL_T force_clean, OUT GW_WF_NWC_STAT_T *p_nc_tp, GW_WF_MD_T* p_md);

/**
 * @brief init wifi link reset.
 *
 * @param[in] mthd
 *
 * @param[in] md
 *
 * @note according to mthd and md, wifi reset module will be inited
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

OPERATE_RET tuya_wifi_reset_init(CONST GW_WF_CFG_MTHD_SEL mthd, GW_WF_MD_T md);

/**
 * @brief validate wifi params,such as nc_tp.
 *
 * @param[in&out] p_gw_wsm wifi related params
 *
 *
 * @note
 *
 * @return NONE
 */

VOID_T tuya_wifi_params_validate(GW_WORK_STAT_MAG_S* p_gw_wsm);

/**
 * @brief reset wifi link when devos and wifi link is NOT inited.
 *
 * @param[in] wifi_start_mode
 *
 * @param[in] force_clean
 *
 * @note only called for fast mode when start up
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

OPERATE_RET tuya_wifi_reset_fast(IN CONST GW_WF_START_MODE wifi_start_mode, CONST GW_WF_CFG_MTHD_SEL mthd, IN CONST BOOL_T force_clean);


#ifdef __cplusplus
}
#endif

#endif
