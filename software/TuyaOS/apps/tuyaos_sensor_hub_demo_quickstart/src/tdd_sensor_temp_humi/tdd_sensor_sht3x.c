/**
 * @file tdd_sensor_sht3x.c
 * @brief tdd_sensor_sht3x module is used to drive sht3x chip
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
 * @brief CRC result
 */
#define CRC_OK                      (0)
#define CRC_ERR                     (-1)

/**
 * @brief work status
 */
#define SHT3X_CLOSE                 0
#define SHT3X_OPEN                  (!SHT3X_CLOSE)

/**
 * @brief resource array index
 */
#define SHT3X_RSRC_INDEX_STAT       0
#define SHT3X_RSRC_INDEX_ADDR       1
#define SHT3X_RSRC_INDEX_PREC       2
#define SHT3X_RSRC_INDEX_FREQ       3

/***********************************************************
***********************typedef define***********************
***********************************************************/
/**
 * @brief sht3x command
 */
typedef USHORT_T SHT3X_CMD_E;
#define SHT3X_CMD_SOFT_RESET        0x30A2  // soft reset
#define SHT3X_CMD_READ_SERIALNBR    0x3780  // read serial number
#define SHT3X_CMD_HEATER_ENABLE     0x306D  // enabled heater
#define SHT3X_CMD_HEATER_DISABLE    0x3066  // disable heater
#define SHT3X_CMD_READ_STATUS       0xF32D  // read status register
#define SHT3X_CMD_CLEAR_STATUS      0x3041  // clear status register
#define SHT3X_CMD_ART               0x2B32  // activate ART
#define SHT3X_CMD_BREAK             0x3093  // stop periodic data acquisition mode
#define SHT3X_CMD_FETCH_DATA        0xE000  // readout measurements for periodic mode
#define SHT3X_CMD_MEAS_PERI_05_H    0x2032  // measurement: periodic 0.5 mps, high repeatability
#define SHT3X_CMD_MEAS_PERI_05_M    0x2024  // measurement: periodic 0.5 mps, medium repeatability
#define SHT3X_CMD_MEAS_PERI_05_L    0x202F  // measurement: periodic 0.5 mps, low repeatability
#define SHT3X_CMD_MEAS_PERI_1_H     0x2130  // measurement: periodic 1 mps, high repeatability
#define SHT3X_CMD_MEAS_PERI_1_M     0x2126  // measurement: periodic 1 mps, medium repeatability
#define SHT3X_CMD_MEAS_PERI_1_L     0x212D  // measurement: periodic 1 mps, low repeatability
#define SHT3X_CMD_MEAS_PERI_2_H     0x2236  // measurement: periodic 2 mps, high repeatability
#define SHT3X_CMD_MEAS_PERI_2_M     0x2220  // measurement: periodic 2 mps, medium repeatability
#define SHT3X_CMD_MEAS_PERI_2_L     0x222B  // measurement: periodic 2 mps, low repeatability
#define SHT3X_CMD_MEAS_PERI_4_H     0x2334  // measurement: periodic 4 mps, high repeatability
#define SHT3X_CMD_MEAS_PERI_4_M     0x2322  // measurement: periodic 4 mps, medium repeatability
#define SHT3X_CMD_MEAS_PERI_4_L     0x2329  // measurement: periodic 4 mps, low repeatability
#define SHT3X_CMD_MEAS_PERI_10_H    0x2737  // measurement: periodic 10 mps, high repeatability
#define SHT3X_CMD_MEAS_PERI_10_M    0x2721  // measurement: periodic 10 mps, medium repeatability
#define SHT3X_CMD_MEAS_PERI_10_L    0x272A  // measurement: periodic 10 mps, low repeatability
#define SHT3X_CMD_MEAS_POLLING_H    0x2400  // measurement: polling, high repeatability
#define SHT3X_CMD_MEAS_POLLING_M    0x240B  // measurement: polling, medium repeatability
#define SHT3X_CMD_MEAS_POLLING_L    0x2416  // measurement: polling, low repeatability
#define SHT3X_CMD_MEAS_CLOCKSTR_H   0x2C06  // measurement: clock stretching, high repeatability
#define SHT3X_CMD_MEAS_CLOCKSTR_M   0x2C0D  // measurement: clock stretching, medium repeatability
#define SHT3X_CMD_MEAS_CLOCKSTR_L   0x2C10  // measurement: clock stretching, low repeatability
#define SHT3X_CMD_W_AL_LIM_HS       0x611D  // write alert limits, high set
#define SHT3X_CMD_W_AL_LIM_HC       0x6116  // write alert limits, high clear
#define SHT3X_CMD_W_AL_LIM_LC       0x610B  // write alert limits, low clear
#define SHT3X_CMD_W_AL_LIM_LS       0x6100  // write alert limits, low set
#define SHT3X_CMD_R_AL_LIM_LS       0xE102  // read alert limits, low set
#define SHT3X_CMD_R_AL_LIM_LC       0xE109  // read alert limits, low clear
#define SHT3X_CMD_R_AL_LIM_HS       0xE11F  // read alert limits, high set
#define SHT3X_CMD_R_AL_LIM_HC       0xE114  // read alert limits, high clear

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
STATIC VOID_T __sht3x_delay_ms(IN CONST ULONG_T tm)
{
    tal_system_sleep(tm);
}

