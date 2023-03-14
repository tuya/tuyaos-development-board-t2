/**
 * @file dp_process.c
 * @author www.tuya.com
 * @brief dp_process module is used to 
 * @version 0.1
 * @date 2022-10-28
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include "tal_log.h"
#include "tal_memory.h"
#include "tuya_cloud_wifi_defs.h"
#include "tuya_cloud_com_defs.h"
#include "tuya_wifi_status.h"
#include "tuya_iot_com_api.h"
#include "dp_process.h"
#include "app_led.h"

/***********************************************************
************************macro define************************
***********************************************************/


/***********************************************************
***********************typedef define***********************
***********************************************************/


/***********************************************************
********************function declaration********************
***********************************************************/


/***********************************************************
***********************variable define**********************
***********************************************************/


/***********************************************************
***********************function define**********************
***********************************************************/

VOID_T update_led_status_dp(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    INT_T dp_cnt = 1; /* update DP number */

    /* no connect router, return */
    GW_WIFI_NW_STAT_E wifi_state = STAT_LOW_POWER;
    get_wf_gw_nw_status(&wifi_state);
    if (wifi_state <= STAT_AP_STA_DISC || wifi_state == STAT_STA_DISC) {
        return;
    }

    TY_OBJ_DP_S *dp_arr = (TY_OBJ_DP_S *)tal_malloc(dp_cnt * SIZEOF(TY_OBJ_DP_S));
    if (NULL == dp_arr) {
        TAL_PR_ERR("malloc failed");
        return;
    }

    dp_arr[0].dpid = DPID_SWITCH; /* DP ID */
    dp_arr[0].type = PROP_BOOL;   /* DP type */
    dp_arr[0].time_stamp = 0;
    dp_arr[0].value.dp_bool = get_led_status(); /* DP data */

    /* report DP */
    op_ret = dev_report_dp_json_async(NULL, dp_arr, dp_cnt);

    /* free requested memory space */
    tal_free(dp_arr);
    dp_arr = NULL;
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("dev_report_dp_json_async relay_config data error,err_num", op_ret);
    }

    return;
}

/**
 * @brief upload all dp data
 *
 * @param[in] none
 * 
 * @return none
 */
VOID_T update_all_dp(VOID_T)
{
    update_led_status_dp();
    return;
}

/**
 * @brief handle dp commands from the cloud
 *
 * @param[in] root: pointer header for dp data
 * 
 * @return none
 */
VOID_T deal_dp_proc(IN CONST TY_OBJ_DP_S *root)
{
    UCHAR_T dpid;
    dpid = root->dpid;
    TAL_PR_DEBUG("dpid:%d", dpid);

    switch (dpid)
    {
    case DPID_SWITCH:
        if (root->value.dp_bool == TRUE) {
            set_led_status(LED_ON);
            TAL_PR_NOTICE("led on");
        }
        else {
            set_led_status(LED_OFF);
            TAL_PR_NOTICE("led off");
        }

        /* update device current status to cloud */
        update_all_dp();
        break;

    default:
        break;
    }
    return;
}
