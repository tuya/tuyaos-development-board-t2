/**
 * @file tdd_sensor_sht4x.c
 * @brief tdd_sensor_sht4x module is used to drive sht4x chip
 * @version 0.1
 * @date 2022-08-03
 */

#include "tdl_sensor_driver.h"
#include "tdd_sensor_temp_humi.h"
#include "tal_system.h"
#include "tal_log.h"

/***********************************************************
************************macro define************************
***********************************************************/
/**
 * @brief CRC result
 */
#define CRC_OK                      (0)
#define CRC_ERR                     (-1)

/**
 * @brief work status
 */
#define SHT4X_CLOSE                 0
#define SHT4X_OPEN                  (!SHT4X_CLOSE)

/**
 * @brief resource array index
 */
#define SHT4X_RSRC_INDEX_STAT       0
#define SHT4X_RSRC_INDEX_ADDR       1
#define SHT4X_RSRC_INDEX_PREC       2

/***********************************************************
***********************typedef define***********************
***********************************************************/
/**
 * @brief sht4x command
 */
typedef BYTE_T SHT4X_CMD_E;
#define SHT4X_CMD_MEAS_PREC_H       0xFD    // measurement with high precision
#define SHT4X_CMD_MEAS_PREC_M       0xF6    // measurement with medium precision
#define SHT4X_CMD_MEAS_PREC_L       0xE0    // measurement with lowest precision
#define SHT4X_CMD_READ_SERIALNBR    0x89    // read serial number
#define SHT4X_CMD_SOFT_RESET        0x94    // soft reset
#define SHT4X_CMD_HEATER_200MW_1S   0x39    // activate heater with 200mW for 1s
#define SHT4X_CMD_HEATER_200MW_01S  0x32    // activate heater with 200mW for 0.1s
#define SHT4X_CMD_HEATER_110MW_1S   0x2F    // activate heater with 110mW for 1s
#define SHT4X_CMD_HEATER_110MW_01S  0x24    // activate heater with 110mW for 0.1s
#define SHT4X_CMD_HEATER_20MW_1S    0x1E    // activate heater with 20mW for 1s
#define SHT4X_CMD_HEATER_20MW_01S   0x15    // activate heater with 20mW for 0.1s

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
STATIC VOID_T __sht4x_delay_ms(IN CONST ULONG_T tm)
{
    tal_system_sleep(tm);
}

/**
 * @brief get CRC8 value for sht4x
 *
 * @param[in] data: data to be calculated
 * @param[in] len: data length
 *
 * @return CRC8 value
 */
STATIC UCHAR_T __sht4x_get_crc8(IN CONST UCHAR_T *data, IN USHORT_T len)
{
    UCHAR_T i;
    UCHAR_T crc = 0xFF;

    while (len--) {
        crc ^= *data;
        for (i = 8; i > 0; --i) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x31;
            } else {
                crc = (crc << 1);
            }
        }
        data++;
    }
    return crc;
}

/**
 * @brief check CRC8
 *
 * @param[in] data: data to be checked
 * @param[in] len: data length
 * @param[in] crc_val: crc value
 *
 * @return check result
 */
STATIC INT_T __sht4x_check_crc8(IN CONST UCHAR_T *data, IN CONST USHORT_T len, IN CONST UCHAR_T crc_val)
{
    if (__sht4x_get_crc8(data, len) != crc_val){
        return CRC_ERR;
    }
    return CRC_OK;
}

/**
 * @brief read data from sht4x
 *
 * @param[in] dev: device resource
 * @param[in] len: data length
 * @param[out] data: data received from sht4x
 *
 * @return none
 */
STATIC VOID_T __sht4x_read_data(IN CONST SR_RSRC_T *dev, IN CONST USHORT_T len, OUT UCHAR_T *data)
{
    SR_I2C_MSG_T rd_msg = {
        .flags = SR_I2C_FLAG_RD,
        .addr = dev->info[SHT4X_RSRC_INDEX_ADDR],
        .len = len,
        .buff = data
    };
    tdd_sensor_i2c_xfer(dev->port, &rd_msg);
}

/**
 * @brief write command to sht4x
 *
 * @param[in] dev: device resource
 * @param[in] cmd: control command
 *
 * @return none
 */
STATIC VOID_T __sht4x_write_cmd(IN CONST SR_RSRC_T *dev, IN SHT4X_CMD_E cmd)
{
    SR_I2C_MSG_T wr_msg = {
        .flags = SR_I2C_FLAG_WR,
        .addr = dev->info[SHT4X_RSRC_INDEX_ADDR],
        .len = 1,
        .buff = &cmd
    };
    tdd_sensor_i2c_xfer(dev->port, &wr_msg);
}

/**
 * @brief start single measurement
 *
 * @param[in] dev: device resource
 *
 * @return none
 */
