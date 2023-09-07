/**
 * @file app_temp_humi.c
 * @brief RH-T sensor application demo
 * @version 0.1
 * @date 2022-09-07
 */

#include "app_temp_humi.h"
#include "tdl_sensor_hub.h"
#include "tdd_sensor_temp_humi.h"
#include "tkl_memory.h"
#include "tal_sw_timer.h"
#include "tal_log.h"
#include "tuya_iot_com_api.h"
#include "tuya_wifi_status.h"
#include "tuya_ws_db.h"

/***********************************************************
************************macro define************************
***********************************************************/

/**
 * @brief alert code
 */
#define ALT_LOWER       0
#define ALT_UPPER       1
#define ALT_CANCEL      2

/***********************************************************
***********************typedef define***********************
***********************************************************/
/**
 * @brief DP data typedef
 */
typedef struct {
    TY_OBJ_DP_VALUE_U temp;
    TY_OBJ_DP_VALUE_U humi;
    TY_OBJ_DP_VALUE_U temp_alert;
    TY_OBJ_DP_VALUE_U humi_alert;
} TH_DP_DATA_T;

/**
 * @brief flag typedef
 */
typedef struct {
    UCHAR_T turn_on         : 1;
    UCHAR_T temp_change     : 1;
    UCHAR_T humi_change     : 1;
    UCHAR_T alert           : 1;
    UCHAR_T reserved        : 4;
} TH_FLAG_T;

/***********************************************************
********************function declaration********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
/**
 * @brief sht30 process
 */
STATIC SENSOR_HANDLE_T sg_sht30_handle = NULL;
STATIC SR_TH_STATUS_U sg_sht30_status;
STATIC SR_TH_LIMIT_T sg_sht30_alert_limit;
STATIC TH_FLAG_T sg_sht30_flag;
STATIC TIMER_ID sg_sht30_report_timer;

/**
 * @brief DP process
 */
STATIC TH_DP_DATA_T sg_th_dp_data;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief report one dp data
 *
 * @param[in] dp_id: DP ID
 * @param[in] dp_type: DP type
 * @param[in] dp_value: DP value
 *
 * @return none
 */
STATIC VOID_T __repo_one_dp_data(CONST UCHAR_T dp_id, CONST UCHAR_T dp_type, CONST TY_OBJ_DP_VALUE_U dp_value)
{
    GW_WIFI_NW_STAT_E wifi_stat = STAT_LOW_POWER;
    get_wf_gw_nw_status(&wifi_stat);
    if (wifi_stat <= STAT_AP_STA_DISC || wifi_stat == STAT_STA_DISC) {
        return;
    }

    TY_OBJ_DP_S dp_repo;

    dp_repo.dpid = dp_id;
    dp_repo.type = dp_type;
    dp_repo.time_stamp = 0;

    switch (dp_type) {
    case PROP_BOOL:
        dp_repo.value.dp_bool = dp_value.dp_bool;
        break;

    case PROP_VALUE:
        dp_repo.value.dp_value = dp_value.dp_value;
        break;

    case PROP_ENUM:
        dp_repo.value.dp_enum = dp_value.dp_enum;
        break;

    case PROP_STR:
        dp_repo.value.dp_str = dp_value.dp_str;
        break;

    case PROP_BITMAP:
        dp_repo.value.dp_bitmap = dp_value.dp_bitmap;
        break;

    default:
        break;
    }

    TAL_PR_INFO("repo_one_dp_data ID:%d", dp_repo.dpid);
    OPERATE_RET op_ret = dev_report_dp_json_async(NULL, &dp_repo, 1);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("__repo_one_dp_data -- dev_report_dp_json_async, error code: %d.", op_ret);
    }
}


/**
 * @brief data ready inform
 *
 * @param[in] name: device name
 * @param[in] ele_num: number of elements
 * @param[in] ele_data: element data
 *
 * @return none
 */
