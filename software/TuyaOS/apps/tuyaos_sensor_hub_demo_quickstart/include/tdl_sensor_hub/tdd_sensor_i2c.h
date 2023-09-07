/**
 * @file tdd_sensor_i2c.h
 * @brief Common process - adapter the i2c api
 * @version 0.1
 * @date 2022-08-03
 */

#ifndef __TDD_SENSOR_I2C_H__
#define __TDD_SENSOR_I2C_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************macro define************************
***********************************************************/
/**
 * @brief i2c flag
 */
#define SR_I2C_FLAG_WR          (1u << 0)   // write
#define SR_I2C_FLAG_RD          (1u << 1)   // read
#define SR_I2C_FLAG_ADDR_10BIT  (1u << 2)   // 10bits address
#define SR_I2C_FLAG_NO_START    (1u << 3)   // no start
#define SR_I2C_FLAG_IGNORE_NACK (1u << 4)   // ignore NACK
#define SR_I2C_FLAG_NO_READ_ACK (1u << 5)   // read without ACK
#define SR_I2C_FLAG_NO_ADDR     (1u << 6)   // massage wuthout address

/***********************************************************
***********************typedef define***********************
***********************************************************/
/**
 * @brief i2c pin
 */
typedef struct {
    TUYA_GPIO_NUM_E scl;
    TUYA_GPIO_NUM_E sda;
} SR_I2C_GPIO_T;

/**
 * @brief i2c message struct
 *
 */
typedef struct {
    UINT_T      flags;
    USHORT_T    addr;
    USHORT_T    len;
    UCHAR_T    *buff;
} SR_I2C_MSG_T;

/***********************************************************
********************function declaration********************
***********************************************************/
/**
 * @brief i2c init
 *
 * @param[in] port: i2c port
 * @param[in] i2c_pin: i2c pin number
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdd_sensor_i2c_init(UCHAR_T port, SR_I2C_GPIO_T i2c_pin);

/**
 * @brief i2c deinit
 *
 * @param[in] i2c_pin: i2c pin number
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdd_sensor_i2c_deinit(UCHAR_T port);

/**
 * @brief i2c transfer
 *
 * @param[in] i2c_pin: i2c pin number
 * @param[in] msg: msg send to i2c bus
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdd_sensor_i2c_xfer(UCHAR_T port, SR_I2C_MSG_T *msg);

#ifdef __cplusplus
}
#endif

#endif /* __TDD_SENSOR_I2C_H__ */