STATIC VOID_T __sht4x_start_single_measurement(IN CONST SR_RSRC_T *dev)
{
    switch (dev->info[SHT4X_RSRC_INDEX_PREC]) {
    default:
    case SR_TH_PREC_HIGH:
        __sht4x_write_cmd(dev, SHT4X_CMD_MEAS_PREC_H);
        break;
    case SR_TH_PREC_MEDIUM:
        __sht4x_write_cmd(dev, SHT4X_CMD_MEAS_PREC_M);
        break;
    case SR_TH_PREC_LOW:
        __sht4x_write_cmd(dev, SHT4X_CMD_MEAS_PREC_L);
        break;
    }
}

/**
 * @brief set precision
 *
 * @param[inout] dev: device resource
 * @param[in] prec: precision level
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __sht4x_set_precision(INOUT SR_RSRC_T *dev, IN CONST SR_TH_PREC_E *prec)
{
    if ((prec == NULL) || (*prec >= SR_TH_PREC_INVALID)) {
        return OPRT_INVALID_PARM;
    }

    dev->info[SHT4X_RSRC_INDEX_PREC] = *prec;

    return OPRT_OK;
}

/**
 * @brief calculate temperature
 *
 * @param[in] temp: temperature raw data from sensor
 * @param[out] temp_val: temperature value in °C
 *
 * @return none
 */
