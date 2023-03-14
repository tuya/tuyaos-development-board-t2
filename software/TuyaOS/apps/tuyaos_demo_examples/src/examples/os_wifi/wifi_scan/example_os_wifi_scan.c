/**
* @file examples_os_wifi_scan.c
* @author www.tuya.com
* @brief examples_os_wifi_scan module is used to 
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
* @brief WiFi scanf task
*
* @param[in] param:Task parameters
* @return none
*/
VOID example_wifi_scan(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;
    AP_IF_S *ap_info;
    UINT_T ap_info_nums;
    INT_T i = 0;
    CHAR_T info_ssid[50];

    TAL_PR_NOTICE("------ wifi scan example start ------");

    /*Scan WiFi information in the current environment*/
    TUYA_CALL_ERR_GOTO(tal_wifi_all_ap_scan(&ap_info, &ap_info_nums), __EXIT);
    TAL_PR_DEBUG("Scanf to %d wifi signals", ap_info_nums);
    for(i = 0; i < ap_info_nums; i++) {
        strcpy((char *)info_ssid, (const char *)ap_info[i].ssid);
        TAL_PR_DEBUG("channel:%d, ssid:%s", ap_info[i].channel, info_ssid);  
    }

    /*Release the acquired WiFi information in the current environment*/
    TUYA_CALL_ERR_LOG(tal_wifi_release_ap(ap_info));

__EXIT:
    return;
}