STATIC VOID_T __sht30_inform_cb(CHAR_T* name, UCHAR_T ele_num, SR_ELE_BUFF_T *ele_data)
{
    INT_T temp, humi;

    for (UCHAR_T i = 0; i < ele_num; i++) {
        if (ele_data[i].id == SR_TH_ELE_ID_TEMP) {
            temp = (INT_T)(ele_data[i].val[0].sr_float * 10 * TEMP_SCALE);
            
            TAL_PR_INFO("Temp: %d sg_th_dp_data.temp.dp_value: %d", temp, sg_th_dp_data.temp.dp_value);
            if (temp != sg_th_dp_data.temp.dp_value) {
                if ((temp > (sg_th_dp_data.temp.dp_value + APP_TEMP_REPORT_INTERVAL)) || \
                    ((temp + APP_TEMP_REPORT_INTERVAL) < sg_th_dp_data.temp.dp_value)) {

                    sg_th_dp_data.temp.dp_value = temp;
                    sg_sht30_flag.temp_change = TRUE;
                    TAL_PR_INFO("Temp change. Enable report");
                }

            }
        } else {
            humi = (INT_T)(ele_data[i].val[0].sr_float * 10 * HUMI_SCALE);

            TAL_PR_INFO("Humi: %d sg_th_dp_data.humi.dp_value: %d", humi, sg_th_dp_data.humi.dp_value);
            if (humi != sg_th_dp_data.humi.dp_value) {
                if ((humi > (sg_th_dp_data.humi.dp_value + APP_HUMI_REPORT_INTERVAL)) || \
                    ((humi + APP_HUMI_REPORT_INTERVAL) < sg_th_dp_data.humi.dp_value)) {

                    sg_th_dp_data.humi.dp_value = humi;
                    sg_sht30_flag.humi_change = TRUE;
                    TAL_PR_INFO("Humi changed. Enable report");
                }

            }
        }
    }
}

/**
 * @brief sht30 alert callback
 *
 * @param none
 *
 * @return none
 */
STATIC VOID_T __sht30_alert_cb(VOID_T *args)
{
    sg_sht30_flag.alert = TRUE;
}

