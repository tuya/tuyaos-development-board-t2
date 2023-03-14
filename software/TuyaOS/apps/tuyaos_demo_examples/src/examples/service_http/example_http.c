/**
* @file example_http.c
* @author www.tuya.com
* @brief example_http module is used to 
* @version 0.1
* @date 2022-05-20
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include "tuya_cloud_types.h"
#include "iot_httpc.h"
#include "ty_cJSON.h"

#include "tal_log.h"
#include "tal_system.h"

/***********************************************************
*************************micro define***********************
***********************************************************/


/***********************************************************
***********************typedef define***********************
***********************************************************/
#define WEATHER_API "tuya.device.public.weathers.get"
#define API_VERSION "1.0"

/***********************************************************
***********************variable define**********************
***********************************************************/

/**
* @brief  http task
*
* @param[in] param:Task parameters
* @return none
*/
VOID example_http(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;

    CHAR_T post_data[] = {"{\"codes\":[\"w.temp\",\"w.pm25\",\"w.sunSet\",\"w.sunRise\",\"t.local\",\"w.humidity\",\"w.condition\",\"w.windSpeed\",\"w.windDir\",\"w.windLevel\",\"w.aqi\",\"w.tips\",\"w.rank\",\"w.pm10\",\"w.o3\",\"w.no2\",\"w.co\",\"w.so2\",\"w.thigh\",\"w.tlow\",\"w.conditionNum\",\"w.realFeel\",\"w.date.7\"]}"};
    ty_cJSON* result = NULL;
    char *print_data = NULL;

    TUYA_CALL_ERR_LOG(iot_httpc_common_post_simple(WEATHER_API, API_VERSION, post_data, NULL, &result));
    if (NULL == result) {
        TAL_PR_DEBUG("result is NULL");
        return;
    }

    print_data = ty_cJSON_PrintUnformatted(result);
    if (NULL != print_data) {
        TAL_PR_NOTICE("print_data: %s", print_data);
        ty_cJSON_FreeBuffer(print_data);
        print_data = NULL;
    }

    ty_cJSON_Delete(result);
    result = NULL;

    return;
}