/**
 * @brief get CRC8 value for sht3x
 *
 * @param[in] data: data to be calculated
 * @param[in] len: data length
 *
 * @return CRC8 value
 */
STATIC UCHAR_T __sht3x_get_crc8(IN CONST UCHAR_T *data, IN USHORT_T len)
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
STATIC INT_T __sht3x_check_crc8(IN CONST UCHAR_T *data, IN CONST USHORT_T len, IN CONST UCHAR_T crc_val)
{
    if (__sht3x_get_crc8(data, len) != crc_val) {
        return CRC_ERR;
    }
    return CRC_OK;
}

/**
 * @brief read data from sht3x
 *
 * @param[in] dev: device resource
 * @param[in] len: data length
 * @param[out] data: data received from sht3x
 *
 * @return none
 */
STATIC OPERATE_RET __sht3x_read_data(IN CONST SR_RSRC_T *dev, IN CONST USHORT_T len, OUT UCHAR_T *data)
{
    SR_I2C_MSG_T rd_msg = {
        .flags = SR_I2C_FLAG_RD,
        .addr = dev->info[SHT3X_RSRC_INDEX_ADDR],
        .len = len,
        .buff = data
    };
    
    return tdd_sensor_i2c_xfer(dev->port, &rd_msg);
}

/**
 * @brief write command to sht3x
 *
 * @param[in] dev: device resource
 * @param[in] cmd: control command
 *
 * @return none
 */
STATIC OPERATE_RET __sht3x_write_cmd(IN CONST SR_RSRC_T *dev, IN CONST SHT3X_CMD_E cmd)
{
    UCHAR_T cmd_bytes[2];
    cmd_bytes[0] = (UCHAR_T)(cmd >> 8);
    cmd_bytes[1] = (UCHAR_T)(cmd & 0x00FF);

    SR_I2C_MSG_T wr_msg = {
        .flags = SR_I2C_FLAG_WR,
        .addr = dev->info[SHT3X_RSRC_INDEX_ADDR],
        .len = 2,
        .buff = cmd_bytes
    };

    return tdd_sensor_i2c_xfer(dev->port, &wr_msg);
}

/**
 * @brief write command and data to sht3x
 *
 * @param[in] dev: device resource
 * @param[in] cmd: control command
 * @param[in] data: data to be written
 *
 * @return none
 */