/**
 * @brief sht30 register
 *
 * @param none
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __sht30_register(VOID_T)
{
    OPERATE_RET op_ret;

    SR_I2C_GPIO_T i2c_gpio = {
        .scl = SHT30_SCL_PIN,
        .sda = SHT30_SDA_PIN
    };
    SR_TH_I2C_CFG_T sht30_i2c_cfg = {
        .port = 0,
        .addr = SR_I2C_ADDR_SHT3X_A,
        .gpio = i2c_gpio
    };

    SR_TH_MEAS_CFG_T sht30_meas_cfg = {
        .prec = SR_TH_PREC_HIGH,
        .freq = SR_TH_FREQ_1HZ
    };

    op_ret = tdd_sensor_sht3x_register("SHT30", sht30_i2c_cfg, sht30_meas_cfg);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("tdd_sensor_sht3x_register, error code: %d.", op_ret);
        return op_ret;
    }

    op_ret = tdl_sensor_dev_find("SHT30", &sg_sht30_handle);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("tdl_sensor_dev_find, error code: %d.", op_ret);
        return op_ret;
    }

    return OPRT_OK;
}

/**
 * @brief clear status
 *
 * @param none
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __sht30_clear_status(VOID_T)
{
    OPERATE_RET op_ret;

    op_ret = tdl_sensor_dev_config(sg_sht30_handle, SR_TH_CMD_CLR_STATUS, NULL);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("SR_TH_CMD_CLR_STATUS, error code: %d.", op_ret);
        return op_ret;
    }

    op_ret = tdl_sensor_dev_config(sg_sht30_handle, SR_TH_CMD_GET_STATUS, &sg_sht30_status);
    if (op_ret != OPRT_OK) {
        TAL_PR_ERR("SR_TH_CMD_GET_STATUS, error code: %d.", op_ret);
        return op_ret;
    }

    TAL_PR_DEBUG("Status <alert pending>: %d.", sg_sht30_status.bit.alert_pending);
    TAL_PR_DEBUG("Status <temp alert>: %d.", sg_sht30_status.bit.temp_alert);
    TAL_PR_DEBUG("Status <humi alert>: %d.", sg_sht30_status.bit.humi_alert);

    return OPRT_OK;
}


/**
 * @brief sht30 set alert
 *
 * @param none
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __sht30_set_alert(VOID_T)
{
    OPERATE_RET op_ret;

    // set alert limit
    sg_sht30_alert_limit.temp_high_set.sr_float = 125.0f;
    sg_sht30_alert_limit.temp_high_clear.sr_float = sg_sht30_alert_limit.temp_high_set.sr_float - 5.0f;
    sg_sht30_alert_limit.temp_low_set.sr_float = -40.0f;
    sg_sht30_alert_limit.temp_low_clear.sr_float = sg_sht30_alert_limit.temp_low_set.sr_float + 5.0f;
    sg_sht30_alert_limit.humi_high_set.sr_float = 100.0f;
    sg_sht30_alert_limit.humi_high_clear.sr_float = sg_sht30_alert_limit.humi_high_set.sr_float - 5.0f;
    sg_sht30_alert_limit.humi_low_set.sr_float = 0.0f;
    sg_sht30_alert_limit.humi_low_clear.sr_float = sg_sht30_alert_limit.humi_low_set.sr_float + 5.0f;

    op_ret = tdl_sensor_dev_config(sg_sht30_handle, SR_TH_CMD_SET_ALT_LIMIT, &sg_sht30_alert_limit);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("SR_TH_CMD_SET_ALT_LIMIT, error code: %d.", op_ret);
        return op_ret;
    }

    // clear alert status
    op_ret = __sht30_clear_status();
    if (op_ret != OPRT_OK) {
        return op_ret;
    }

    // enable alert function
    SR_TH_ALT_CFG_T sht30_alert_cfg;
    sht30_alert_cfg.pin = SHT30_ALT_PIN;
    sht30_alert_cfg.irq.mode = TUYA_GPIO_IRQ_RISE;
    sht30_alert_cfg.irq.cb = __sht30_alert_cb;
    sht30_alert_cfg.irq.arg = NULL;

    op_ret = tdl_sensor_dev_config(sg_sht30_handle, SR_TH_CMD_ENABLE_ALT_PIN, &sht30_alert_cfg);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("SR_TH_CMD_ENABLE_ALT_PIN, error code: %d.", op_ret);
        return op_ret;
    }

    return OPRT_OK;
}

/**
 * @brief sht30 open
 *
 * @param none
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __sht30_open(VOID_T)
{
    if (sg_sht30_flag.turn_on) {
        TAL_PR_ERR("The device is already turned on.");
        return OPRT_COM_ERROR;
    }

    OPERATE_RET op_ret;
    SR_DEV_CFG_T sht30_cfg;
    tkl_system_memset(&sht30_cfg, 0, SIZEOF(SR_DEV_CFG_T));

    sht30_cfg.mode.trig_mode = SR_MODE_POLL_SOFT_TM;
    sht30_cfg.mode.poll_intv_ms = APP_TEMP_HUMI_READ_CYCLE;
    sht30_cfg.inform_cb.ele = __sht30_inform_cb;
    sht30_cfg.fifo_size = 1;
    sht30_cfg.ele_sub = NULL;

    op_ret = tdl_sensor_dev_open(sg_sht30_handle, &sht30_cfg);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("tdl_sensor_dev_open, error code: %d.", op_ret);
        return op_ret;
    }

    sg_sht30_flag.turn_on = TRUE;
    TAL_PR_INFO("Open device successfully.");

    return OPRT_OK;
}

/**
 * @brief sht30 close
 *
 * @param none
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __sht30_close(VOID_T)
{
    if (!sg_sht30_flag.turn_on) {
        TAL_PR_ERR("The device is already turned off.");
        return OPRT_COM_ERROR;
    }

    OPERATE_RET op_ret = tdl_sensor_dev_close(sg_sht30_handle);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("tdl_sensor_dev_close, error code: %d.", op_ret);
        return op_ret;
    }

    sg_sht30_flag.turn_on = FALSE;
    TAL_PR_INFO("Close device successfully.");

    return OPRT_OK;
}

/**
 * @brief get alert code
 *
 * @param[in] cur_val: current value
 * @param[in] high_lim: high set limit
 * @param[in] low_lim: low set limit
 *
 * @return alert code
 */
