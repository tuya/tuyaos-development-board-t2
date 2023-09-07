/**
 * @file tdd_sensor_temp_humi.h
 * @brief tdd_sensor_temp_humi module is used to drive temperature & humidity sensor
 * @version 0.1
 * @date 2022-08-03
 */

#ifndef __TDD_SENSOR_TEMP_HUMI_H__
#define __TDD_SENSOR_TEMP_HUMI_H__

#include "tdl_sensor_type.h"
#include "tdd_sensor_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************macro define************************
***********************************************************/
/**
 * @brief element list
 */
#define SR_TH_ELE_ID_TEMP           0x00
#define SR_TH_ELE_ID_HUMI           0x01
#define SR_TH_ELE_NUM               0x02

/**
 * @brief command list
 */
#define SR_TH_CMD_SOFT_RESET        0x00    // soft reset, param: none
#define SR_TH_CMD_GET_SERIAL_NBR    0x01    // read serial number, param: SR_TH_CMD_READ_SERIAL_NBR
#define SR_TH_CMD_SET_PREC          0x02    // set measurement precision, param: SR_TH_PREC_E
#define SR_TH_CMD_SET_FREQ          0x03    // set measurement frequency, param: SR_TH_FREQ_E
#define SR_TH_CMD_SET_ALT_LIMIT     0x04    // set alert thresholds, param: SR_TH_LIMIT_T
#define SR_TH_CMD_GET_ALT_LIMIT     0x05    // get alert thresholds, param: SR_TH_LIMIT_T
#define SR_TH_CMD_ENABLE_ALT_PIN    0x06    // enable alert pin, param: SR_TH_ALT_PIN_T
#define SR_TH_CMD_DISABLE_ALT_PIN   0x07    // disable alert pin, param: TKL_GPIO_NAME_E
#define SR_TH_CMD_GET_STATUS        0x08    // read status register, param: SR_TH_STATUS_U
#define SR_TH_CMD_CLR_STATUS        0x09    // clear status register, param: none
#define SR_TH_CMD_ENABLE_HEATER     0x0A    // enable heater, param: none
#define SR_TH_CMD_DISABLE_HEATER    0x0B    // disable heater, param: none
#define SR_TH_CMD_ACTIVATE_HEATER   0x0C    // activate heater for a while, param: SR_TH_HEATER_T

/**
 * @brief I2C address list
 */
#define SR_I2C_ADDR_SHT3X_A         0x44    // SHT3x : ADDR pin - GND
#define SR_I2C_ADDR_SHT3X_B         0x45    // SHT3x : ADDR pin - VCC
#define SR_I2C_ADDR_SHT4X_A         0x44    // SHT4x-A */
#define SR_I2C_ADDR_SHT4X_B         0x45    // SHT4x-B */
#define SR_I2C_ADDR_CHT8305_A       0x40    // CHT8305 : AD0 pin - GND
#define SR_I2C_ADDR_CHT8305_B       0x44    // CHT8305 : AD0 pin - VCC
#define SR_I2C_ADDR_CHT8305_C       0x48    // CHT8305 : AD0 pin - SDA
#define SR_I2C_ADDR_CHT8305_D       0x4C    // CHT8305 : AD0 pin - SCL

/***********************************************************
***********************typedef define***********************
***********************************************************/
/**
 * @brief measurement precision
 *        enum value for SR_TH_CMD_SET_PREC
 */
typedef BYTE_T SR_TH_PREC_E;
#define SR_TH_PREC_HIGH             0x00
#define SR_TH_PREC_MEDIUM           0x01
#define SR_TH_PREC_LOW              0x02
#define SR_TH_PREC_INVALID          0x03

/**
 * @brief measurement frequency
 *        enum value for SR_TH_CMD_SET_FREQ
 */
typedef BYTE_T SR_TH_FREQ_E;
#define SR_TH_FREQ_HZ5              0x00
#define SR_TH_FREQ_1HZ              0x01
#define SR_TH_FREQ_2HZ              0x02
#define SR_TH_FREQ_4HZ              0x03
#define SR_TH_FREQ_10HZ             0x04
#define SR_TH_FREQ_INVALID          0x05

/**
 * @brief heater work power
 *        enum value for SR_TH_CMD_ACTIVATE_HEATER
 */
