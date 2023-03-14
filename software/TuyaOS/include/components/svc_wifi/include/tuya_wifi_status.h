#ifndef __TUYA_WIFI_STATUS_H__
#define __TUYA_WIFI_STATUS_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "tuya_cloud_wifi_defs.h"

typedef struct tuya_wifi_status_config {
    /** handler for network state change */
    GET_WF_NW_STAT_CB get_wf_nw_stat_cb;
    /** handler when network configuration error happens */
    WF_NW_CFG_ERR_CODE_CB wf_nw_err_code_cb;
} TUYA_WIFI_STATUS_CONFIG, *PTR_TUYA_WIFI_STATUS_CONFIG;


/**
 * @brief get wifi link network status.
 *
 * @note called when wifi network status is needed,refer to GW_WIFI_NW_STAT_E.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET wifi_get_network_status(OUT GW_WIFI_NW_STAT_E *nw_stat);
/**
 * @brief update wifi network status.
 *
 * @note called when wifi work mode or hal wifi status changes.
 *
 * @return none
 */
VOID_T wifi_update_network_status(VOID_T);

/**
 * @brief set wifi connect status.
 *
 * @note called when wifi is connecting.
 *
 * @return none
 */

void tuya_wifi_set_connect_status(NW_CFG_ERR_CODE_E e_status);

/**
 * @brief Get device network state
 *
 * @param[out] nw_stat Network state, see GW_WIFI_NW_STAT_E
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET get_wf_gw_nw_status(OUT GW_WIFI_NW_STAT_E *nw_stat);

#ifdef __cplusplus
}
#endif

#endif
