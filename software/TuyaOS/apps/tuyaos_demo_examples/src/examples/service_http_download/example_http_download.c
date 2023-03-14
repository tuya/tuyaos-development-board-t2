/**
* @file example_http_download.c
* @author www.tuya.com
* @brief example_http_download module is used to 
* @version 0.1
* @date 2022-05-20
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include "tuya_cloud_types.h"
#include "iot_httpc.h"

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

STATIC OPERATE_RET http_get_file_data_cb(IN PVOID_T priv_data, IN CONST UINT_T total_len, IN CONST UINT_T offset, \
                                               IN CONST BYTE_T *data, IN CONST UINT_T len, OUT UINT_T *remain_len)
{
    INT_T i = 0;

    TAL_PR_DEBUG("--------------------http get data callback-----------------");
    TAL_PR_DEBUG("download total len: %u", total_len);
    TAL_PR_DEBUG("download offset:%u , download len:%u", offset, len);
    for(i = 0;i<len;i++) {
        TAL_PR_DEBUG_RAW("0x%02x ", data[i]);
    }
    TAL_PR_DEBUG_RAW("\r\n");

    *remain_len = 0;
    return OPRT_OK;
}

/**
* @brief  http task
*
* @param[in] param:Task parameters
* @return none
*/
VOID example_http_download(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;
    UINT_T mlk_buff_len = 1024;
    UINT_T download_len = 4096;
    UCHAR_T key[] = "gI0xxxxxxxxxxxxxxxxxxxxxxxxxxxxvx8uO";
    UCHAR_T hmac[] = "04F6xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx2C2F";
    CHAR_T download_url[] = "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

    /*http gw download file*/
    TAL_PR_NOTICE("will start downloading files");
    TUYA_CALL_ERR_LOG(http_gw_download_file(download_url, mlk_buff_len, http_get_file_data_cb, NULL, download_len, key, strlen((const char *)key), hmac));

    return;
}
