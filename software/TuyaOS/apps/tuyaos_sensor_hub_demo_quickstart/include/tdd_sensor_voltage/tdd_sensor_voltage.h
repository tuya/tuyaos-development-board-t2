/**
 * @file tdd_sensor_voltage.h
 * @brief tdd_sensor_voltage module is used to drive voltage sensor
 * @version 0.1
 * @date 2022-08-15
 */

#ifndef __TDD_SENSOR_VOLTAGE_H__
#define __TDD_SENSOR_VOLTAGE_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************macro define************************
***********************************************************/
/**
 * @brief element list
 */
#define SR_VOLT_ELE_ID_VOLT         0x00
#define SR_VOLT_ELE_NUM             0x01

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
********************function declaration********************
***********************************************************/
/**
 * @brief register voltage
 *
 * @param[in] name: device name
 * @param[in] adc_port: adc port number
 * @param[in] adc_cfg: adc base configuration
 * @param[in] scale: partial pressure coefficient
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdd_sensor_voltage_register(IN CHAR_T *name, IN TUYA_ADC_NUM_E adc_port,
                                        IN TUYA_ADC_BASE_CFG_T *adc_cfg, IN FLOAT_T scale);

#ifdef __cplusplus
}
#endif

#endif /* __TDD_SENSOR_VOLTAGE_H__ */
