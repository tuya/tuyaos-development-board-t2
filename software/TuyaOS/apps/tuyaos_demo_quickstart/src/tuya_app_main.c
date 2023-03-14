/**
 * @file tuya_app_main.c
 * @author www.tuya.com
 * @brief tuya_app_main module is used to 
 * @version 0.1
 * @date 2022-10-28
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include "tuya_cloud_types.h"
#include "tuya_iot_wifi_api.h"
#include "tuya_ws_db.h"
#include "gw_intf.h"
#include "tal_log.h"
#include "tal_wifi.h"
#include "tal_uart.h"
#include "mf_test.h"
#if defined(ENABLE_LWIP) && (ENABLE_LWIP == 1)
#include "lwip_init.h"
#endif

#include "dp_process.h"
#include "app_key.h"
#include "app_led.h"

/***********************************************************
************************macro define************************
***********************************************************/

#define PID "uiiyltkjmkhwumga"

/* The registration code here does not work, you need to apply for a new one.
 * https://developer.tuya.com/cn/docs/iot/lisence-management?id=Kb4qlem97idl0
 */
// #define UUID     "f998xxxxxxxx2409"
// #define AUTHKEY  "WEHAxxxxxxxxxxxxxxxxxxxxxxxxVVkf"

/* network button, LED pin */
#define KEY_PIN 7
#define LED_PIN 17

/* Compatible with TuyaOS 3.3.x */
#ifndef TUYA_UART_NUM_0
#define TUYA_UART_NUM_0 UART_NUM_0
#endif

/***********************************************************
***********************typedef define***********************
***********************************************************/


/***********************************************************
********************function declaration********************
***********************************************************/


/***********************************************************
***********************variable define**********************
***********************************************************/
/* app thread handle */
STATIC THREAD_HANDLE ty_app_thread = NULL;

/***********************************************************
***********************function define**********************
***********************************************************/

/**
 * @brief SOC device upgrade entry
 *
 * @param[in] fw: firmware info
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h".
 */
STATIC OPERATE_RET __soc_dev_rev_upgrade_info_cb(IN CONST FW_UG_S *fw)
{
    TAL_PR_DEBUG("SOC Rev Upgrade Info");
    TAL_PR_DEBUG("fw->tp:%d", fw->tp);
    TAL_PR_DEBUG("fw->fw_url:%s", fw->fw_url);
    TAL_PR_DEBUG("fw->fw_hmac:%s", fw->fw_hmac);
    TAL_PR_DEBUG("fw->sw_ver:%s", fw->sw_ver);
    TAL_PR_DEBUG("fw->file_size:%u", fw->file_size);

    return OPRT_OK;
}

/**
 * @brief SOC device cloud state change callback
 *
 * @param[in] status: current status
 *
 * @return none
 */
STATIC VOID_T __soc_dev_status_changed_cb(IN CONST GW_STATUS_E status)
{
    TAL_PR_DEBUG("SOC TUYA-Cloud Status:%d", status);
    return;
}

/**
 * @brief SOC device DP query entry
 *
 * @param[in] dp_qry: DP query list
 *
 * @return none
 */
STATIC VOID_T __soc_dev_dp_query_cb(IN CONST TY_DP_QUERY_S *dp_qry)
{
    UINT32_T index = 0;

    TAL_PR_DEBUG("SOC Rev DP Query Cmd");
    if (dp_qry->cid != NULL) {
        TAL_PR_ERR("soc not have cid.%s", dp_qry->cid);
    }

    if (dp_qry->cnt == 0) {
        TAL_PR_DEBUG("soc rev all dp query");
        update_all_dp();
    } else {
        TAL_PR_DEBUG("soc rev dp query cnt:%d", dp_qry->cnt);
        for (index = 0; index < dp_qry->cnt; index++) {
            TAL_PR_DEBUG("rev dp query:%d", dp_qry->dpid[index]);
            // UserTODO
        }
    }

    return;
}

/**
 * @brief SOC device format command data delivery entry
 *
 * @param[in] dp: obj dp info
 *
 * @return none
 */
STATIC VOID_T __soc_dev_obj_dp_cmd_cb(IN CONST TY_RECV_OBJ_DP_S *dp)
{
    UINT32_T index = 0;

    TAL_PR_DEBUG("SOC Rev DP Obj Cmd t1:%d t2:%d CNT:%u", dp->cmd_tp, dp->dtt_tp, dp->dps_cnt);
    if (dp->cid != NULL) {
        TAL_PR_ERR("soc not have cid.%s", dp->cid);
    }

    for (index = 0; index < dp->dps_cnt; index++) {
        deal_dp_proc(&(dp->dps[index]));
    }

    return;
}

