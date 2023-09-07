/**
 * @file tdd_sensor_cht8305.c
 * @brief tdd_sensor_cht8305 module is used to drive sht8305 chip
 * @version 0.1
 * @date 2022-08-03
 */

#include "tdl_sensor_driver.h"
#include "tdd_sensor_temp_humi.h"
#include "tkl_gpio.h"
#include "tal_system.h"
#include "tal_log.h"

/***********************************************************
************************macro define************************
***********************************************************/
/**
 * @brief work status
 */
#define CHT8305_CLOSE           0
#define CHT8305_OPEN            (!CHT8305_CLOSE)

/**
 * @brief resource array index
 */
#define CHT8305_RSRC_INDEX_STAT 0
#define CHT8305_RSRC_INDEX_ADDR 1

/**
 * @brief resolution value
 */
#define CHT8305_RES_8           (2<<8)  // T_RES: -, H_RES: 10 (T_RES has no 8-bit)
#define CHT8305_RES_11          (5<<8)  // T_RES: 1, H_RES: 01
#define CHT8305_RES_14          (0<<8)  // T_RES: 0, H_RES: 00

/**
 * @brief config register bit
 */
#define CHT8305_CFG_BIT_SRST    (1<<15)
#define CHT8305_CFG_BIT_HEATER  (1<<13)
#define CHT8305_CFG_BIT_TRES    (1<<10)
#define CHT8305_CFG_BIT_HRES    (3<<8)
#define CHT8305_CFG_BIT_APS     (1<<5)
#define CHT8305_CFG_BIT_HALT    (1<<4)
#define CHT8305_CFG_BIT_TALT    (1<<3)
#define CHT8305_CFG_BIT_STATUS  (7<<3)

/***********************************************************
***********************typedef define***********************
***********************************************************/
/**
 * @brief cht8305 register address
 */
typedef BYTE_T CHT8305_RA_E;
#define CHT8305_RA_TEMP         0x00    // temperature measurement data
#define CHT8305_RA_HUMI         0x01    // humidity measurement data
#define CHT8305_RA_CONFIG       0x02    // configuration and status
#define CHT8305_RA_ALERT        0x03    // alert limit
#define CHT8305_RA_MFR_ID       0xFE    // manufacture ID
#define CHT8305_RA_VER_ID       0xFF    // version ID

/***********************************************************
********************function declaration********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
/**
 * @brief element value type
 */
#if (SR_CONFIG_NO_FPU == 1)
STATIC SR_ELE_CFG_T sg_ele_val_tp[SR_TH_ELE_NUM] = {
    {
        .id     = SR_TH_ELE_ID_TEMP,
        .val_tp = SR_VAL_TP_INT,
    },

    {
        .id     = SR_TH_ELE_ID_HUMI,
        .val_tp = SR_VAL_TP_INT,
    }
};
#else
STATIC SR_ELE_CFG_T sg_ele_val_tp[SR_TH_ELE_NUM] = {
    {
        .id     = SR_TH_ELE_ID_TEMP,
        .val_tp = SR_VAL_TP_FLOAT,
    },

    {
        .id     = SR_TH_ELE_ID_HUMI,
        .val_tp = SR_VAL_TP_FLOAT,
    }
};
#endif

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief delay (ms)
 *
 * @param[in] tm: delay time
 *
 * @return none
 */
STATIC VOID_T __cht8305_delay_ms(IN CONST ULONG_T tm)
{
    tal_system_sleep(tm);
}

/**
 * @brief read data from cht8305's register
 *
 * @param[in] dev: device resource
 * @param[in] reg_addr: register address
 * @param[out] reg_val: register value
 * @param[in] reg_num: number of registers to read
 * @param[in] wait_ms: waiting time after send register address (ms)
 *
 * @return none
 */
STATIC VOID_T __cht8305_read_register(IN CONST SR_RSRC_T *dev, IN CHT8305_RA_E reg_addr, OUT USHORT_T *reg_val, \
                                      IN CONST UCHAR_T reg_num, IN CONST ULONG_T wait_ms)
{
    if (reg_num == 0 || reg_num > 2) {
        return;
    }

    SR_I2C_MSG_T wr_msg = {
        .flags = SR_I2C_FLAG_WR,
        .addr = dev->info[CHT8305_RSRC_INDEX_ADDR],
        .len = 1,
        .buff = &reg_addr
    };
    tdd_sensor_i2c_xfer(dev->port, &wr_msg);

    if (wait_ms) {
        __cht8305_delay_ms(wait_ms);
    }

    UCHAR_T buf[4] = {0};
    SR_I2C_MSG_T rd_msg = {
        .flags = SR_I2C_FLAG_RD,
        .addr = dev->info[CHT8305_RSRC_INDEX_ADDR],
        .len = (reg_num * 2),
        .buff = buf
    };
    tdd_sensor_i2c_xfer(dev->port, &rd_msg);

    for (UCHAR_T i = 0; i < reg_num; i++) {
        reg_val[i] = ((USHORT_T)buf[i*2] << 8) | buf[i*2+1];
    }
}

