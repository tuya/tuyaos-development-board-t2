/**
* @file example_ffc_slave.c
* @author www.tuya.com
* @brief example_ffc_slave module is used to 
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
* @brief ffc slave status callback
* 
* @param[in] state: ffc state
* 
* @return oprt_ok
*/
STATIC int ffc_status_callback(ffc_cb_state_t state)
{
    TAL_PR_DEBUG("-------------------ffc slave status callback-------------");
    TAL_PR_DEBUG("FFC status : %d", state);

    return OPRT_OK;
}

/**
* @brief ffc slave receive data callback
* 
* @param[in] data_cmd: receive data 
* @param[in] data_len: data len
* 
* @return oprt_ok
*/
STATIC int ffc_recv_callback(uint8_t *data_cmd, uint16_t data_len)
{
    TAL_PR_DEBUG("-------------------ffc slave receive callback-------------");
    INT_T i  = 0;
    for ( i = 0; i < data_len; i++) {
        TAL_PR_DEBUG("data_cmd [%d] : %d", i, data_cmd[i]);
    }
    
    return OPRT_OK;
}

/**
* @brief  ffc slave init and bind
* 
* @param[in] param:Task parameters
* 
* @return none
*/
VOID example_ffc_slave_init(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;
    static uint8_t is_init = 0;

    if (0 == is_init) {
        /*ffc init*/
        TUYA_CALL_ERR_GOTO(tuya_iot_wifi_ffc_init(FFC_SLAVER, (ffc_status_cb *)ffc_status_callback, (ffc_recv_cb *)ffc_recv_callback), __EXIT);

        /* work under scan channel */
        TUYA_CALL_ERR_GOTO(tuya_iot_wifi_ffc_control(FFC_CHANNEL_SCAN_FLAG, NULL), __EXIT);
    }

    /*ffc bind*/
    TUYA_CALL_ERR_LOG(tuya_iot_wifi_ffc_bind(30));

__EXIT:
    return;
}
