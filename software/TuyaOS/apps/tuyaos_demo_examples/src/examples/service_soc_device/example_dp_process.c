/**
 * @file example_dp_process.c
 * @author www.tuya.com
 * @brief example_dp_process module is used to 
 * @version 0.1
 * @date 2022-09-22
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include "tuya_cloud_types.h"
#include "tuya_iot_com_api.h"

#include "tal_log.h"

/***********************************************************
************************macro define************************
***********************************************************/
#define DPID_SWITCH         20 /* bool */
#define DPID_MODE           21 /* enum */
#define DPID_WHITE_BRIGHT   22 /* value */
#define DPID_COLOR          24 /* string */
#define DPID_POWER_MEMORY   33 /* raw */

#define LIGHT_MODE_WHITE    0
#define LIGHT_MODE_COLOUR   1
#define LIGHT_MODE_SCENE    2
#define LIGHT_MODE_MUSIC    3

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef struct{
    UINT8_T switch_status;
    UINT8_T mode;
    UINT16_T white_bright;
    UINT8_T color[13];
    UINT8_T power_memory[12];
}LIGHT_DATA_T;

/***********************************************************
********************function declaration********************
***********************************************************/


/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC LIGHT_DATA_T light = {0};

CONST STATIC INT8_T str_buffer[][12] = {
    {"000003e803e8"},
    {"003c03e803e8"},
    {"007803e803e8"},
    {"00b403e803e8"},
    {"00f003e803e8"},
    {"012c03e803e8"},
    {"016803e803e8"},
};

CONST STATIC UINT8_T raw_buffer[][12] = {
    {0x00, 0x02, 0x00, 0x00, 0x03, 0xe8, 0x03, 0xe8, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x02, 0x00, 0x3c, 0x03, 0xe8, 0x03, 0xe8, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x02, 0x00, 0x78, 0x03, 0xe8, 0x03, 0xe8, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x02, 0x00, 0xb4, 0x03, 0xe8, 0x03, 0xe8, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x02, 0x00, 0xfc, 0x03, 0xe8, 0x03, 0xe8, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x02, 0x01, 0x2c, 0x03, 0xe8, 0x03, 0xe8, 0x00, 0x00, 0x00, 0x00},
};

/***********************************************************
***********************function define**********************
***********************************************************/

/**
 * @brief Output the received data and reply to the cloud
 *
 * @param[in] dp_data: received data
 *
 * @return none
 */
VOID dp_obj_process(TY_OBJ_DP_S *dp_data)
{
    OPERATE_RET rt = OPRT_OK;

    switch (dp_data->dpid) {
        case DPID_SWITCH:
            TAL_PR_NOTICE("switch: %d", dp_data->value.dp_bool);
            light.switch_status = dp_data->value.dp_bool;
        break;
        case DPID_MODE:
            TAL_PR_NOTICE("mode: %d", dp_data->value.dp_enum);
            light.mode = dp_data->value.dp_enum;
        break;
        case DPID_WHITE_BRIGHT:
            TAL_PR_NOTICE("white bright: %d", dp_data->value.dp_value);
            light.white_bright = dp_data->value.dp_value;
        break;
        case DPID_COLOR:
            TAL_PR_NOTICE("color: %s", dp_data->value.dp_str);
            memset(light.color, 0, 12);
            memcpy(light.color, dp_data->value.dp_str, 12);
        break;
        default: break;
    }

    TUYA_CALL_ERR_LOG(dev_report_dp_json_async(NULL, dp_data, 1));

    return;
}

/**
 * @brief Output the received raw type data and reply to the cloud
 *
 * @param[in] raw_dp: received raw type data
 *
 * @return none
 */
VOID dp_raw_process(TY_RECV_RAW_DP_S *raw_dp)
{
    OPERATE_RET rt = OPRT_OK;

    TAL_PR_NOTICE("raw dp id: %s", raw_dp->dpid);

    for (int i=0; i<raw_dp->len; i++) {
        TAL_PR_DEBUG("0x%02x ", raw_dp->data[i]);
    }
    memcpy(light.power_memory, raw_dp->data, 12);
    TUYA_CALL_ERR_LOG(dev_report_dp_raw_sync(raw_dp->cid, raw_dp->dpid, raw_dp->data, raw_dp->len, 0));

    return;
}

/**
 * @brief report all dp to the cloud
 *
 * @param[in] none: 
 *
 * @return none
 */