STATIC VOID_T __sht4x_calc_temp(IN CONST USHORT_T temp, OUT SR_VAL_U *temp_val)
{
#if (SR_CONFIG_NO_FPU == 1)
    temp_val->sr_int = temp * 175 / 65535 - 45;
#else
    temp_val->sr_float = temp * 175.0f / 65535.0f - 45.0f;
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
STATIC VOID_T __sht4x_calc_humi(IN CONST USHORT_T humi, OUT SR_VAL_U *humi_val)
{
#if (SR_CONFIG_NO_FPU == 1)
    humi_val->sr_int = humi * 125 / 65535 - 6;
#else
    humi_val->sr_float = humi * 125.0f / 65535.0f - 6.0f;
#endif
}

/**
 * @brief read temperature and humidity from sht4x
 *
 * @param[in] dev: device resource
 * @param[out] temp: temperature value
 * @param[out] humi: humidity value
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __sht4x_read_temp_humi(IN CONST SR_RSRC_T *dev, OUT USHORT_T *temp, OUT USHORT_T *humi)
{
    UCHAR_T buf[6] = {0};
    __sht4x_read_data(dev, 6, buf);

    if ((CRC_ERR == __sht4x_check_crc8(buf, 2, buf[2])) ||\
        (CRC_ERR == __sht4x_check_crc8(buf+3, 2, buf[5]))) {
        TAL_PR_ERR("[SHT4x] The received temp_humi data can't pass the CRC8 check.");
        return OPRT_CRC32_FAILED;
    }

    *temp = ((USHORT_T)buf[0] << 8) | buf[1];
    *humi = ((USHORT_T)buf[3] << 8) | buf[4];

    return OPRT_OK;
}

/**
 * @brief activate heater for a while
 *
 * @param[in] dev: device resource
 * @param[in] heater: heater configuration
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __sht4x_activate_heater(IN CONST SR_RSRC_T *dev, IN CONST SR_TH_HEATER_T *heater)
{
    if ((heater == NULL) ||\
        (heater->power >= SR_TH_HEATER_PWR_INVALID) ||\
        (heater->time >= SR_TH_HEATER_TIM_INVALID)) {
        return OPRT_INVALID_PARM;
    }

    SHT4X_CMD_E cmd;
    switch (heater->power) {
        case SR_TH_HEATER_PWR_HIGH:
            cmd = heater->time == SR_TH_HEATER_TIM_LONG ? SHT4X_CMD_HEATER_200MW_1S : SHT4X_CMD_HEATER_200MW_01S;
            break;
        case SR_TH_HEATER_PWR_MEDIUM:
            cmd = heater->time == SR_TH_HEATER_TIM_LONG ? SHT4X_CMD_HEATER_110MW_1S : SHT4X_CMD_HEATER_110MW_01S;
            break;
        case SR_TH_HEATER_PWR_LOW:
            cmd = heater->time == SR_TH_HEATER_TIM_LONG ? SHT4X_CMD_HEATER_20MW_1S : SHT4X_CMD_HEATER_20MW_01S;
            break;
        default:
            break;
    }
    __sht4x_write_cmd(dev, cmd);

    return OPRT_OK;
}

/**
 * @brief soft reset
 *
 * @param[in] dev: device resource
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __sht4x_soft_reset(IN CONST SR_RSRC_T *dev)
{
    __sht4x_write_cmd(dev, SHT4X_CMD_SOFT_RESET);
    __sht4x_delay_ms(50);
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
STATIC OPERATE_RET __sht4x_read_serial_number(IN CONST SR_RSRC_T *dev, OUT SR_TH_SERIAL_NBR_T *serial_nbr)
{
    UCHAR_T buf[6] = {0};

    __sht4x_write_cmd(dev, SHT4X_CMD_READ_SERIALNBR);
    __sht4x_delay_ms(20);
    __sht4x_read_data(dev, 6, buf);

    if ((CRC_ERR == __sht4x_check_crc8(buf, 2, buf[2])) ||\
        (CRC_ERR == __sht4x_check_crc8(buf+3, 2, buf[5]))) {
        TAL_PR_ERR("[SHT4x] The received serial number can't pass the CRC8 check.");
        return OPRT_CRC32_FAILED;
    }

    serial_nbr->val = ((UINT_T)buf[0] << 24) | ((UINT_T)buf[1] << 16) |\
                      ((UINT_T)buf[3] <<  8) | ((UINT_T)buf[4]);
    return OPRT_OK;
}

/**
 * @brief open sht4x device
 *
 * @param[inout] dev: device resource
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdd_sensor_sht4x_open(INOUT SR_RSRC_T *dev)
{
    dev->info[SHT4X_RSRC_INDEX_STAT] = SHT4X_OPEN;
    return OPRT_OK;
}

/**
 * @brief close sht4x device
 *
 * @param[inout] dev: device resource
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdd_sensor_sht4x_close(INOUT SR_RSRC_T *dev)
{
    dev->info[SHT4X_RSRC_INDEX_STAT] = SHT4X_CLOSE;
    return OPRT_OK;
}

/**
 * @brief control sht4x device
 *
 * @param[inout] dev: device resource
 * @param[in] cmd: control command
 * @param[inout] param: command parameter, data type depends on the specified command
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdd_sensor_sht4x_control(INOUT SR_RSRC_T* dev, IN UCHAR_T cmd, INOUT VOID_T *param)
{
    OPERATE_RET op_ret = OPRT_OK;

    if ((SHT4X_OPEN == dev->info[SHT4X_RSRC_INDEX_STAT]) &&\
        (cmd == SR_TH_CMD_SOFT_RESET ||\
         cmd == SR_TH_CMD_GET_SERIAL_NBR)) {
        TAL_PR_ERR("[SHT4x] Command <%d> is only supported when acquisition is closed.");
        return OPRT_COM_ERROR;
    }

    switch (cmd) {
    case SR_TH_CMD_SOFT_RESET:
        op_ret = __sht4x_soft_reset(dev);
        break;
    case SR_TH_CMD_GET_SERIAL_NBR:
        op_ret = __sht4x_read_serial_number(dev, (SR_TH_SERIAL_NBR_T *)param);
        break;
    case SR_TH_CMD_SET_PREC:
        op_ret = __sht4x_set_precision(dev, (SR_TH_PREC_E *)param);
        break;
    case SR_TH_CMD_ACTIVATE_HEATER:
        op_ret = __sht4x_activate_heater(dev, (SR_TH_HEATER_T *)param);
        break;
    default:
        op_ret = OPRT_INVALID_PARM;
        TAL_PR_ERR("[SHT4x] Command <%d> is invalid.", cmd);
        break;
    }
    return op_ret;
}

/**
 * @brief read data from sht4x device
 *
 * @param[in] dev: device
 * @param[inout] ele_data: element data
 * @param[in] ele_num: number of elements
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdd_sensor_sht4x_read(IN SR_RSRC_T* dev, INOUT SR_ELE_DATA_T *ele_data, IN UCHAR_T ele_num)
{
    USHORT_T temp = 0;
    USHORT_T humi = 0;

    __sht4x_start_single_measurement(dev);
    __sht4x_delay_ms(20);

    OPERATE_RET op_ret = __sht4x_read_temp_humi(dev, &temp, &humi);
    if (op_ret != OPRT_OK) {
        return op_ret;
    }

    for (UCHAR_T i = 0; i < ele_num; i++) {
        switch (ele_data[i].id) {
        case SR_TH_ELE_ID_TEMP:
            __sht4x_calc_temp(temp, &ele_data[i].val);
            break;
        case SR_TH_ELE_ID_HUMI:
            __sht4x_calc_humi(humi, &ele_data[i].val);
            break;
        default:
            TAL_PR_ERR("[SHT4x] Element ID: %d is invalid.", ele_data[i].id);
            break;
        }
    }
    return OPRT_OK;
}

/**
 * @brief register sht4x device
 *
 * @param[in] name: device name
 * @param[in] i2c_cfg: i2c configuration
 * @param[in] prec: measurement precision
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdd_sensor_sht4x_register(IN CHAR_T *name, IN SR_TH_I2C_CFG_T i2c_cfg, IN SR_TH_PREC_E prec)
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
    resource.info[SHT4X_RSRC_INDEX_STAT] = SHT4X_CLOSE;
    resource.info[SHT4X_RSRC_INDEX_ADDR] = i2c_cfg.addr;
    resource.info[SHT4X_RSRC_INDEX_PREC] = prec;

    // define driver interfaces
    STATIC SR_INTFS_T s_intfs;
    s_intfs.open = __tdd_sensor_sht4x_open;
    s_intfs.close = __tdd_sensor_sht4x_close;
    s_intfs.control = __tdd_sensor_sht4x_control;
    s_intfs.read_ele = __tdd_sensor_sht4x_read;

    // register the device
    OPERATE_RET op_ret = tdl_sensor_register(name, &s_intfs, SR_TH_ELE_NUM, sg_ele_val_tp, &resource);
    if (OPRT_OK != op_ret) {
        return op_ret;
    }

    return OPRT_OK;
}