STATIC UINT_T __get_alert_code(CONST FLOAT_T cur_val, CONST FLOAT_T high_lim, CONST FLOAT_T low_lim)
{
    UINT_T code;
    FLOAT_T diff_abs_h, diff_abs_l;

    if (cur_val >= high_lim) {
        diff_abs_h = cur_val - high_lim;
    } else {
        diff_abs_h = high_lim - cur_val;
    }
    if (cur_val >= low_lim) {
        diff_abs_l = cur_val - low_lim;
    } else {
        diff_abs_l = low_lim - cur_val;
    }

    if (diff_abs_h < diff_abs_l) {
        code = ALT_UPPER;
    } else {
        code = ALT_LOWER;
    }
    
    return code;
}


/**
 * @brief sht30 alert occur handler
 *
 * @param none
 *
 * @return none
 */
STATIC VOID_T __sht30_alert_occur_handler(VOID_T)
{
    OPERATE_RET op_ret = tdl_sensor_dev_config(sg_sht30_handle, SR_TH_CMD_GET_STATUS, &sg_sht30_status);
    if (op_ret != OPRT_OK) {
        TAL_PR_ERR("SR_TH_CMD_GET_STATUS, error code: %d.", op_ret);
        return;
    }

    FLOAT_T cur_val;

    if (sg_sht30_status.bit.temp_alert) {
        if (ALT_CANCEL == sg_th_dp_data.temp_alert.dp_enum) {
            TAL_PR_INFO("Temperature alert occurred.");
            cur_val = sg_th_dp_data.temp.dp_value / (10.0f * TEMP_SCALE);
            sg_th_dp_data.temp_alert.dp_enum = __get_alert_code(cur_val,\
                                                                sg_sht30_alert_limit.temp_high_set.sr_float,\
                                                                sg_sht30_alert_limit.temp_low_set.sr_float);
            __repo_one_dp_data(DP_ID_TEMP_ALT, PROP_ENUM, sg_th_dp_data.temp_alert);
        }
    } else {
        if (sg_th_dp_data.temp_alert.dp_enum != ALT_CANCEL) {
            TAL_PR_INFO("Temperature alert canceled.");
            sg_th_dp_data.temp_alert.dp_enum = ALT_CANCEL;
            __repo_one_dp_data(DP_ID_TEMP_ALT, PROP_ENUM, sg_th_dp_data.temp_alert);
        }
    }

    if (sg_sht30_status.bit.humi_alert) {
        if (ALT_CANCEL == sg_th_dp_data.humi_alert.dp_enum) {
            TAL_PR_INFO("Humidity alert occurred.");
            cur_val = sg_th_dp_data.humi.dp_value / (10.0f * HUMI_SCALE);
            sg_th_dp_data.humi_alert.dp_enum = __get_alert_code(cur_val,\
                                                                sg_sht30_alert_limit.humi_high_set.sr_float,\
                                                                sg_sht30_alert_limit.humi_low_set.sr_float);
            __repo_one_dp_data(DP_ID_HUMI_ALT, PROP_ENUM, sg_th_dp_data.humi_alert);
        }
    } else {
        if (sg_th_dp_data.humi_alert.dp_enum != ALT_CANCEL) {
            TAL_PR_INFO("Humidity alert canceled.");
            sg_th_dp_data.humi_alert.dp_enum = ALT_CANCEL;
            __repo_one_dp_data(DP_ID_HUMI_ALT, PROP_ENUM, sg_th_dp_data.humi_alert);
        }
    }

    if ((!sg_sht30_status.bit.temp_alert) && (!sg_sht30_status.bit.humi_alert)) {
        sg_sht30_flag.alert = FALSE;
    }
}

/**
 * @brief sht30 report timer callback
 *
 * @param[in] timer_id: timer ID
 * @param[in] arg: argument
 *
 * @return none
 */