/**
 * @brief SOC device transparently transmits command data delivery entry
 *
 * @param[in] dp: raw dp info
 *
 * @return none
 */
STATIC VOID_T __soc_dev_raw_dp_cmd_cb(IN CONST TY_RECV_RAW_DP_S *dp)
{
    TAL_PR_DEBUG("SOC Rev DP Raw Cmd t1:%d t2:%d dpid:%d len:%u", dp->cmd_tp, dp->dtt_tp, dp->dpid, dp->len);
    if (dp->cid != NULL) {
        TAL_PR_ERR("soc not have cid.%s", dp->cid);
    }

    return;
}

/**
 * @brief SOC device process restart request entry
 *
 * @param[in] type: gateway reset type
 *
 * @return none
 */
STATIC VOID_T __soc_dev_restart_req_cb(GW_RESET_TYPE_E type)
{
    TAL_PR_DEBUG("SOC Rev Restart Req %d", type);
    if (GW_RESET_DATA_FACTORY != type) {
        // UserTODO Device process restart
        tal_system_reset();
    }

    return;
}

/**
 * @brief SOC external network status change callback
 *
 * @param[in] stat: curr network status
 *
 * @return none
 */
STATIC VOID __soc_dev_net_status_cb(IN CONST GW_WIFI_NW_STAT_E stat)
{
    TAL_PR_DEBUG("network status:%d", stat);
    return;
}

/**
 * @brief mf uart init
 *
 * @param[in] baud: Baud rate
 * @param[in] bufsz: uart receive buffer size
 *
 * @return none
 */
VOID mf_uart_init_callback(UINT_T baud, UINT_T bufsz)
{
    TAL_UART_CFG_T cfg;
    memset(&cfg, 0, sizeof(TAL_UART_CFG_T));
    cfg.base_cfg.baudrate = baud;
    cfg.base_cfg.databits = TUYA_UART_DATA_LEN_8BIT;
    cfg.base_cfg.parity = TUYA_UART_PARITY_TYPE_NONE;
    cfg.base_cfg.stopbits = TUYA_UART_STOP_LEN_1BIT;
    cfg.rx_buffer_size = bufsz;

    tal_uart_init(TUYA_UART_NUM_0, &cfg);

    return;
}

/**
 * @brief mf uart free
 *
 * @param[in] none
 *
 * @return none
 */
VOID mf_uart_free_callback(VOID)
{
    tal_uart_deinit(TUYA_UART_NUM_0);
    return;
}

/**
 * @brief mf uart send function
 *
 * @param[in] data: send data
 * @param[in] len: send data length
 * 
 * @return none
 */
VOID mf_uart_send_callback(IN BYTE_T *data, IN CONST UINT_T len)
{
    tal_uart_write(TUYA_UART_NUM_0, data, len);
    return;
}

/**
 * @brief mf uart receive function
 *
 * @param[in] buf: receive buffer
 * @param[in] len: receive buffer max length
 *
 * @return receive data length
 */
UINT_T mf_uart_recv_callback(OUT BYTE_T *buf, IN CONST UINT_T len)
{
    return tal_uart_read(TUYA_UART_NUM_0, buf, len);
}

/**
 * @brief Product test callback function
 *
 * @param[in] cmd: Command
 * @param[in] data: data
 * @param[out] ret_data: Resulting data
 * @param[out] ret_len: Resulting data length
 * 
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h".
 */
OPERATE_RET mf_user_product_test_callback(USHORT_T cmd, UCHAR_T *data, UINT_T len, OUT UCHAR_T **ret_data, OUT USHORT_T *ret_len)
{
    /* USER todo */
    return OPRT_OK;
}

/**
 * @brief mf configure write callback functions
 *
 * @param[in] none
 *
 * @return none
 */
VOID mf_user_callback(VOID)
{
    return ;
}

/**
 * @brief Callback function before entering the production test
 *
 * @param[in] none
 *
 * @return none
 */
VOID mf_user_enter_mf_callback(VOID)
{
    return ;
}

BOOL_T mf_gpio_test_cb(IN CONST CHAR_T *in, OUT CHAR_T *out)
{
    return TRUE;
}


BOOL_T mf_pre_gpio_test_cb(VOID)
{
    return TRUE;
}
/**
 * @brief SOC device initialization
 *
 * @param[in] none
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h".
 */