STATIC VOID_T __sht3x_write_2bytes_data(IN CONST SR_RSRC_T *dev, IN CONST SHT3X_CMD_E cmd, IN CONST USHORT_T data)
{
    UCHAR_T buf[5];
    buf[0] = (UCHAR_T)(cmd >> 8);
    buf[1] = (UCHAR_T)(cmd & 0x00FF);
    buf[2] = (UCHAR_T)(data >> 8);
    buf[3] = (UCHAR_T)(data & 0x00FF);
    buf[4] = __sht3x_get_crc8(buf+2, 2);

    SR_I2C_MSG_T wr_msg = {
        .flags = SR_I2C_FLAG_WR,
        .addr = dev->info[SHT3X_RSRC_INDEX_ADDR],
        .len = 5,
        .buff = buf
    };

    tdd_sensor_i2c_xfer(dev->port, &wr_msg);
}

/**
 * @brief start periodic measurement
 *
 * @param[in] dev: device resource
 *
 * @return none
 */
STATIC VOID_T __sht3x_start_periodic_measurement(IN CONST SR_RSRC_T *dev)
{
    switch (dev->info[SHT3X_RSRC_INDEX_PREC]) {
    default:
    case SR_TH_PREC_HIGH:
        switch (dev->info[SHT3X_RSRC_INDEX_FREQ]) {
        case SR_TH_FREQ_HZ5:
            __sht3x_write_cmd(dev, SHT3X_CMD_MEAS_PERI_05_H);
            break;
        default:
        case SR_TH_FREQ_1HZ:
            __sht3x_write_cmd(dev, SHT3X_CMD_MEAS_PERI_1_H);
            break;
        case SR_TH_FREQ_2HZ:
            __sht3x_write_cmd(dev, SHT3X_CMD_MEAS_PERI_2_H);
            break;
        case SR_TH_FREQ_4HZ:
            __sht3x_write_cmd(dev, SHT3X_CMD_MEAS_PERI_4_H);
            break;
        case SR_TH_FREQ_10HZ:
            __sht3x_write_cmd(dev, SHT3X_CMD_MEAS_PERI_10_H);
            break;
        }
        break;

    case SR_TH_PREC_MEDIUM:
        switch (dev->info[SHT3X_RSRC_INDEX_FREQ]) {
        case SR_TH_FREQ_HZ5:
            __sht3x_write_cmd(dev, SHT3X_CMD_MEAS_PERI_05_M);
            break;
        default:
        case SR_TH_FREQ_1HZ:
            __sht3x_write_cmd(dev, SHT3X_CMD_MEAS_PERI_1_M);
            break;
        case SR_TH_FREQ_2HZ:
            __sht3x_write_cmd(dev, SHT3X_CMD_MEAS_PERI_2_M);
            break;
        case SR_TH_FREQ_4HZ:
            __sht3x_write_cmd(dev, SHT3X_CMD_MEAS_PERI_4_M);
            break;
        case SR_TH_FREQ_10HZ:
            __sht3x_write_cmd(dev, SHT3X_CMD_MEAS_PERI_10_M);
            break;
        }
        break;

    case SR_TH_PREC_LOW:
        switch (dev->info[SHT3X_RSRC_INDEX_FREQ]) {
        case SR_TH_FREQ_HZ5:
            __sht3x_write_cmd(dev, SHT3X_CMD_MEAS_PERI_05_L);
            break;

        default:
        case SR_TH_FREQ_1HZ:
            __sht3x_write_cmd(dev, SHT3X_CMD_MEAS_PERI_1_L);
            break;

        case SR_TH_FREQ_2HZ:
            __sht3x_write_cmd(dev, SHT3X_CMD_MEAS_PERI_2_L);
            break;

        case SR_TH_FREQ_4HZ:
            __sht3x_write_cmd(dev, SHT3X_CMD_MEAS_PERI_4_L);
            break;

        case SR_TH_FREQ_10HZ:
            __sht3x_write_cmd(dev, SHT3X_CMD_MEAS_PERI_10_L);
            break;
        }
        break;
    }
}