STATIC VOID_T __sht30_report_timer_cb(TIMER_ID timer_id, VOID_T *arg)
{
    if (sg_sht30_flag.alert) {
        __sht30_alert_occur_handler();
    }

    if (sg_sht30_flag.temp_change) {
        TAL_PR_INFO("temp change report.");
        sg_sht30_flag.temp_change = FALSE;
        __repo_one_dp_data(DP_ID_TEMP, PROP_VALUE, sg_th_dp_data.temp);
    }

    if (sg_sht30_flag.humi_change) {
        TAL_PR_NOTICE("humi change report.");
        sg_sht30_flag.humi_change = FALSE;
        __repo_one_dp_data(DP_ID_HUMI, PROP_VALUE, sg_th_dp_data.humi);
    }
}

/**
 * @brief temp_humi main
 *
 * @param none
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET app_temp_humi_init(VOID_T)
{
    OPERATE_RET op_ret;
    BOOL_T is_exist = FALSE;

    tkl_system_memset(&sg_sht30_flag, 0, SIZEOF(TH_FLAG_T));

    sg_th_dp_data.temp.dp_value = 0;
    sg_th_dp_data.humi.dp_value = 0;
    sg_th_dp_data.temp_alert.dp_enum = ALT_CANCEL;
    sg_th_dp_data.humi_alert.dp_enum = ALT_CANCEL;
    sg_sht30_status.word = 0;

    op_ret = __sht30_register();
    if (OPRT_OK != op_ret) {
        return op_ret;
    }

    op_ret = __sht30_set_alert();
    if (OPRT_OK != op_ret) {
        return op_ret;
    }

    op_ret = __sht30_open();
    if (OPRT_OK != op_ret) {
        return op_ret;
    }

    op_ret = tal_sw_timer_create(__sht30_report_timer_cb, NULL, &sg_sht30_report_timer);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("tal_sw_timer_create -- sg_sht30_report_timer, error code: %d.", op_ret);
        return op_ret;
    } else {
        tal_sw_timer_start(sg_sht30_report_timer, APP_TEMP_HUMI_REPORT_CYCLE, TAL_TIMER_CYCLE);
    }

    //read alert limit data
    wd_common_exist(KEY_NAME, &is_exist);
    if (TRUE == is_exist) {
        BYTE_T  *read_buf = NULL;
        UINT_T read_len;
        op_ret = wd_common_read(KEY_NAME, &read_buf, &read_len);
        if (OPRT_OK != op_ret) {
            TAL_PR_ERR("wd_common_read %s error code: %d,", KEY_NAME, op_ret);
            return op_ret;
        }

        SR_TH_LIMIT_T *data = (SR_TH_LIMIT_T *)read_buf;
        sg_sht30_alert_limit.temp_high_set.sr_float = data->temp_high_set.sr_float;
        sg_sht30_alert_limit.temp_low_set.sr_float = data->temp_low_set.sr_float;
        sg_sht30_alert_limit.humi_high_set.sr_float = data->humi_high_set.sr_float;
        sg_sht30_alert_limit.humi_low_set.sr_float = data->humi_low_set.sr_float;

        if (NULL != read_buf) {
            wd_common_free_data(read_buf);
            read_buf = NULL;
        }
    } else {
        //set default value
        sg_sht30_alert_limit.temp_high_set.sr_float = 125.0f;
        sg_sht30_alert_limit.temp_low_set.sr_float = -40.0f;
        sg_sht30_alert_limit.humi_high_set.sr_float = 100.0f;
        sg_sht30_alert_limit.humi_low_set.sr_float = 0.0f;
        op_ret = wd_common_write(KEY_NAME, (BYTE_T *)&sg_sht30_alert_limit, sizeof(sg_sht30_alert_limit));
        if (OPRT_OK != op_ret) {
            TAL_PR_ERR("wd_common_write %s error code: %d,", KEY_NAME, op_ret);

            return op_ret;
        }
    }

    return OPRT_OK;
}

/**
 * @brief sht30 set alert by cloud
 *
 * @param none
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __sht30_set_alert_by_cloud(VOID_T)
{
    OPERATE_RET op_ret;

    sg_sht30_alert_limit.temp_high_clear.sr_float = sg_sht30_alert_limit.temp_high_set.sr_float;
    sg_sht30_alert_limit.temp_low_clear.sr_float = sg_sht30_alert_limit.temp_low_set.sr_float;
    sg_sht30_alert_limit.humi_high_clear.sr_float = sg_sht30_alert_limit.humi_high_set.sr_float;
    sg_sht30_alert_limit.humi_low_clear.sr_float = sg_sht30_alert_limit.humi_low_set.sr_float;

    op_ret = wd_common_write(KEY_NAME, (BYTE_T *)&sg_sht30_alert_limit, sizeof(sg_sht30_alert_limit));
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("wd_common_write %s error code: %d,", KEY_NAME, op_ret);

        return op_ret;
    }

    op_ret = tdl_sensor_dev_config(sg_sht30_handle, SR_TH_CMD_SET_ALT_LIMIT, &sg_sht30_alert_limit);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("SR_TH_CMD_SET_ALT_LIMIT, error code: %d.", op_ret);
        return op_ret;
    }

    op_ret = __sht30_clear_status();
    if (OPRT_OK != op_ret) {
        return op_ret;
    }

    return OPRT_OK;
}

/**
 * @brief switch device
 *
 * @param[in] enabled: TRUE on enabled, FALSE on disabled
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __switch_dev_by_cloud(CONST BOOL_T enabled)
{
    OPERATE_RET op_ret;
    if (enabled) {
        op_ret = __sht30_open();
    } else {
        op_ret = __sht30_close();
    }
    return op_ret;
}

/**
 * @brief DP processing
 *
 * @param[in] dp: dp data
 *
 * @return none
 */