OPERATE_RET __soc_device_init(VOID_T)
{
    OPERATE_RET rt = OPRT_OK;

#if (defined(UUID) && defined(AUTHKEY))
    ws_db_init_mf();

    /* Set authorization information
     * Note that if you use the default authorization information of the code, there may be problems of multiple users and conflicts, 
     * so try to use all the authorizations purchased from the tuya iot platform.
     * Buying guide: https://developer.tuya.com/cn/docs/iot/lisence-management?id=Kb4qlem97idl0.
     * You can also apply for two authorization codes for free in the five-step hardware development stage of the Tuya IoT platform.
     * Authorization information can also be written through the production testing tool.
     * When the production testing function is started and the authorization is burned with the Tuya Cloud module tool, 
     * please comment out this piece of code.
     */
    WF_GW_PROD_INFO_S prod_info = {UUID, AUTHKEY};
    TUYA_CALL_ERR_RETURN(tuya_iot_set_wf_gw_prod_info(&prod_info));
#else
    // 产测初始化， 注册函数需要应用实现，其中串口驱动不需要应用提供
    MF_IMPORT_INTF_S intf = {0};

    intf.uart_init = mf_uart_init_callback;
    intf.uart_free = mf_uart_free_callback;
    intf.uart_send = mf_uart_send_callback;
    intf.uart_recv = mf_uart_recv_callback;

    intf.gpio_test = mf_gpio_test_cb;
    intf.mf_user_product_test = mf_user_product_test_callback;
    intf.user_callback = mf_user_callback;
    intf.user_enter_mf_callback = mf_user_enter_mf_callback;
    intf.user_pre_gpio_test = mf_pre_gpio_test_cb;

    TAL_PR_ERR("mf_init APP_BIN_NAME[%s] USER_SW_VER[%s]", APP_BIN_NAME, USER_SW_VER);
    
    TUYA_CALL_ERR_RETURN(mf_init(&intf, APP_BIN_NAME, USER_SW_VER, TRUE));
#endif

    /* Initialize TuyaOS product information */
    TY_IOT_CBS_S iot_cbs = {0};
    iot_cbs.gw_status_cb = __soc_dev_status_changed_cb;
    iot_cbs.gw_ug_cb = __soc_dev_rev_upgrade_info_cb;
    iot_cbs.gw_reset_cb = __soc_dev_restart_req_cb;
    iot_cbs.dev_obj_dp_cb = __soc_dev_obj_dp_cmd_cb;
    iot_cbs.dev_raw_dp_cb = __soc_dev_raw_dp_cmd_cb;
    iot_cbs.dev_dp_query_cb = __soc_dev_dp_query_cb;

    TUYA_CALL_ERR_RETURN(tuya_iot_wf_soc_dev_init(GWCM_OLD, WF_START_AP_FIRST, &iot_cbs, PID, USER_SW_VER));

    TUYA_CALL_ERR_RETURN(tuya_iot_reg_get_wf_nw_stat_cb(__soc_dev_net_status_cb));

    /* Network button, LED initialization */
    app_led_init(LED_PIN);
    app_key_init(KEY_PIN);

    return 0;
}

STATIC VOID_T user_main(VOID_T)
{
    OPERATE_RET rt = OPRT_OK;

    /* Initialization, because DB initialization takes a long time, 
     * which affects the startup efficiency of some devices, 
     * so special processing is performed during initialization to delay initialization of DB
     */
    TY_INIT_PARAMS_S init_param = {0};
    init_param.init_db = TRUE;
    strcpy(init_param.sys_env, TARGET_PLATFORM);
    TUYA_CALL_ERR_LOG(tuya_iot_init_params(NULL, &init_param));

    tal_log_set_manage_attr(TAL_LOG_LEVEL_DEBUG);

    /* Initialization device */
    TAL_PR_DEBUG("device_init in");
    TUYA_CALL_ERR_LOG(__soc_device_init());

    return;
}

/**
* @brief  task thread
*
* @param[in] arg:Parameters when creating a task
* @return none
*/
STATIC VOID_T tuya_app_thread(VOID_T *arg)
{
    /* Initialization LWIP first!!! */
#if defined(ENABLE_LWIP) && (ENABLE_LWIP == 1)
    TUYA_LwIP_Init();
#endif

    user_main();

    tal_thread_delete(ty_app_thread);
    ty_app_thread = NULL;
}

/**
 * @brief user entry function
 *
 * @param[in] none: 
 *
 * @return none
 */
VOID_T tuya_app_main(VOID_T)
{
    THREAD_CFG_T thrd_param = {4096, 4, "tuya_app_main"};
    tal_thread_create_and_start(&ty_app_thread, NULL, NULL, tuya_app_thread, NULL, &thrd_param);
}
