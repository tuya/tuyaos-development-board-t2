/**
* @file examples_os_kv_common.c
* @author www.tuya.com
* @brief examples_os_kv_common module is used to 
* @version 0.1
* @date 2022-05-20
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include "tuya_cloud_types.h"

#include "tuya_ws_db.h"
#include "tal_log.h"

/***********************************************************
*************************micro define***********************
***********************************************************/
#define KEY_NAME "my_key"

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
* @brief kv_common task
*
* @param[in] param:Task parameters
* @return none
*/
VOID example_kv_common(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;
    INT_T i;
    BYTE_T  *read_buf = NULL;
    UINT_T read_len;
    BOOL_T is_exist = FALSE;

    TAL_PR_NOTICE("------ kv common example start ------");

    /* Write "my_key" value to kv database */
    BYTE_T write_buf[] = {0x00, 0x01, 0x02, 0x03};

    TUYA_CALL_ERR_GOTO(wd_common_write(KEY_NAME, write_buf, CNTSOF(write_buf)), __EXIT);

    /* Output the values written to the KV database */
    TAL_PR_NOTICE("kv flash write data:");
    for (i=0; i<CNTSOF(write_buf); i++) {
        TAL_PR_DEBUG_RAW("0x%02x ", write_buf[i]);
    }
    TAL_PR_DEBUG_RAW("\r\n\r\n");

    /* Check whether "my_key" exists in the kv database */
    TUYA_CALL_ERR_GOTO(wd_common_exist(KEY_NAME, &is_exist), __EXIT);
    if (FALSE == is_exist) {
        TAL_PR_NOTICE("my_key is not exist in the kv database");
        goto __EXIT;
    }

    /* read "my_key" value from kv database */
    TUYA_CALL_ERR_GOTO(wd_common_read(KEY_NAME, &read_buf, &read_len), __EXIT);

    TAL_PR_NOTICE("read len %d, read data:", read_len);
    for ( i = 0; i < read_len; i++) {
        TAL_PR_DEBUG_RAW("0x%02x ", read_buf[i]);
    }
    TAL_PR_DEBUG_RAW("\r\n\r\n");

__EXIT:
    /*free data*/
    if (NULL != read_buf) {
        TUYA_CALL_ERR_LOG(wd_common_free_data(read_buf));
        read_buf = NULL;
    }

    /*delete key*/
    rt = wd_common_delete(KEY_NAME);
    if(OPRT_OK != rt) {
        TAL_PR_ERR("err<%d>,delete fail!", rt);
    } else {
        TAL_PR_NOTICE("my_key is deleted in kv database");
    }

    return;
}