VOID example_update_device_status(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;
    TY_OBJ_DP_S all_report_data[4];

    memset(all_report_data, 0, 4*SIZEOF(TY_OBJ_DP_S));

    /* bool type data */
    all_report_data[0].dpid = DPID_SWITCH;
    all_report_data[0].type = PROP_BOOL;
    all_report_data[0].value.dp_bool = light.switch_status;
    all_report_data[0].time_stamp = 0;

    /* enum type data */
    all_report_data[1].dpid = DPID_MODE;
    all_report_data[1].type = PROP_ENUM;
    all_report_data[1].value.dp_enum = light.mode;
    all_report_data[1].time_stamp = 0;

    /* value type data */
    all_report_data[2].dpid = DPID_WHITE_BRIGHT;
    all_report_data[2].type = PROP_VALUE;
    all_report_data[2].value.dp_value = light.white_bright;
    all_report_data[2].time_stamp = 0;

    /* string type data */
    all_report_data[3].dpid = DPID_COLOR;
    all_report_data[3].type = PROP_STR;
    all_report_data[3].value.dp_str = (CHAR_T *)light.color;
    all_report_data[3].time_stamp = 0;

    TUYA_CALL_ERR_LOG(dev_report_dp_json_async(NULL, all_report_data, 4));

    /* raw type data */
    TUYA_CALL_ERR_LOG(dev_report_dp_raw_sync(NULL, DPID_POWER_MEMORY, light.power_memory, 12, 0));

    return;
}

/**
 * @brief Change the current device switch status and report it to the cloud.
 *
 * @param[in] none: 
 *
 * @return none
 */
VOID example_dp_bool(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;
    TY_OBJ_DP_S report_data = {0};

    light.switch_status = (light.switch_status) ? (0) : (1);

    report_data.dpid = DPID_SWITCH;
    report_data.type = PROP_BOOL;
    report_data.value.dp_bool = light.switch_status;
    report_data.time_stamp = 0;

    TUYA_CALL_ERR_LOG(dev_report_dp_json_async(NULL, &report_data, 1));

    return;
}

/**
 * @brief Change the current device mode status and report it to the cloud
 *
 * @param[in] none: 
 *
 * @return none
 */
VOID example_dp_enum(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;
    TY_OBJ_DP_S report_data = {0};

    light.mode = (0 == light.mode) ? (1) : (0);

    report_data.dpid = DPID_MODE;
    report_data.type = PROP_ENUM;
    report_data.value.dp_enum = light.mode;
    report_data.time_stamp = 0;

    TUYA_CALL_ERR_LOG(dev_report_dp_json_async(NULL, &report_data, 1));

    return;
}

/**
 * @brief Change the current device white bright status and report it to the cloud
 *
 * @param[in] : 
 *
 * @return none
 */
VOID example_dp_value(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;
    TY_OBJ_DP_S report_data = {0};

    /* 10 - 1000 */
    light.white_bright = (light.white_bright + 100) % 1000;

    if (0 == light.white_bright) {
        light.white_bright = 10;
    }

    report_data.dpid = DPID_WHITE_BRIGHT;
    report_data.type = PROP_VALUE;
    report_data.value.dp_value = light.white_bright;
    report_data.time_stamp = 0;

    TUYA_CALL_ERR_LOG(dev_report_dp_json_async(NULL, &report_data, 1));

    return;
}

/**
 * @brief Change the current device colour status and report it to the cloud
 *
 * @param[in] : 
 *
 * @return none
 */
VOID example_dp_str(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;
    TY_OBJ_DP_S report_data = {0};

    static UINT8_T colour_idx = 0;

    colour_idx = (colour_idx+1) % 7;

    memset(light.color, 0, 13);
    memcpy(light.color, str_buffer[colour_idx], 12);

    report_data.dpid = DPID_COLOR;
    report_data.type = PROP_STR;
    report_data.value.dp_str = (CHAR_T *)light.color;
    report_data.time_stamp = 0;

    TUYA_CALL_ERR_LOG(dev_report_dp_json_async(NULL, &report_data, 1));

    return;
}

/**
 * @brief Change the current device power memory status and report it to the cloud
 *
 * @param[in] none: 
 *
 * @return none
 */
VOID example_dp_raw(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;
    static UINT8_T raw_idx = 0;

    raw_idx = (raw_idx+1) % 6;

    memcpy(light.power_memory, raw_buffer[raw_idx], 12);

    TUYA_CALL_ERR_LOG(dev_report_dp_raw_sync(NULL, DPID_POWER_MEMORY, light.power_memory, 12, 0));

    return;
}