/**
 * @brief stop periodic measurement
 *
 * @param[in] dev: device resource
 *
 * @return none
 */
STATIC VOID_T __sht3x_stop_periodic_measurement(IN CONST SR_RSRC_T *dev)
{
    __sht3x_write_cmd(dev, SHT3X_CMD_BREAK);
}

/**
 * @brief set precision
 *
 * @param[inout] dev: device resource
 * @param[in] prec: precision level
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __sht3x_set_precision(INOUT SR_RSRC_T *dev, IN CONST SR_TH_PREC_E *prec)
{
    if ((prec == NULL) || (*prec >= SR_TH_PREC_INVALID)) {
        return OPRT_INVALID_PARM;
    }

    dev->info[SHT3X_RSRC_INDEX_PREC] = *prec;
    __sht3x_start_periodic_measurement(dev);

    return OPRT_OK;
}

/**
 * @brief set frequency
 *
 * @param[inout] dev: device resource
 * @param[in] freq: frequency
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __sht3x_set_frequency(INOUT SR_RSRC_T *dev, IN CONST SR_TH_PREC_E *freq)
{
    if ((freq == NULL) || (*freq >= SR_TH_FREQ_INVALID)) {
        return OPRT_INVALID_PARM;
    }

    dev->info[SHT3X_RSRC_INDEX_PREC] = *freq;
    __sht3x_start_periodic_measurement(dev);

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
STATIC VOID_T __sht3x_calc_temp(IN CONST USHORT_T temp, OUT SR_VAL_U *temp_val)
{
#if (SR_CONFIG_NO_FPU == 1)
    temp_val->sr_int = temp * 175 / 65535 - 45;
#else
    temp_val->sr_float = temp * 175.0f / 65535.0f - 45.0f;
#endif
}

/**
 * @brief calculate raw temperature
 *
 * @param[in] temp_val: temperature value in °C
 *
 * @return temperature raw data to sensor
 */