/**
 * @brief write data to cht8305's register
 *
 * @param[in] dev: device resource
 * @param[in] reg_addr: register address
 * @param[in] reg_val: register value
 *
 * @return none
 */
STATIC VOID_T __cht8305_write_register(IN CONST SR_RSRC_T *dev, IN CONST CHT8305_RA_E reg_addr, IN CONST USHORT_T reg_val)
{
    UCHAR_T buf[3];
    buf[0] = reg_addr;
    buf[1] = (UCHAR_T)(reg_val >> 8);
    buf[2] = (UCHAR_T)(reg_val & 0xFF);

    SR_I2C_MSG_T wr_msg = {
        .flags = SR_I2C_FLAG_WR,
        .addr = dev->info[CHT8305_RSRC_INDEX_ADDR],
        .len = 3,
        .buff = buf
    };
    tdd_sensor_i2c_xfer(dev->port, &wr_msg);
}

/**
 * @brief calculate temperature
 *
 * @param[in] temp: temperature raw data from sensor
 * @param[out] temp_val: temperature value in °C
 *
 * @return none
 */
STATIC VOID_T __cht8305_calc_temp(IN CONST USHORT_T temp, OUT SR_VAL_U *temp_val)
{
#if (SR_CONFIG_NO_FPU == 1)
    temp_val->sr_int = temp * 165 / 65535 - 40;
#else
    temp_val->sr_float = temp * 165.0f / 65535.0f - 40.0f;
#endif
}

/**
 * @brief calculate raw temperature
 *
 * @param[in] temp_val: temperature value in °C
 *
 * @return temperature raw data to sensor
 */
STATIC USHORT_T __cht8305_calc_temp_raw(IN CONST SR_VAL_U temp_val)
{
#if (SR_CONFIG_NO_FPU == 1)
    return ((temp_val.sr_int + 40) * 65535 / 165)
#else
    return ((temp_val.sr_float + 40.0f) * 65535.0f / 165.0f);
#endif
}

/**
 * @brief calculate humidity
 *
 * @param[in] humi: humidity raw data from sensor
 * @param[out] humi_val: humidity value in %RH
 *
 * @return none
 */
STATIC VOID_T __cht8305_calc_humi(IN CONST USHORT_T humi, OUT SR_VAL_U *humi_val)
{
#if (SR_CONFIG_NO_FPU == 1)
    humi_val->sr_int = humi * 100 / 65535;
#else
    humi_val->sr_float = humi * 100.0f / 65535.0f;
#endif
}

/**
 * @brief calculate raw humidity
 *
 * @param[in] humi_val: humidity value in %RH
 *
 * @return humidity raw data to sensor
 */
STATIC USHORT_T __cht8305_calc_humi_raw(IN CONST SR_VAL_U humi_val)
{
#if (SR_CONFIG_NO_FPU == 1)
    return (humi_val.sr_int * 65535 / 100)
#else
    return (humi_val.sr_float * 65535.0f / 100.0f);
#endif
}

/**
 * @brief read temperature and humidity from cht8305
 *
 * @param[in] dev: device resource
 * @param[out] temp: temperature value
 * @param[out] humi: humidity value
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __cht8305_read_temp_humi(IN CONST SR_RSRC_T *dev, OUT USHORT_T *temp, OUT USHORT_T *humi)
{
    USHORT_T reg_val[2] = {0};

    __cht8305_read_register(dev, CHT8305_RA_TEMP, reg_val, 2, 20);

    *temp = reg_val[0];
    *humi = reg_val[1];

    return OPRT_OK;
}

/**
 * @brief get config value to be written
 *
 * @param[in] dev: device resource
 * @param[in] cfg_bit: config register bit
 * @param[in] bit_val: bit value
 *
 * @return config value to be written
 */
STATIC USHORT_T __cht8305_get_config(IN CONST SR_RSRC_T *dev, IN CONST USHORT_T cfg_bit, IN CONST USHORT_T bit_val)
{
    USHORT_T config_val;
    __cht8305_read_register(dev, CHT8305_RA_CONFIG, &config_val, 1, 0);
    config_val &= (~cfg_bit);
    config_val |= bit_val;
    return config_val;
}

