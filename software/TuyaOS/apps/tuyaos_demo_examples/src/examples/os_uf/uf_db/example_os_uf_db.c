/**
* @file examples_os_uf_db.c
* @author www.tuya.com
* @brief examples_os_uf_db module is used to 
* @version 0.1
* @date 2022-05-20
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include "tuya_cloud_types.h"

#include "tal_log.h"
#include "tuya_uf_db.h"

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
VOID example_uf_db(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET op_ret = OPRT_OK;
    INT_T i;
    BYTE_T *out_value = NULL;
    UINT_T len = 0;

    TAL_PR_NOTICE("------ uf db example start ------");

    /* write data */
    BYTE_T value[5] = {0x01, 0x02, 0x03, 0x04, 0x05};
    op_ret = uf_db_user_param_write(value, (sizeof(value)));
    if(OPRT_OK != op_ret) {
        TAL_PR_ERR("err<%d>, write fail", op_ret);
        return;
    }
    TAL_PR_NOTICE("uf db write data:");
    for (i=0; i<sizeof(value)/sizeof(value[0]); i++) {
        TAL_PR_DEBUG_RAW("0x%02x ", value[i]);
    }
    TAL_PR_DEBUG_RAW("\r\n\r\n");

    /*read data*/
    op_ret = uf_db_user_param_read(&out_value, &len);
    if(OPRT_OK != op_ret) {
        TAL_PR_ERR("err<%d>, read fail!", op_ret);
    } else {
        TAL_PR_DEBUG("read len %d", len);
        for ( i = 0; i < len; i++) {
            TAL_PR_DEBUG("out_value[%d] = %d", i, out_value[i]);
        }
    }

    return;
}
