/**
 * @file tdd_sw_i2c.h
 * @author www.tuya.com
 * @version 0.1
 * @date 2022-08-26
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#ifndef __TDD_SW_I2C_H__
#define __TDD_SW_I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"

/***********************************************************
************************macro define************************
***********************************************************/
/**
 * @brief i2c flag
 */
#define I2C_FLAG_WR                             (1u << 0)   // write
#define I2C_FLAG_RD                             (1u << 1)   // read
#define I2C_FLAG_ADDR_10BIT                     (1u << 2)   // 10bits address
#define I2C_FLAG_NO_START                       (1u << 3)   // no start
#define I2C_FLAG_IGNORE_NACK                    (1u << 4)   // ignore NACK
#define I2C_FLAG_NO_READ_ACK                    (1u << 5)   // read without ACK
#define I2C_FLAG_NO_ADDR                        (1u << 6)   // massage wuthout address
                   
/***********************************************************
***********************typedef define***********************
***********************************************************/
/**
 * @brief i2c pin
 */
typedef struct {
    TUYA_GPIO_NUM_E scl;
    TUYA_GPIO_NUM_E sda;
} TDD_I2C_PIN_T;

/**
 * @brief i2c message struct
 *
 */
typedef struct {
    UINT_T      flags;
    USHORT_T    addr;
    USHORT_T    len;
    UCHAR_T    *buff;
} TDD_I2C_MSG_T;

/***********************************************************
********************function declaration********************
***********************************************************/
/**
 * @brief i2c init
 *
 * @param[in] i2c_pin: i2c pin number
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tdd_sw_i2c_init(TDD_I2C_PIN_T i2c_pin);

/**
 * @brief i2c deinit
 *
 * @param[in] i2c_pin: i2c pin number
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tdd_sw_i2c_deinit(TDD_I2C_PIN_T i2c_pin);

/**
 * @brief i2c transfer
 *
 * @param[in] i2c_pin: i2c pin number
 * @param[in] msg: msg send to i2c bus
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tdd_sw_i2c_xfer(TDD_I2C_PIN_T i2c_pin, TDD_I2C_MSG_T *msg);

#ifdef __cplusplus
}
#endif

#endif /* __TDD_SW_I2C_H__ */
