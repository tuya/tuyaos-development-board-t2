/**
* @file examples_os_kv_tuya_db.c
* @author www.tuya.com
* @brief examples_os_kv_tuya_db module is used to 
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
* @brief base db task
*
* @param[in] param:Task parameters
* @return none
*/
VOID example_kv_tuya_db(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET op_ret = OPRT_OK;
    INT_T i;
    BYTE_T  *out_value = NULL;
    UINT_T len;

    TAL_PR_NOTICE("------ kv tuya db example start ------");

    /*read the user parameter from tuya key-value database*/
    op_ret = wd_user_param_read(&out_value, &len);
    if(OPRT_OK != op_ret) {
        TAL_PR_ERR("err<%d>, read the user parameter from tuya key-value database fail!", op_ret);
    } else {
        TAL_PR_DEBUG("len = %d", len);
        for ( i = 0; i < len; i++) {
            TAL_PR_DEBUG("out_value[%d] = %d", i, out_value[i]);
        }
    }

    /*free data*/
    op_ret = wd_common_free_data(out_value);
    if(OPRT_OK != op_ret) {
        TAL_PR_ERR("err<%d>,free data fail!", op_ret);
    }

    return;
}
