/**
* @file examples_os_kv_fuzzy.c
* @author www.tuya.com
* @brief 
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
* @brief kv_fuzzy task
*
* @param[in] param:Task parameters
* @return none
*/
VOID example_kv_fuzzy(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET op_ret = OPRT_OK;
    INT_T i = 0x02;
    BYTE_T  *out_value = NULL;
    UINT_T len;
    UINT_T index = 0;

    TAL_PR_NOTICE("------ kv fuzzy example start ------");

    /*wirte data*/
    BYTE_T value[] = {0x04, 0x05, 0x06, 0x07};
    op_ret = wd_common_write("key1", value, (sizeof(value)));
    if(OPRT_OK != op_ret) {
        TAL_PR_ERR("err<%d>, write fail", op_ret);
    }
    TAL_PR_NOTICE("kv flash write data:");
    for (i=0; i<sizeof(value)/sizeof(value[0]); i++) {
        TAL_PR_DEBUG_RAW("0x%02x ", value[i]);
    }
    TAL_PR_DEBUG_RAW("\r\n\r\n");

    /*fuzzy read*/
    op_ret = wd_common_fuzzy_read("key", &index, &out_value, &len);
    if(OPRT_OK != op_ret) {
        TAL_PR_ERR("err<%d>, fuzzy read fail!", op_ret);
    } else {
        TAL_PR_DEBUG("index = %d", index);
        for (i = 0; i < len; i++) {
            TAL_PR_DEBUG("out_value[%d] = %d", i, out_value[i]);
        }
    }

    /*free data*/
    op_ret = wd_common_free_data(out_value);
    if(OPRT_OK != op_ret) {
        TAL_PR_ERR("err<%d>, free data fail!", op_ret);
    }

    /*delete key*/
    op_ret = wd_common_delete("key1");
    if(OPRT_OK != op_ret) {
        TAL_PR_ERR("err<%d>,delete fail!", op_ret);
    } else {
        TAL_PR_NOTICE("key1 is deleted in kv database");
    }

    return;
}
