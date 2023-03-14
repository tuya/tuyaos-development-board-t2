/**
* @file examples_os_kv_protect.c
* @author www.tuya.com
* @brief examples_os_kv_protect module is used to 
* @version 0.1
* @date 2022-05-20
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include "tuya_cloud_types.h"

#include "tal_log.h"
#include "tuya_ws_db.h"

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
* @brief kv_protect task
*
* @param[in] param:Task parameters
* @return none
*/
VOID example_kv_protect(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET op_ret = OPRT_OK;
    INT_T i=0;
    BYTE_T *out_value = NULL;
    UINT_T len;

    TAL_PR_NOTICE("------ kv protect example start ------");

    /* protect write data */
    BYTE_T value[] = {0x08, 0x09, 0x0a, 0x0b};
    op_ret = wd_protected_write("key1", value, (sizeof(value)));
    if(OPRT_OK != op_ret) {
        TAL_PR_ERR("err<%d>, write fail", op_ret);
    }
    TAL_PR_NOTICE("kv flash protect write data:");
    for (i=0; i<sizeof(value)/sizeof(value[0]); i++) {
        TAL_PR_DEBUG_RAW("0x%02x ", value[i]);
    }
    TAL_PR_DEBUG_RAW("\r\n\r\n");

    /* protect read */
    op_ret = wd_protected_read("key1", &out_value, &len);
    if(OPRT_OK != op_ret) {
        TAL_PR_ERR("err<%d>, fuzzy read fail!", op_ret);
    } else {
        TAL_PR_DEBUG("len = %d", len);
        for ( i = 0; i < len; i++) {
            TAL_PR_DEBUG("out_value[%d] = %d", i, out_value[i]);
        }
    }

    /*delete key*/
    op_ret = wd_protected_delete("key1");
    if(OPRT_OK != op_ret) {
        TAL_PR_ERR("err<%d>,delete fail!", op_ret);
    } else {
        TAL_PR_NOTICE("key1 is protected deleted in kv database");
    }

    return;
}