STATIC USHORT_T __sht3x_calc_temp_raw(IN CONST SR_VAL_U temp_val)
{
#if (SR_CONFIG_NO_FPU == 1)
    return ((temp_val.sr_int + 45) * 65535 / 175)
#else
    return ((temp_val.sr_float + 45.0f) * 65535.0f / 175.0f);
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
STATIC VOID_T __sht3x_calc_humi(IN CONST USHORT_T humi, OUT SR_VAL_U *humi_val)
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
STATIC USHORT_T __sht3x_calc_humi_raw(IN CONST SR_VAL_U humi_val)
{
#if (SR_CONFIG_NO_FPU == 1)
    return (humi_val.sr_int * 65535 / 100)
#else
    return (humi_val.sr_float * 65535.0f / 100.0f);
#endif
}

/**
 * @brief read temperature and humidity from sht3x
 *
 * @param[in] dev: device resource
 * @param[out] temp: temperature value
 * @param[out] humi: humidity value
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __sht3x_read_temp_humi(IN CONST SR_RSRC_T *dev, OUT USHORT_T *temp, OUT USHORT_T *humi)
{
    UCHAR_T buf[6] = {0};
    OPERATE_RET ret = OPRT_OK;

    __sht3x_write_cmd(dev, SHT3X_CMD_FETCH_DATA);
    ret = __sht3x_read_data(dev, 6, buf);
    if(ret != OPRT_OK)
        return ret;

    if ((CRC_ERR == __sht3x_check_crc8(buf, 2, buf[2])) ||\
        (CRC_ERR == __sht3x_check_crc8(buf+3, 2, buf[5]))) {
        TAL_PR_ERR("[SHT3x] The received temp_humi data can't pass the CRC8 check.");
        return OPRT_CRC32_FAILED;
    }

    *temp = ((USHORT_T)buf[0] << 8) | buf[1];
    *humi = ((USHORT_T)buf[3] << 8) | buf[4];

    return OPRT_OK;
}

/**
 * @brief read limit
 *
 * @param[in] dev: device resource
 * @param[in] cmd: alert limit command
 * @param[out] temp_limit: temperature limit value
 * @param[out] humi_limit: humidity limit value
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET __sht3x_read_limit(IN CONST SR_RSRC_T *dev, IN CONST SHT3X_CMD_E cmd, OUT SR_VAL_U *temp_limit, OUT SR_VAL_U *humi_limit)
{
    UCHAR_T buf[3] = {0};
    OPERATE_RET ret = OPRT_OK;

    __sht3x_write_cmd(dev, cmd);
    ret = __sht3x_read_data(dev, 3, buf);
    if(ret != OPRT_OK)
        return ret;

    if (CRC_ERR == __sht3x_check_crc8(buf, 2, buf[2])) {
        TAL_PR_ERR("[SHT3x] The received limit can't pass the CRC8 check.");
        return OPRT_CRC32_FAILED;
    }

    USHORT_T limit = ((USHORT_T)buf[0] << 8) | buf[1];
    __sht3x_calc_temp((limit << 7), temp_limit);
    __sht3x_calc_humi((limit & 0xFE00), humi_limit);

    return OPRT_OK;
}

/**
 * @brief write limit
 *
 * @param[in] dev: device resource
 * @param[in] cmd: alert limit command
 * @param[in] temp_limit: temperature limit value
 * @param[in] humi_limit: humidity limit value
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET __sht3x_write_limit(IN CONST SR_RSRC_T *dev, IN CONST SHT3X_CMD_E cmd,
                                IN CONST SR_VAL_U temp_limit, IN CONST SR_VAL_U humi_limit)
{
#if (SR_CONFIG_NO_FPU == 1)
    if (humi_limit.sr_int < 0 || humi_limit.sr_int > 100 ||\
        temp_limit.sr_int < -40 || temp_limit.sr_int > 125) {
        return OPRT_INVALID_PARM;
    }
#else
    if (humi_limit.sr_float < 0.0f || humi_limit.sr_float > 100.0f ||\
        temp_limit.sr_float < -40.0f || temp_limit.sr_float > 125.0f) {
        return OPRT_INVALID_PARM;
    }
#endif

    USHORT_T limit_raw = ((__sht3x_calc_humi_raw(humi_limit) & 0xFE00) |\
                          ((__sht3x_calc_temp_raw(temp_limit) >> 7) & 0x001FF));

    __sht3x_write_2bytes_data(dev, cmd, limit_raw);

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
STATIC OPERATE_RET __sht3x_get_alert_limit(IN CONST SR_RSRC_T *dev, OUT SR_TH_LIMIT_T *limit)
{
    OPERATE_RET op_ret;

    op_ret = __sht3x_read_limit(dev, SHT3X_CMD_R_AL_LIM_HS, &limit->temp_high_set, &limit->humi_high_set);
    if (op_ret) {
        TAL_PR_ERR("[SHT3x] Read high_set limit failed.");
        return op_ret;
    }
    op_ret = __sht3x_read_limit(dev, SHT3X_CMD_R_AL_LIM_HC, &limit->temp_high_clear, &limit->humi_high_clear);
    if (op_ret) {
        TAL_PR_ERR("[SHT3x] Read high_clear limit failed.");
        return op_ret;
    }
    op_ret = __sht3x_read_limit(dev, SHT3X_CMD_R_AL_LIM_LS, &limit->temp_low_set, &limit->humi_low_set);
    if (op_ret) {
        TAL_PR_ERR("[SHT3x] Read low_set limit failed.");
        return op_ret;
    }
    op_ret = __sht3x_read_limit(dev, SHT3X_CMD_R_AL_LIM_LC, &limit->temp_low_clear, &limit->humi_low_clear);
    if (op_ret) {
        TAL_PR_ERR("[SHT3x] Read low_clear limit failed.");
        return op_ret;
    }
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
STATIC OPERATE_RET __sht3x_set_alert_limit(IN CONST SR_RSRC_T *dev, IN CONST SR_TH_LIMIT_T *limit)
{
    OPERATE_RET op_ret;
    op_ret = __sht3x_write_limit(dev, SHT3X_CMD_W_AL_LIM_HS, limit->temp_high_set, limit->humi_high_set);
    if (op_ret) {
        TAL_PR_ERR("[SHT3x] The high_set limit value is invalid.");
        return op_ret;
    }

    op_ret = __sht3x_write_limit(dev, SHT3X_CMD_W_AL_LIM_HC, limit->temp_high_clear, limit->humi_high_clear);
    if (op_ret) {
        TAL_PR_ERR("[SHT3x] The high_clear limit value is invalid.");
        return op_ret;
    }

    op_ret = __sht3x_write_limit(dev, SHT3X_CMD_W_AL_LIM_LS, limit->temp_low_set, limit->humi_low_set);
    if (op_ret) {
        TAL_PR_ERR("[SHT3x] The low_set limit value is invalid.");
        return op_ret;
    }

    op_ret = __sht3x_write_limit(dev, SHT3X_CMD_W_AL_LIM_LC, limit->temp_low_clear, limit->humi_low_clear);
    if (op_ret) {
        TAL_PR_ERR("[SHT3x] The low_clear limit value is invalid.");
        return op_ret;
    }
    return OPRT_OK;
}

/**
 * @brief enable alert function
 *
 * @param[in] dev: device resource
 * @param[in] alert_pin: alert pin configuration
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __sht3x_enable_alert(IN CONST SR_RSRC_T *dev, IN SR_TH_ALT_CFG_T *alert)
{
    OPERATE_RET op_ret;

    op_ret = tkl_gpio_irq_init(alert->pin, &alert->irq);
    if (op_ret != OPRT_OK) {
        TAL_PR_ERR("[SHT3x] P%d interrupt for alert init failed, error code: %d", alert->pin, op_ret);
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
STATIC OPERATE_RET __sht3x_disable_alert(IN CONST SR_RSRC_T *dev, IN CONST TUYA_GPIO_NUM_E *alert_pin)
{
    OPERATE_RET op_ret;

    op_ret = tkl_gpio_irq_disable(*alert_pin);
    if (op_ret != OPRT_OK) {
        TAL_PR_ERR("[SHT3x] P%d interrupt for alert disable failed, error code: %d", *alert_pin, op_ret);
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
STATIC OPERATE_RET __sht3x_read_status(IN CONST SR_RSRC_T *dev, OUT SR_TH_STATUS_U *status)
{
    UCHAR_T buf[3];
    OPERATE_RET ret = OPRT_OK;

    __sht3x_write_cmd(dev, SHT3X_CMD_READ_STATUS);
    ret = __sht3x_read_data(dev, 3, buf);
    if(ret != OPRT_OK)
        return ret;

    if (CRC_ERR == __sht3x_check_crc8(buf, 2, buf[2])) {
        TAL_PR_ERR("[SHT3x] The received status can't pass the CRC8 check.");
        return OPRT_COM_ERROR;
    }

    status->word = ((USHORT_T)buf[0] << 8) | buf[1];

    return OPRT_OK;
}

/**
 * @brief clear status
 *
 * @param[in] dev: device resource
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __sht3x_clear_status(IN CONST SR_RSRC_T *dev)
{
    __sht3x_write_cmd(dev, SHT3X_CMD_CLEAR_STATUS);
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
STATIC OPERATE_RET __sht3x_set_heater(IN CONST SR_RSRC_T *dev, IN CONST BOOL_T enabled)
{
    if (enabled) {
        __sht3x_write_cmd(dev, SHT3X_CMD_HEATER_ENABLE);
    } else {
        __sht3x_write_cmd(dev, SHT3X_CMD_HEATER_DISABLE);
    }
    return OPRT_OK;
}

/**
 * @brief soft reset
 *
 * @param[in] dev: device resource
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __sht3x_soft_reset(IN CONST SR_RSRC_T *dev)
{
    __sht3x_write_cmd(dev, SHT3X_CMD_SOFT_RESET);
    __sht3x_delay_ms(50);
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
STATIC OPERATE_RET __sht3x_read_serial_number(IN CONST SR_RSRC_T *dev, OUT SR_TH_SERIAL_NBR_T *serial_nbr)
{
    UCHAR_T buf[6] = {0};
    OPERATE_RET ret = OPRT_OK;

    __sht3x_write_cmd(dev, SHT3X_CMD_READ_SERIALNBR);
    __sht3x_delay_ms(20);
    ret = __sht3x_read_data(dev, 6, buf);
    if(ret != OPRT_OK)
        return ret;

    if ((CRC_ERR == __sht3x_check_crc8(buf, 2, buf[2])) ||\
        (CRC_ERR == __sht3x_check_crc8(buf+3, 2, buf[5]))) {
        TAL_PR_ERR("[SHT3x] The received serial number can't pass the CRC8 check.");
        return OPRT_CRC32_FAILED;
    }

    serial_nbr->val = ((UINT_T)buf[0] << 24) | ((UINT_T)buf[1] << 16) |\
                      ((UINT_T)buf[3] <<  8) | ((UINT_T)buf[4]);
    return OPRT_OK;
}

/**
 * @brief open sht3x device
 *
 * @param[inout] dev: device resource
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdd_sensor_sht3x_open(INOUT SR_RSRC_T *dev)
{
    __sht3x_start_periodic_measurement(dev);
    dev->info[SHT3X_RSRC_INDEX_STAT] = SHT3X_OPEN;
    return OPRT_OK;
}

/**
 * @brief close sht3x device
 *
 * @param[inout] dev: device resource
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdd_sensor_sht3x_close(INOUT SR_RSRC_T *dev)
{
    __sht3x_stop_periodic_measurement(dev);
    dev->info[SHT3X_RSRC_INDEX_STAT] = SHT3X_CLOSE;
    return OPRT_OK;
}

/**
 * @brief control sht3x device
 *
 * @param[inout] dev: device resource
 * @param[in] cmd: control command
 * @param[inout] param: command parameter, data type depends on the specified command
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdd_sensor_sht3x_control(INOUT SR_RSRC_T* dev, IN UCHAR_T cmd, INOUT VOID_T *param)
{
    OPERATE_RET op_ret = OPRT_OK;

    if ((SHT3X_OPEN == dev->info[SHT3X_RSRC_INDEX_STAT]) &&\
        (cmd == SR_TH_CMD_SET_PREC      ||\
         cmd == SR_TH_CMD_SET_FREQ      ||\
         cmd == SR_TH_CMD_SOFT_RESET    ||\
         cmd == SR_TH_CMD_GET_SERIAL_NBR)) {
        TAL_PR_ERR("[SHT3x] Command <%d> is only supported when acquisition is closed.");
        return OPRT_COM_ERROR;
    }

    switch (cmd) {
    case SR_TH_CMD_SOFT_RESET:
        op_ret = __sht3x_soft_reset(dev);
        break;

    case SR_TH_CMD_GET_SERIAL_NBR:
        op_ret = __sht3x_read_serial_number(dev, (SR_TH_SERIAL_NBR_T *)param);
        break;
        
    case SR_TH_CMD_SET_PREC:
        op_ret = __sht3x_set_precision(dev, (SR_TH_PREC_E *)param);
        break;

    case SR_TH_CMD_SET_FREQ:
        op_ret = __sht3x_set_frequency(dev, (SR_TH_FREQ_E *)param);
        break;

    case SR_TH_CMD_SET_ALT_LIMIT:
        op_ret = __sht3x_set_alert_limit(dev, (SR_TH_LIMIT_T *)param);
        break;

    case SR_TH_CMD_GET_ALT_LIMIT:
        op_ret = __sht3x_get_alert_limit(dev, (SR_TH_LIMIT_T *)param);
        break;

    case SR_TH_CMD_ENABLE_ALT_PIN:
        op_ret = __sht3x_enable_alert(dev, (SR_TH_ALT_CFG_T *)param);
        break;

    case SR_TH_CMD_DISABLE_ALT_PIN:
        op_ret = __sht3x_disable_alert(dev, (TUYA_GPIO_NUM_E *)param);
        break;

    case SR_TH_CMD_GET_STATUS:
        op_ret = __sht3x_read_status(dev, (SR_TH_STATUS_U *)param);
        break;

    case SR_TH_CMD_CLR_STATUS:
        op_ret = __sht3x_clear_status(dev);
        break;

    case SR_TH_CMD_ENABLE_HEATER:
        op_ret = __sht3x_set_heater(dev, TRUE);
        break;

    case SR_TH_CMD_DISABLE_HEATER:
        op_ret = __sht3x_set_heater(dev, FALSE);
        break;

    default:
        op_ret = OPRT_INVALID_PARM;
        TAL_PR_ERR("[SHT3x] Command <%d> is invalid.", cmd);
        break;
    }

    return op_ret;
}

/**
 * @brief read data from sht3x device
 *
 * @param[in] dev: device
 * @param[out] ele_data: element data
 * @param[in] ele_num: number of elements
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdd_sensor_sht3x_read(IN SR_RSRC_T* dev, OUT SR_ELE_DATA_T *ele_data, IN UCHAR_T ele_num)
{
    USHORT_T temp = 0;
    USHORT_T humi = 0;

    OPERATE_RET op_ret = __sht3x_read_temp_humi(dev, &temp, &humi);
    if (op_ret != OPRT_OK) {
        return op_ret;
    }

    for (UCHAR_T i = 0; i < ele_num; i++) {
        switch (ele_data[i].id) {
        case SR_TH_ELE_ID_TEMP:
            __sht3x_calc_temp(temp, &ele_data[i].val);
            break;

        case SR_TH_ELE_ID_HUMI:
            __sht3x_calc_humi(humi, &ele_data[i].val);
            break;
            
        default:
            TAL_PR_ERR("[SHT3x] Element ID: %d is invalid.", ele_data[i].id);
            break;
        }
    }
    return OPRT_OK;
}

/**
 * @brief register sht3x device
 *
 * @param[in] name: device name
 * @param[in] i2c_cfg: i2c configuration
 * @param[in] meas_cfg: measurement configuration
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdd_sensor_sht3x_register(IN CHAR_T *name, IN SR_TH_I2C_CFG_T i2c_cfg, IN SR_TH_MEAS_CFG_T meas_cfg)
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
    resource.info[SHT3X_RSRC_INDEX_STAT] = SHT3X_CLOSE;
    resource.info[SHT3X_RSRC_INDEX_ADDR] = i2c_cfg.addr;
    resource.info[SHT3X_RSRC_INDEX_PREC] = meas_cfg.prec;
    resource.info[SHT3X_RSRC_INDEX_FREQ] = meas_cfg.freq;

    // define driver interfaces
    STATIC SR_INTFS_T s_intfs;
    s_intfs.open = __tdd_sensor_sht3x_open;
    s_intfs.close = __tdd_sensor_sht3x_close;
    s_intfs.control = __tdd_sensor_sht3x_control;
    s_intfs.read_ele = __tdd_sensor_sht3x_read;

    // register the device
    OPERATE_RET op_ret = tdl_sensor_register(name, &s_intfs, SR_TH_ELE_NUM, sg_ele_val_tp, &resource);
    if (OPRT_OK != op_ret) {
        return op_ret;
    }

    return OPRT_OK;
}