/**
 * @brief set precision
 *
 * @param[in] dev: device resource
 * @param[in] prec: precision level
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __cht8305_set_precision(IN CONST SR_RSRC_T *dev, IN CONST SR_TH_PREC_E *prec)
{
    if ((prec == NULL) || (*prec >= SR_TH_PREC_INVALID)) {
        return OPRT_INVALID_PARM;
    }

    USHORT_T res;
    switch (*prec) {
    default:
    case SR_TH_PREC_HIGH:
        res = __cht8305_get_config(dev, CHT8305_CFG_BIT_TRES|CHT8305_CFG_BIT_HRES, CHT8305_RES_14);
        TAL_PR_INFO("CHT8305's resolution has been set to 14-bit.");
        break;
    case SR_TH_PREC_MEDIUM:
        res = __cht8305_get_config(dev, CHT8305_CFG_BIT_TRES|CHT8305_CFG_BIT_HRES, CHT8305_RES_11);
        TAL_PR_INFO("CHT8305's resolution has been set to 11-bit.");
        break;
    case SR_TH_PREC_LOW:
        res = __cht8305_get_config(dev, CHT8305_CFG_BIT_HRES, CHT8305_RES_8);
        TAL_PR_INFO("CHT8305's humidity resolution has been set to 8-bit." );
        break;
    }
    __cht8305_write_register(dev, CHT8305_RA_CONFIG, res);

    return OPRT_OK;
}

/**
 * @brief set alert limit
 *
 * @param[in] dev: device resource
 * @param[in] limit: alert limit value
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __cht8305_set_alert_limit(IN CONST SR_RSRC_T *dev, IN CONST SR_TH_LIMIT_T *limit)
{
#if (SR_CONFIG_NO_FPU == 1)
    if (limit->humi_high_set.sr_int < 0 || limit->humi_high_set.sr_int > 100 ||\
        limit->temp_high_set.sr_int < -40 || limit->temp_high_set.sr_int > 125) {
        TAL_PR_ERR("[CHT8305] The limit value is invalid.");
        return OPRT_INVALID_PARM;
    }
#else
    if (limit->humi_high_set.sr_float < 0.0f || limit->humi_high_set.sr_float > 100.0f ||\
        limit->temp_high_set.sr_float < -40.0f || limit->temp_high_set.sr_float > 125.0f) {
        TAL_PR_ERR("[CHT8305] The limit value is invalid.");
        return OPRT_INVALID_PARM;
    }
#endif

    USHORT_T limit_raw = ((__cht8305_calc_humi_raw(limit->humi_high_set) & 0xFE00) |\
                          ((__cht8305_calc_temp_raw(limit->temp_high_set) >> 7) & 0x001FF));

    __cht8305_write_register(dev, CHT8305_RA_ALERT, limit_raw);

    return OPRT_OK;
}

/**
 * @brief get alert limit
 *
 * @param[in] dev: device resource
 * @param[out] limit: alert limit value
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __cht8305_get_alert_limit(IN CONST SR_RSRC_T *dev, OUT SR_TH_LIMIT_T *limit)
{
    USHORT_T limit_raw;

    __cht8305_read_register(dev, CHT8305_RA_ALERT, &limit_raw, 1, 0);

    __cht8305_calc_temp((limit_raw << 7), &limit->temp_high_set);
    __cht8305_calc_humi((limit_raw & 0xFE00), &limit->humi_high_set);

    return OPRT_OK;
}

/**
 * @brief enable alert function
 *
 * @param[in] dev: device resource
 * @param[in] alert: alert pin configuration
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __cht8305_enable_alert(IN CONST SR_RSRC_T *dev, IN SR_TH_ALT_CFG_T *alert)
{
    OPERATE_RET op_ret;

    op_ret = tkl_gpio_irq_init(alert->pin, &alert->irq);
    if (op_ret != OPRT_OK) {
        TAL_PR_ERR("[CHT8305] P%d interrupt for alert init failed, error code: %d", alert->pin, op_ret);
        return op_ret;
    }
    op_ret = tkl_gpio_irq_enable(alert->pin);

    return OPRT_OK;
}

/**
 * @brief disable alert function
 *
 * @param[in] dev: device resource
 * @param[in] alert_pin: alert pin name
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __cht8305_disable_alert(IN CONST SR_RSRC_T *dev, IN CONST TUYA_GPIO_NUM_E *alert_pin)
{
    OPERATE_RET op_ret;

    op_ret = tkl_gpio_irq_disable(*alert_pin);
    if (op_ret != OPRT_OK) {
        TAL_PR_ERR("[CHT8305] P%d interrupt for alert disable failed, error code: %d", *alert_pin, op_ret);
        return op_ret;
    }

    return OPRT_OK;
}

/**
 * @brief read status
 *
 * @param[in] dev: device resource
 * @param[out] status: status register value
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __cht8305_read_status(IN CONST SR_RSRC_T *dev, OUT SR_TH_STATUS_U *status)
{
    USHORT_T config_val;
    __cht8305_read_register(dev, CHT8305_RA_CONFIG, &config_val, 1, 0);

    status->bit.alert_pending = (config_val & CHT8305_CFG_BIT_APS) ? 1 : 0;
    status->bit.temp_alert = (config_val & CHT8305_CFG_BIT_TALT) ? 1 : 0;
    status->bit.humi_alert = (config_val & CHT8305_CFG_BIT_HALT) ? 1 : 0;

    return OPRT_OK;
}

/**
 * @brief clear status
 *
 * @param[in] dev: device resource
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __cht8305_clear_status(IN CONST SR_RSRC_T *dev)
{
    USHORT_T status = __cht8305_get_config(dev, CHT8305_CFG_BIT_STATUS, 0);
    __cht8305_write_register(dev, CHT8305_RA_CONFIG, status);
    return OPRT_OK;
}

/**
 * @brief set heater
 *
 * @param[in] dev: device resource
 * @param[in] enabled: TRUE on enabled, FALSE on disabled
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __cht8305_set_heater(IN CONST SR_RSRC_T *dev, IN CONST BOOL_T enabled)
{
    USHORT_T heater;

    if (enabled) {
        heater = __cht8305_get_config(dev, CHT8305_CFG_BIT_HEATER, CHT8305_CFG_BIT_HEATER);
    } else {
        heater = __cht8305_get_config(dev, CHT8305_CFG_BIT_HEATER, 0);
    }
    __cht8305_write_register(dev, CHT8305_RA_CONFIG, heater);

    return OPRT_OK;
}

/**
 * @brief soft reset
 *
 * @param[in] dev: device resource
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __cht8305_soft_reset(IN CONST SR_RSRC_T *dev)
{
    USHORT_T reset = __cht8305_get_config(dev, CHT8305_CFG_BIT_SRST, CHT8305_CFG_BIT_SRST);
    __cht8305_write_register(dev, CHT8305_RA_CONFIG, reset);
    return OPRT_OK;
}

/**
 * @brief read serial number
 *
 * @param[in] dev: device resource
 * @param[out] serial_nbr: serial number
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __cht8305_read_serial_number(IN CONST SR_RSRC_T *dev, OUT SR_TH_SERIAL_NBR_T *serial_nbr)
{
    USHORT_T ver_id;
    __cht8305_read_register(dev, CHT8305_RA_VER_ID, &ver_id, 1, 0);
    serial_nbr->val = (UINT_T)ver_id;
    return OPRT_OK;
}

/**
 * @brief open cht8305 device
 *
 * @param[inout] dev: device resource
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdd_sensor_cht8305_open(INOUT SR_RSRC_T *dev)
{
    dev->info[CHT8305_RSRC_INDEX_STAT] = CHT8305_OPEN;
    return OPRT_OK;
}

/**
 * @brief close cht8305 device
 *
 * @param[inout] dev: device resource
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdd_sensor_cht8305_close(INOUT SR_RSRC_T *dev)
{
    dev->info[CHT8305_RSRC_INDEX_STAT] = CHT8305_CLOSE;
    return OPRT_OK;
}

/**
 * @brief control cht8305 device
 *
 * @param[in] dev: device resource
 * @param[in] cmd: control command
 * @param[inout] param: command parameter, data type depends on the specified command
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdd_sensor_cht8305_control(IN SR_RSRC_T* dev, IN UCHAR_T cmd, INOUT VOID_T *param)
{
    OPERATE_RET op_ret = OPRT_OK;

    if ((CHT8305_OPEN == dev->info[CHT8305_RSRC_INDEX_STAT]) &&\
        (cmd == SR_TH_CMD_SOFT_RESET ||\
         cmd == SR_TH_CMD_GET_SERIAL_NBR)) {
        TAL_PR_ERR("[CHT8305] Command <%d> is only supported when acquisition is closed.");
        return OPRT_COM_ERROR;
    }

    switch (cmd) {
    case SR_TH_CMD_SOFT_RESET:
        op_ret = __cht8305_soft_reset(dev);
        break;
    case SR_TH_CMD_GET_SERIAL_NBR:
        op_ret = __cht8305_read_serial_number(dev, (SR_TH_SERIAL_NBR_T *)param);
        break;
    case SR_TH_CMD_SET_PREC:
        op_ret = __cht8305_set_precision(dev, (SR_TH_PREC_E *)param);
        break;
    case SR_TH_CMD_SET_ALT_LIMIT:
        op_ret = __cht8305_set_alert_limit(dev, (SR_TH_LIMIT_T *)param);
        break;
    case SR_TH_CMD_GET_ALT_LIMIT:
        op_ret = __cht8305_get_alert_limit(dev, (SR_TH_LIMIT_T *)param);
        break;
    case SR_TH_CMD_ENABLE_ALT_PIN:
        op_ret = __cht8305_enable_alert(dev, (SR_TH_ALT_CFG_T *)param);
        break;
    case SR_TH_CMD_DISABLE_ALT_PIN:
        op_ret = __cht8305_disable_alert(dev, (TUYA_GPIO_NUM_E *)param);
        break;
    case SR_TH_CMD_GET_STATUS:
        op_ret = __cht8305_read_status(dev, (SR_TH_STATUS_U *)param);
        break;
    case SR_TH_CMD_CLR_STATUS:
        op_ret = __cht8305_clear_status(dev);
        break;
    case SR_TH_CMD_ENABLE_HEATER:
        op_ret = __cht8305_set_heater(dev, TRUE);
        break;
    case SR_TH_CMD_DISABLE_HEATER:
        op_ret = __cht8305_set_heater(dev, FALSE);
        break;
    default:
        op_ret = OPRT_INVALID_PARM;
        TAL_PR_ERR("[CHT8305] Command <%d> is invalid.", cmd);
        break;
    }
    return op_ret;
}

/**
 * @brief read data from cht8305 device
 *
 * @param[in] dev: device
 * @param[inout] ele_data: element data
 * @param[in] ele_num: number of elements
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdd_sensor_cht8305_read(IN SR_RSRC_T* dev, INOUT SR_ELE_DATA_T *ele_data, IN UCHAR_T ele_num)
{
    USHORT_T temp, humi;
    OPERATE_RET op_ret = __cht8305_read_temp_humi(dev, &temp, &humi);
    if (op_ret != OPRT_OK) {
        return op_ret;
    }

    for (UCHAR_T i = 0; i < ele_num; i++) {
        switch (ele_data[i].id) {
        case SR_TH_ELE_ID_TEMP:
            __cht8305_calc_temp(temp, &ele_data[i].val);
            break;
        case SR_TH_ELE_ID_HUMI:
            __cht8305_calc_humi(humi, &ele_data[i].val);
            break;
        default:
            TAL_PR_ERR("[CHT8305] Element ID: %d is invalid.", ele_data[i].id);
            break;
        }
    }
    return OPRT_OK;
}

/**
 * @brief register cht8305 device
 *
 * @param[in] name: device name
 * @param[in] i2c_cfg: i2c configuration
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdd_sensor_cht8305_register(IN CHAR_T *name, IN SR_TH_I2C_CFG_T i2c_cfg)
{
    // check param
    if (i2c_cfg.gpio.scl == i2c_cfg.gpio.sda) {
        return OPRT_INVALID_PARM;
    }

    // I2C init
    tdd_sensor_i2c_init(i2c_cfg.port, i2c_cfg.gpio);

    // save resourse information
    SR_RSRC_T resource = {0};
    resource.port = i2c_cfg.port;
    resource.handle = NULL;
    resource.info[CHT8305_RSRC_INDEX_STAT] = CHT8305_CLOSE;
    resource.info[CHT8305_RSRC_INDEX_ADDR] = i2c_cfg.addr;

    // define driver interfaces
    STATIC SR_INTFS_T s_intfs;
    s_intfs.open = __tdd_sensor_cht8305_open;
    s_intfs.close = __tdd_sensor_cht8305_close;
    s_intfs.control = __tdd_sensor_cht8305_control;
    s_intfs.read_ele = __tdd_sensor_cht8305_read;

    // register the device
    OPERATE_RET op_ret = tdl_sensor_register(name, &s_intfs, SR_TH_ELE_NUM, sg_ele_val_tp, &resource);
    if (OPRT_OK != op_ret) {
        return op_ret;
    }

    return OPRT_OK;
}
