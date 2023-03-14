/**
* @file tuya_device.c
* @author www.tuya.com
* @brief tuya_device module is used to 
* @version 0.1
* @date 2022-05-20
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include "tuya_cloud_types.h"

#include "tal_system.h"
#include "tal_log.h"
#include "tal_bluetooth.h"

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
* @brief bluebooth event callback function
*
* @param[in] p_event: bluebooth event
* @return none
*/
STATIC VOID_T __ble_central_event_callback(TAL_BLE_EVT_PARAMS_T *p_event)
{
    TAL_PR_DEBUG("----------ble_central event callback-------");
    TAL_PR_DEBUG("ble_central event is : %d", p_event->type);
    switch(p_event->type) {
        case TAL_BLE_EVT_ADV_REPORT: {
            INT_T i = 0;

            /*printf peer addr and addr type*/
            TAL_PR_DEBUG_RAW("Scanf device peer addr: ");
            for(i = 0;i<6;i++) {
                TAL_PR_DEBUG_RAW("  %d", p_event->ble_event.adv_report.peer_addr.addr[i]);
            }
            TAL_PR_DEBUG_RAW(" \r\n");

            if(TAL_BLE_ADDR_TYPE_RANDOM == p_event->ble_event.adv_report.peer_addr.type) {
                TAL_PR_DEBUG("Peer addr type is random address");
            } else {
                TAL_PR_DEBUG("Peer addr type is public address");
            }

            /*printf ADV type*/
            switch(p_event->ble_event.adv_report.adv_type) {
                case TAL_BLE_ADV_DATA: {
                    TAL_PR_DEBUG("ADV data only!");
                    break;
                }

                case TAL_BLE_RSP_DATA: {
                    TAL_PR_DEBUG("Scan Response Data only!");
                    break;
                }

                case TAL_BLE_ADV_RSP_DATA: {
                    TAL_PR_DEBUG("ADV data and Scan Response Data!");
                    break;
                }
                default: break;
            }

            /*printf ADV rssi*/
            TAL_PR_DEBUG("Received Signal Strength Indicator : %d", p_event->ble_event.adv_report.rssi);

            /*printf ADV data*/
            TAL_PR_DEBUG("Advertise packet data length : %d", p_event->ble_event.adv_report.data_len);
            TAL_PR_DEBUG_RAW("Advertise packet data: ");
            for(i = 0;i<p_event->ble_event.adv_report.data_len;i++) {
                TAL_PR_DEBUG_RAW("  0x%02X", p_event->ble_event.adv_report.p_data[i]);
            }
            TAL_PR_DEBUG_RAW(" \r\n");

            break;
        }
        default: break;
    }

    tal_ble_scan_stop();
}

/**
* @brief ble_central task
*
* @param[in] param:Task parameters
* @return none
*/
VOID example_ble_central(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;
    TAL_BLE_SCAN_PARAMS_T scan_cfg;
    memset(&scan_cfg, 0, SIZEOF(TAL_BLE_SCAN_PARAMS_T));

    /*ble_central init*/
    TUYA_CALL_ERR_GOTO(tal_ble_bt_init(TAL_BLE_ROLE_CENTRAL, __ble_central_event_callback), __EXIT);

    /*start scan*/
    scan_cfg.type = TAL_BLE_SCAN_TYPE_ACTIVE;
    scan_cfg.scan_interval = 0x400;
    scan_cfg.scan_window = 0x400;
    scan_cfg.timeout = 0xFFFF;
    scan_cfg.filter_dup = TLS_DISABLE;
    TUYA_CALL_ERR_GOTO(tal_ble_scan_start(&scan_cfg), __EXIT);

    TAL_PR_NOTICE("ble central init success");

__EXIT:
    return;
}
