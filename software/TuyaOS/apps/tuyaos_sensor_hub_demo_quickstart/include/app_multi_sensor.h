/**
 * @file app_multi_sensor.h
 * @brief app_multi_sensor module is used to demonstrate
 *        how to use multiple sensors
 * @version 0.1
 * @date 2022-09-01
 */

#ifndef __APP_MULTI_SENSOR_H__
#define __APP_MULTI_SENSOR_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************macro define************************
***********************************************************/
/**
 * @brief pin define
 */
#define SHT3X_SCL_PIN       GPIO_NUM_20
#define SHT3X_SDA_PIN       GPIO_NUM_22
#define SHT3X_ALT_PIN       GPIO_NUM_14
#define MPU60X0_SCL_PIN     GPIO_NUM_8
#define MPU60X0_SDA_PIN     GPIO_NUM_9
#define MPU60X0_INT_PIN     GPIO_NUM_24

/**
 * @brief mode define
 */
#define SHT3X_WORK_MODE     SR_MODE_POLL_SOFT_TM
#define MPU60X0_WORK_MODE   SR_MODE_EXTI
#define VOLTAGE_WORK_MODE   SR_MODE_POLL_HARD_TM

/**
 * @brief port define
 */
#define SHT3X_I2C_PORT      0
#define MPU60X0_I2C_PORT    1
#define VOLTAGE_ADC_CH      2

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
********************function declaration********************
***********************************************************/
/**
 * @brief multi-sensor demo init
 *
 * @param none
 *
 * @return none
 */
VOID_T app_multi_sensor_init(VOID_T);

#ifdef __cplusplus
}
#endif

#endif /* __APP_MULTI_SENSOR_H__ */
