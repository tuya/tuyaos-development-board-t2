/**
* @file examples_os_wifi_ap.c
* @author www.tuya.com
* @brief examples_os_wifi_ap module is used to 
* @version 0.1
* @date 2022-05-20
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include "tuya_cloud_types.h"

#include "tal_log.h"
#include "tal_wifi.h"

/***********************************************************
*************************micro define***********************
***********************************************************/
#define DEFAULT_WIFI_CHANNEL                 5

/***********************************************************
***********************typedef define***********************
***********************************************************/


/***********************************************************
***********************variable define**********************
***********************************************************/


/***********************************************************
***********************function define**********************
***********************************************************/
/**
* @brief wifi Related event callback function
*
* @param[in] event:event
* @param[in] arg:parameter
* @return none
*/
STATIC VOID_T wifi_event_callback(WF_EVENT_E event, VOID_T *arg)
{
    TAL_PR_DEBUG("-------------event callback-------------");
}

/**
* @brief WiFi AP task
*
* @param[in] param:Task parameters
* @return none
*/
VOID example_wifi_ap(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;
    const char ap_ssid[] = "my-wifi";        // ssid
    const char ap_password[] = "12345678";     // password
    NW_IP_S nw_ip = {
        .ip = "192.168.1.123",
        .mask = "255.255.255.0",
        .gw = "192.168.1.1",
    };

    TAL_PR_NOTICE("------ wifi ap example start ------");

    /*wifi init*/
    TUYA_CALL_ERR_GOTO(tal_wifi_init(wifi_event_callback), __EXIT);

    /*Set WiFi mode to AP*/
    TUYA_CALL_ERR_LOG(tal_wifi_set_work_mode(WWM_SOFTAP));

    WF_AP_CFG_IF_S wifi_cfg = {
        .s_len = strlen(ap_ssid),       //ssid length
        .p_len = strlen(ap_password),   //password length
        .chan = DEFAULT_WIFI_CHANNEL,   //wifi channel
        .md = WAAM_WPA2_PSK,             //encryption type
        .ip = nw_ip,                    //ip information
        .ms_interval = 100,             //broadcast interval,Unit(ms)
        .max_conn = 3                   //max sta connect numbers 
    };

    strcpy((char *)wifi_cfg.ssid, ap_ssid);   //ssid
    strcpy((char *)wifi_cfg.passwd, ap_password); //password
    TUYA_CALL_ERR_LOG(tal_wifi_ap_start(&wifi_cfg));

__EXIT:
    return;
}