VOID_T app_temp_humi_dp_proc(CONST TY_OBJ_DP_S *dp)
{
    OPERATE_RET op_ret;

    switch (dp->dpid) {
    case DP_ID_TEMP_MAX:
        sg_sht30_alert_limit.temp_high_set.sr_float = dp->value.dp_value / (10.0f * TEMP_SCALE);
        TAL_PR_INFO("[APP] Set the maximum temperature to %.1f.", sg_sht30_alert_limit.temp_high_set.sr_float);
        op_ret = __sht30_set_alert_by_cloud();
        break;

    case DP_ID_TEMP_MIN:
        sg_sht30_alert_limit.temp_low_set.sr_float = dp->value.dp_value / (10.0f * TEMP_SCALE);
        TAL_PR_INFO("[APP] Set the minimum temperature to %.1f.", sg_sht30_alert_limit.temp_low_set.sr_float);
        op_ret = __sht30_set_alert_by_cloud();
        break;

    case DP_ID_HUMI_MAX:
        sg_sht30_alert_limit.humi_high_set.sr_float = dp->value.dp_value / (10.0f * TEMP_SCALE);
        TAL_PR_INFO("[APP] Set the maximum humidity to %.1f.",  sg_sht30_alert_limit.humi_high_set.sr_float);
        op_ret = __sht30_set_alert_by_cloud();
        break;

    case DP_ID_HUMI_MIN:
        sg_sht30_alert_limit.humi_low_set.sr_float = dp->value.dp_value / (10.0f * TEMP_SCALE);
        TAL_PR_INFO("[APP] Set the minimum humidity to %.1f.", sg_sht30_alert_limit.humi_low_set.sr_float);
        op_ret = __sht30_set_alert_by_cloud();
        break;

    case DP_ID_SWITCH:
        TAL_PR_INFO("[APP] Switch device: %d.", dp->value.dp_bool);
        op_ret = __switch_dev_by_cloud(dp->value.dp_bool);
        break;

    default:
        op_ret = OPRT_INVALID_PARM;
        break;
    }

    if (OPRT_OK == op_ret) {
        op_ret = dev_report_dp_json_async(NULL, dp, 1);
        if (OPRT_OK != op_ret) {
            TAL_PR_ERR("app_temp_humi_dp_proc -- dev_report_dp_json_async, error code: %d.", op_ret);
        }
    }

    TAL_PR_INFO("app_temp_humi_dp_proc: %d", op_ret);
}

