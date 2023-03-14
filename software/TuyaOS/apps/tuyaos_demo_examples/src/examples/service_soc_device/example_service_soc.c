/**
 * @file example_service_soc.c
 * @author www.tuya.com
 * @brief example_service_soc module is used to 
 * @version 0.1
 * @date 2022-09-20
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include "tuya_cloud_types.h"
#include "tuya_iot_wifi_api.h"
#include "gw_intf.h"

#include "tal_system.h"
#include "tal_log.h"

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
extern CHAR_T product_id[];

/***********************************************************
***********************function define**********************
***********************************************************/

/**
 * @brief SOC device cloud state change callback
 *
 * @param[in] status: current device status
 *
 * @return none
 */
STATIC VOID_T __soc_dev_status_changed_cb(IN CONST GW_STATUS_E status)
{
    TAL_PR_DEBUG("soc device status: %d", status);
    return;
}

/**
 * @brief SOC device upgrade entry
 *
 * @param[in] fw: firmware info
 *
 * @return none
 */
STATIC INT_T __soc_dev_rev_upgrade_info_cb(IN CONST FW_UG_S *fw)
{
    TAL_PR_DEBUG("SOC Rev Upgrade Info");
    TAL_PR_DEBUG("fw->tp:%d", fw->tp);
    TAL_PR_DEBUG("fw->fw_url:%s", fw->fw_url);
    TAL_PR_DEBUG("fw->fw_hmac:%s", fw->fw_hmac);
    TAL_PR_DEBUG("fw->sw_ver:%s", fw->sw_ver);
    TAL_PR_DEBUG("fw->file_size:%u", fw->file_size);

    // UserTODO
    return OPRT_OK;
}

/**
* @brief SOC device process restart request entry
*
* @param[in] type: gateway reset type
* @return none
*/
STATIC VOID_T __soc_dev_restart_req_cb(GW_RESET_TYPE_E type)
{
    if (GW_RESET_DATA_FACTORY != type) {
        tal_system_reset();
    }

    return;
}

/**
* @brief  SOC device format command data delivery entry
*
* @param[in] dp: obj dp info
* @return none
*/
extern VOID dp_obj_process(TY_OBJ_DP_S *dp_data);
STATIC VOID __soc_dev_obj_dp_cmd_cb(IN CONST TY_RECV_OBJ_DP_S *dp)
{
    TAL_PR_DEBUG("SOC Rev DP Obj Cmd t1:%d t2:%d CNT:%u", dp->cmd_tp, dp->dtt_tp, dp->dps_cnt);
    if(dp->cid != NULL) TAL_PR_ERR("soc not have cid.%s", dp->cid);

    UINT_T index = 0;
    for(index = 0; index < dp->dps_cnt; index++) {
        TY_OBJ_DP_S *p_dp_obj = (TY_OBJ_DP_S *)(dp->dps + index);
        dp_obj_process(p_dp_obj);
    }

    return;
}


/**
 * @brief called after the cloud sends data of type raw
 *
 * @param[in] dp: raw dp info
 *
 * @return none
 */
extern VOID dp_raw_process(TY_RECV_RAW_DP_S *raw_dp);
STATIC VOID __soc_dev_raw_dp_cmd_cb(IN CONST TY_RECV_RAW_DP_S *dp)
{
    TAL_PR_DEBUG("SOC Rev DP Raw Cmd t1:%d t2:%d dpid:%d len:%u", dp->cmd_tp, dp->dtt_tp, dp->dpid, dp->len);
    if(dp->cid != NULL) TAL_PR_ERR("soc not have cid.%s", dp->cid);

    dp_raw_process((TY_RECV_RAW_DP_S *)dp);

    return;
}

/**
* @brief query device dp status
*
* @param[in] dp_qry: query info
*
* @return none
*/
extern VOID example_update_device_status(INT_T argc, CHAR_T *argv[]);
STATIC VOID __soc_dev_dp_query_cb(IN CONST TY_DP_QUERY_S *dp_qry)
{
    TAL_PR_DEBUG("SOC Rev DP Query Cmd");
    if(dp_qry->cid != NULL) TAL_PR_ERR("soc not have cid.%s", dp_qry->cid);

    if(dp_qry->cnt == 0) {
        TAL_PR_DEBUG("soc rev all dp query");
        // UserTODO
        example_update_device_status(0, NULL);
    }else {
        TAL_PR_DEBUG("soc rev dp query cnt:%d", dp_qry->cnt);
        UINT_T index = 0;
        for(index = 0; index < dp_qry->cnt; index++) {
            TAL_PR_DEBUG("rev dp query:%d", dp_qry->dpid[index]);
            // UserTODO
        }
    }
}


/**
* @brief  SOC external network status change callback
*
* @param[in] stat: curr network status
* 
* @return none
*/
STATIC VOID __soc_dev_net_status_cb(IN CONST GW_WIFI_NW_STAT_E stat)
{
    TAL_PR_DEBUG("network status:%d", stat);
    if (STAT_CLOUD_CONN == stat) {
        example_update_device_status(0, NULL);
    }
}

/**
 * @brief soc device init
 *
 * @param[in] none:
 *
 * @return none
 */
VOID example_soc_device_init(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;

    // Initialize TuyaOS product information
    TY_IOT_CBS_S iot_cbs = {0};
    iot_cbs.gw_status_cb = __soc_dev_status_changed_cb;
    iot_cbs.gw_ug_cb = __soc_dev_rev_upgrade_info_cb;
    iot_cbs.gw_reset_cb = __soc_dev_restart_req_cb;
    iot_cbs.dev_obj_dp_cb = __soc_dev_obj_dp_cmd_cb;
    iot_cbs.dev_raw_dp_cb = __soc_dev_raw_dp_cmd_cb;
    iot_cbs.dev_dp_query_cb = __soc_dev_dp_query_cb;
    TUYA_CALL_ERR_LOG(tuya_iot_wf_soc_dev_init_param(GWCM_OLD, WF_START_SMART_AP_CONCURRENT, &iot_cbs, NULL, product_id, USER_SW_VER));

    TUYA_CALL_ERR_LOG(tuya_iot_reg_get_wf_nw_stat_cb(__soc_dev_net_status_cb));

    return;
}

/**
 * @brief remove SoC device
 *
 * @param[in] none:
 *
 * @return none
 */
VOID example_soc_device_remove(INT_T argc, CHAR_T *argv[])
{
    tuya_iot_wf_gw_unactive();
}

STATIC THREAD_HANDLE free_heap_thrd = NULL;

STATIC VOID __free_heap_task(PVOID_T args)
{
    for (;;) {
        TAL_PR_NOTICE("free heap: %d", tal_system_get_free_heap_size());
        tal_system_sleep(3000);
    }
}

VOID example_output_free_heap(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;

    THREAD_CFG_T thread_cfg = {
        .thrdname = "free_heap",
        .stackDepth = 1024,
        .priority = THREAD_PRIO_5,
    };
    if (NULL == free_heap_thrd) {
        TUYA_CALL_ERR_LOG(tal_thread_create_and_start(&free_heap_thrd, NULL, NULL, __free_heap_task, NULL, &thread_cfg));
    }
    return;
}
