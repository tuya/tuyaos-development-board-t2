/**
* @file example_ffc_master.c
* @author www.tuya.com
* @brief example_ffc_master module is used to 
* @version 0.1
* @date 2022-05-20
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include "tuya_cloud_types.h"

#include "tal_log.h"
#include "ffc_app.h"

/***********************************************************
*************************micro define***********************
***********************************************************/


/***********************************************************
***********************typedef define***********************
***********************************************************/


/***********************************************************
***********************variable define**********************
***********************************************************/

/**
* @brief ffc status callback
*
* @param[in] state: ffc state
* @return oprt_ok
*/
STATIC int ffc_status_callback(ffc_cb_state_t state)
{
    TAL_PR_DEBUG("-------------------ffc status callback-------------");
    TAL_PR_DEBUG("FFC status : %d", state);

    return OPRT_OK;
}

/**
* @brief ffc receive data callback
*
* @param[in] data_cmd: receive data 
* @param[in] data_len: data len 
* @return oprt_ok
*/
STATIC int ffc_recv_callback(uint8_t *data_cmd, uint16_t data_len)
{
    TAL_PR_DEBUG("-------------------ffc receive callback-------------");
    INT_T i  = 0;
    for ( i = 0; i < data_len; i++) {
        TAL_PR_DEBUG("data_cmd [%d] : %d", i, data_cmd[i]);
    }

    return OPRT_OK;
}

/**
 * @brief ffc master init and bind
 *
 * @param[in] param:Task parameters
 *
 * @return none
 */
VOID example_ffc_master_init(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;

    static uint8_t is_init = 0;

    INT_T ffc_type = FFC_MASTER_TYPE_ONLINE;

    if (0 == is_init) {
        /* ffc init */
        TUYA_CALL_ERR_GOTO(tuya_iot_wifi_ffc_init(FFC_MASTER, (ffc_status_cb *)ffc_status_callback, (ffc_recv_cb *)ffc_recv_callback), __EXIT);

        /* ffc control */
        TUYA_CALL_ERR_GOTO(tuya_iot_wifi_ffc_control(FFC_MASTER_TYPE_SET_FLAG, &ffc_type), __EXIT);

        /* work under scan channel */
        TUYA_CALL_ERR_GOTO(tuya_iot_wifi_ffc_control(FFC_CHANNEL_SCAN_FLAG, NULL), __EXIT);

        is_init = 1;
    }

    /* Send bind packets */
    TUYA_CALL_ERR_GOTO(tuya_iot_wifi_ffc_bind(30), __EXIT);

__EXIT:
    return;
}

/**
 * @brief ffc send data
 *
 * @param[in] : 
 *
 * @return none
 */
VOID example_ffc_master_send(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;

    uint8_t send_buff[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

    TUYA_CALL_ERR_LOG(tuya_iot_wifi_ffc_send(send_buff, SIZEOF(send_buff)/SIZEOF(send_buff[0])));

    return;
}