/**
 * @brief report all DP data
 *
 * @param none
 *
 * @return none
 */
VOID_T app_temp_humi_repo_all_dp(VOID_T)
{
    GW_WIFI_NW_STAT_E wifi_stat = STAT_LOW_POWER;
    get_wf_gw_nw_status(&wifi_stat);
    if (wifi_stat <= STAT_AP_STA_DISC || wifi_stat == STAT_STA_DISC) {
        return;
    }

    TY_OBJ_DP_S *dp_arr = (TY_OBJ_DP_S *)tkl_system_malloc(MAX_DP_NUM*SIZEOF(TY_OBJ_DP_S));
    if (NULL == dp_arr) {
        TAL_PR_ERR("dp_arr malloc failed.");
        return;
    }

    tkl_system_memset(dp_arr, 0, MAX_DP_NUM*SIZEOF(TY_OBJ_DP_S));

    dp_arr[0].dpid = DP_ID_TEMP;
    dp_arr[0].type = PROP_VALUE;
    dp_arr[0].time_stamp = 0;
    dp_arr[0].value.dp_value = sg_th_dp_data.temp.dp_value;

    dp_arr[1].dpid = DP_ID_HUMI;
    dp_arr[1].type = PROP_VALUE;
    dp_arr[1].time_stamp = 0;
    dp_arr[1].value.dp_value = sg_th_dp_data.humi.dp_value;

    dp_arr[2].dpid = DP_ID_TEMP_MAX;
    dp_arr[2].type = PROP_VALUE;
    dp_arr[2].time_stamp = 0;
    dp_arr[2].value.dp_value = (INT_T)(sg_sht30_alert_limit.temp_high_set.sr_float * (10 * TEMP_SCALE));

    dp_arr[3].dpid = DP_ID_TEMP_MIN;
    dp_arr[3].type = PROP_VALUE;
    dp_arr[3].time_stamp = 0;
    dp_arr[3].value.dp_value = (INT_T)(sg_sht30_alert_limit.temp_low_set.sr_float * (10 * TEMP_SCALE));

    dp_arr[4].dpid = DP_ID_HUMI_MAX;
    dp_arr[4].type = PROP_VALUE;
    dp_arr[4].time_stamp = 0;
    dp_arr[4].value.dp_value = (INT_T)(sg_sht30_alert_limit.humi_high_set.sr_float * (10 * HUMI_SCALE));

    dp_arr[5].dpid = DP_ID_HUMI_MIN;
    dp_arr[5].type = PROP_VALUE;
    dp_arr[5].time_stamp = 0;
    dp_arr[5].value.dp_value = (INT_T)(sg_sht30_alert_limit.humi_low_set.sr_float * (10 * HUMI_SCALE));

    dp_arr[6].dpid = DP_ID_TEMP_ALT;
    dp_arr[6].type = PROP_ENUM;
    dp_arr[6].time_stamp = 0;
    dp_arr[6].value.dp_enum = sg_th_dp_data.temp_alert.dp_enum;

    dp_arr[7].dpid = DP_ID_HUMI_ALT;
    dp_arr[7].type = PROP_ENUM;
    dp_arr[7].time_stamp = 0;
    dp_arr[7].value.dp_enum = sg_th_dp_data.humi_alert.dp_enum;

    dp_arr[8].dpid = DP_ID_SWITCH;
    dp_arr[8].type = PROP_BOOL;
    dp_arr[8].time_stamp = 0;
    dp_arr[8].value.dp_bool = sg_sht30_flag.turn_on;


    OPERATE_RET op_ret = dev_report_dp_json_async(NULL, dp_arr, MAX_DP_NUM);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("app_temp_humi_repo_all_dp -- dev_report_dp_json_async, error code: %d.", op_ret);
    } else {
        TAL_PR_INFO("app_temp_humi_repo_all_dp: %d", op_ret);
    }
    tkl_system_free(dp_arr);
    dp_arr = NULL;
}
