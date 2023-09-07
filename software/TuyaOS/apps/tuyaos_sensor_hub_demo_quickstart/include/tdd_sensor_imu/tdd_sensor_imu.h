/**
 * @file tdd_sensor_imu.h
 * @brief tdd_sensor_imu module is used to drive imu sensor
 * @version 0.1
 * @date 2022-08-03
 */

#ifndef __TDD_SENSOR_IMU_H__
#define __TDD_SENSOR_IMU_H__

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
#define SR_IMU_ELE_ID_GYRO_X        0x00
#define SR_IMU_ELE_ID_GYRO_Y        0x01
#define SR_IMU_ELE_ID_GYRO_Z        0x02
#define SR_IMU_ELE_ID_ACCEL_X       0x03
#define SR_IMU_ELE_ID_ACCEL_Y       0x04
#define SR_IMU_ELE_ID_ACCEL_Z       0x05
#define SR_IMU_ELE_ID_TEMP          0x06
#define SR_IMU_ELE_NUM              0x07

/**
 * @brief command list
 */
#define SR_IMU_CMD_SOFT_RST         0x00    // soft reset, param: none
#define SR_IMU_CMD_GET_DEV_ID       0x01    // get device ID, param: SR_IMU_DEV_ID_T
#define SR_IMU_CMD_INIT_DEV         0x02    // init device, param: SR_IMU_BASE_CFG_T
#define SR_IMU_CMD_SET_INT          0x03    // set INT pin, param: SR_IMU_INT_CFG_T

/**
 * @brief I2C address list
 */
#define SR_I2C_ADDR_MPU60X0_A       0x68    // MPU60X0 : AD0 pin - GND
#define SR_I2C_ADDR_MPU60X0_B       0x69    // MPU60X0 : AD0 pin - VCC

/***********************************************************
***********************typedef define***********************
***********************************************************/
/**
 * @brief gyro full-scale range
 */
typedef BYTE_T SR_IMU_GYRO_FSR_E;
#define SR_IMU_GYRO_FSR_250         0x00    // 250dps
#define SR_IMU_GYRO_FSR_500         0x01    // 500dps
#define SR_IMU_GYRO_FSR_1000        0x02    // 1000dps
#define SR_IMU_GYRO_FSR_2000        0x03    // 2000dps

/**
 * @brief accel full-scale range
 */
typedef BYTE_T SR_IMU_ACCEL_FSR_E;
#define SR_IMU_ACCEL_FSR_2          0x00    // 2g
#define SR_IMU_ACCEL_FSR_4          0x01    // 4g
#define SR_IMU_ACCEL_FSR_8          0x02    // 8g
#define SR_IMU_ACCEL_FSR_16         0x03    // 16g

/**
 * @brief clock source
 */
typedef BYTE_T SR_IMU_CLK_SRC_E;
#define SR_IMU_CLK_INTERNAL         0x00
#define SR_IMU_CLK_PLL_XGYRO        0x01
#define SR_IMU_CLK_PLL_YGYRO        0x02
#define SR_IMU_CLK_PLL_ZGYRO        0x03
#define SR_IMU_CLK_PLL_EXT32K       0x04
#define SR_IMU_CLK_PLL_EXT19M       0x05

/**
 * @brief I2C configuration
 */
typedef struct {
    UCHAR_T         port;
    UCHAR_T         addr;
    SR_I2C_GPIO_T   gpio;
} SR_IMU_I2C_CFG_T;

/**
 * @brief param type for SR_IMU_CMD_GET_DEV_ID
 */
typedef struct {
    UCHAR_T val;
} SR_IMU_DEV_ID_T;

/**
 * @brief param type for SR_IMU_CMD_INIT_DEV
 */
typedef struct {
    SR_IMU_CLK_SRC_E    clk_src;
    SR_IMU_GYRO_FSR_E   gyro_fsr;
    SR_IMU_ACCEL_FSR_E  accel_fsr;
    USHORT_T            freq_hz;
} SR_IMU_BASE_CFG_T;

/**
 * @brief param type for SR_IMU_CMD_SET_INT
 */
typedef struct {
    BOOL_T active_low;
} SR_IMU_INT_CFG_T;

/***********************************************************
********************function declaration********************
***********************************************************/
/**
 * @brief register mpu60x0
 *
 * @param[in] name: device name
 * @param[in] i2c_cfg: i2c configuration
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdd_sensor_mpu60x0_register(IN CHAR_T *name, IN SR_IMU_I2C_CFG_T i2c_cfg);

#ifdef __cplusplus
}
#endif

#endif /* __TDD_SENSOR_IMU_H__ */
