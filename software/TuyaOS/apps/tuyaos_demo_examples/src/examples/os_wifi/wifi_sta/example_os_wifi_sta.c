/**
* @file examples_os_wifi_sta.c
* @author www.tuya.com
* @brief examples_os_wifi_sta module is used to 
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
    OPERATE_RET op_ret = OPRT_OK;
    NW_IP_S sta_info;
    memset(&sta_info, 0, SIZEOF(NW_IP_S));

    TAL_PR_DEBUG("-------------event callback-------------");
    switch (event) {
        case WFE_CONNECTED:{
            TAL_PR_DEBUG("connection succeeded!");

            /* output ip information */
            op_ret = tal_wifi_get_ip(WF_STATION, &sta_info);
            if (OPRT_OK != op_ret) {
                TAL_PR_ERR("get station ip error");
                return;
            }
            TAL_PR_NOTICE("gw: %s", sta_info.gw);
            TAL_PR_NOTICE("ip: %s", sta_info.ip);
            TAL_PR_NOTICE("mask: %s", sta_info.mask);
            break;
        }

        case WFE_CONNECT_FAILED: {
            TAL_PR_DEBUG("connection fail!");
            break;
        }

        case WFE_DISCONNECTED: {
            TAL_PR_DEBUG("disconnect!");
            break;        
        }
    }
}

/**
* @brief WiFi STA task
*
* @param[in] param:Task parameters
* @return none
*/
VOID example_wifi_sta(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;
    CHAR_T connect_ssid[] = "xxxx";    // connect wifi ssid
    CHAR_T connect_passwd[] = "xxxxxxxx";   // connect wifi password

    TAL_PR_NOTICE("------ wifi station example start ------");

    /*WiFi init*/
    TUYA_CALL_ERR_GOTO(tal_wifi_init(wifi_event_callback), __EXIT);

    /*Set WiFi mode to station*/
    TUYA_CALL_ERR_GOTO(tal_wifi_set_work_mode(WWM_STATION), __EXIT);

    /*STA mode, connect to WiFi*/
    TAL_PR_NOTICE("\r\nconnect wifi ssid: %s, password: %s\r\n", connect_ssid, connect_passwd);
    TUYA_CALL_ERR_LOG(tal_wifi_station_connect((SCHAR_T *)connect_ssid, (SCHAR_T *)connect_passwd));

__EXIT:
    return;
}