typedef BYTE_T SR_TH_HEATER_PWR_E;
#define SR_TH_HEATER_PWR_HIGH       0x00
#define SR_TH_HEATER_PWR_MEDIUM     0x01
#define SR_TH_HEATER_PWR_LOW        0x02
#define SR_TH_HEATER_PWR_INVALID    0x03

/**
 * @brief heater work time
 *        enum value for SR_TH_CMD_ACTIVATE_HEATER
 */
typedef BYTE_T SR_TH_HEATER_TIM_E;
#define SR_TH_HEATER_TIM_LONG       0x00
#define SR_TH_HEATER_TIM_SHORT      0x01
#define SR_TH_HEATER_TIM_INVALID    0x02

/**
 * @brief I2C configuration
 */
typedef struct {
    UCHAR_T         port;
    UCHAR_T         addr;
    SR_I2C_GPIO_T   gpio;
} SR_TH_I2C_CFG_T;

/**
 * @brief measurement configuration
 *        param type for tdd_sensor_sht3x_register
 */
typedef struct {
    SR_TH_PREC_E prec;
    SR_TH_FREQ_E freq;
} SR_TH_MEAS_CFG_T;

/**
 * @brief alert limit
 *        param type for SR_TH_CMD_SET_ALT_LIMIT
 *                   and SR_TH_CMD_GET_ALT_LIMIT
 *        temp_range: -40 ~ 125 (°C)
 *        humi_range:   0 ~ 100 (%RH)
 */
typedef struct {
    SR_VAL_U temp_high_set;
    SR_VAL_U temp_high_clear;
    SR_VAL_U temp_low_set;
    SR_VAL_U temp_low_clear;
    SR_VAL_U humi_high_set;
    SR_VAL_U humi_high_clear;
    SR_VAL_U humi_low_set;
    SR_VAL_U humi_low_clear;
} SR_TH_LIMIT_T;

/**
 * @brief alert pin configuration
 *        param type for SR_TH_CMD_ENABLE_ALT_PIN
 */
typedef struct {
    TUYA_GPIO_NUM_E pin;
    TUYA_GPIO_IRQ_T irq;
} SR_TH_ALT_CFG_T;

/**
 * @brief heater configuration
 *        param type for SR_TH_CMD_ACTIVATE_HEATER
 */
typedef struct {
    SR_TH_HEATER_PWR_E power;
    SR_TH_HEATER_TIM_E time;
} SR_TH_HEATER_T;

/**
 * @brief status register value
 *        param type for SR_TH_CMD_GET_STATUS
 */
typedef union {
    // little-endian
    struct {
        USHORT_T crc_status     : 1;
        USHORT_T cmd_status     : 1;
        USHORT_T reserve0       : 2;
        USHORT_T reset_detected : 1;
        USHORT_T reserve1       : 5;
        USHORT_T temp_alert     : 1;
        USHORT_T humi_alert     : 1;
        USHORT_T reserve2       : 1;
        USHORT_T heater_status  : 1;
        USHORT_T reserve3       : 1;
        USHORT_T alert_pending  : 1;
    } bit;
    // can use "word" directly
    USHORT_T word;
} SR_TH_STATUS_U;

/**
 * @brief serial number
 *        param type for SR_TH_CMD_READ_SERIAL_NBR
 */
typedef struct {
    UINT_T val;
} SR_TH_SERIAL_NBR_T;

/***********************************************************
********************function declaration********************
***********************************************************/
/**
 * @brief register sht3x device
 *
 * @param[in] name: device name
 * @param[in] i2c_cfg: i2c configuration
 * @param[in] meas_cfg: measurement configuration
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdd_sensor_sht3x_register(IN CHAR_T *name, IN SR_TH_I2C_CFG_T i2c_cfg, IN SR_TH_MEAS_CFG_T meas_cfg);

/**
 * @brief register sht4x device
 *
 * @param[in] name: device name
 * @param[in] i2c_cfg: i2c configuration
 * @param[in] prec: measurement precision
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdd_sensor_sht4x_register(IN CHAR_T *name, IN SR_TH_I2C_CFG_T i2c_cfg, IN SR_TH_PREC_E prec);

/**
 * @brief register cht8305 device
 *
 * @param[in] name: device name
 * @param[in] i2c_cfg: i2c configuration
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdd_sensor_cht8305_register(IN CHAR_T *name, IN SR_TH_I2C_CFG_T i2c_cfg);

#ifdef __cplusplus
}
#endif

#endif /* __TDD_SENSOR_TEMP_HUMI_H__ */
