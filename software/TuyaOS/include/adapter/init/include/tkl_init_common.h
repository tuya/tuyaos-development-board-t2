/**
* @file tkl_init_common.h
* @brief Common process
* @version 1.0
* @date 2021-08-06
*
* @copyright Copyright 2021-2030 Tuya Inc. All Rights Reserved.
*
*/
#ifndef __TKL_INIT_COMMON_H__
#define __TKL_INIT_COMMON_H__

#include "tuya_cloud_types.h"
#include "tkl_uart.h"
#include "tkl_rtc.h"
#include "tkl_watchdog.h"
#include "tkl_adc.h"
#include "tkl_gpio.h"
#include "tkl_flash.h"
#include "tkl_pwm.h"
#include "tkl_timer.h"
#include "tkl_i2c.h"
#include "tkl_spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief the description of tuya kernel adapter layer uart
 */
typedef struct {
    OPERATE_RET (*init)                     (TUYA_UART_NUM_E port_id, TUYA_UART_BASE_CFG_T *cfg);
    OPERATE_RET (*deinit)                   (TUYA_UART_NUM_E port_id);
    INT_T       (*write)                    (TUYA_UART_NUM_E port_id, VOID_T *buff, UINT16_T len);
    INT_T       (*read)                     (TUYA_UART_NUM_E port_id, VOID_T *buff, UINT16_T len);
    VOID_T      (*rx_irq_cb_reg)            (TUYA_UART_NUM_E port_id, TUYA_UART_IRQ_CB rx_cb);
    VOID_T      (*tx_irq_cb_reg)            (TUYA_UART_NUM_E port_id, TUYA_UART_IRQ_CB rx_cb);
    OPERATE_RET (*set_rx_flowctrl)          (TUYA_UART_NUM_E port_id, BOOL_T enable); 
} TKL_UART_DESC_T;

/**
 * @brief register uart description to tuya object manage
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
TKL_UART_DESC_T* tkl_uart_desc_get(VOID_T);


/**
 * @brief the description of tuya kernel adapter layer rtc
 */
typedef struct {
    OPERATE_RET     (*init)                 (VOID_T);
    OPERATE_RET     (*deinit)               (VOID_T);
    OPERATE_RET     (*time_get)             (TIME_T *time_sec);
    OPERATE_RET     (*time_set)             (TIME_T  time_sec);
} TKL_RTC_DESC_T;

/**
 * @brief register rtc description to tuya object manage
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
TKL_RTC_DESC_T* tkl_rtc_desc_get(VOID_T);


/**
 * @brief the description of tuya kernel adapter layer watchdog
 */
typedef struct {
    UINT_T          (*init)                 (TUYA_WDOG_BASE_CFG_T *cfg);
    OPERATE_RET     (*deinit)               (VOID_T);
    OPERATE_RET     (*refresh)              (VOID_T);
} TKL_WATCHDOG_DESC_T;

/**
 * @brief register watchdog description to tuya object manage
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
TKL_WATCHDOG_DESC_T* tkl_watchdog_desc_get(VOID_T);

/**
 * @brief the description of tuya kernel adapter layer gpio
 */
typedef struct {
    OPERATE_RET     (*init)                 (TUYA_GPIO_NUM_E pin_id, CONST TUYA_GPIO_BASE_CFG_T *cfg);
    OPERATE_RET     (*deinit)               (TUYA_GPIO_NUM_E pin_id);
    OPERATE_RET     (*write)                (TUYA_GPIO_NUM_E pin_id, TUYA_GPIO_LEVEL_E level);
    OPERATE_RET     (*read)                 (TUYA_GPIO_NUM_E pin_id, TUYA_GPIO_LEVEL_E *level);
    OPERATE_RET     (*irq_init)             (TUYA_GPIO_NUM_E pin_id, CONST TUYA_GPIO_IRQ_T *cfg);
    OPERATE_RET     (*irq_enable)           (TUYA_GPIO_NUM_E pin_id);
    OPERATE_RET     (*irq_disable)          (TUYA_GPIO_NUM_E pin_id);
} TKL_GPIO_DESC_T;

/**
 * @brief register gpio description to tuya object manage
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
TKL_GPIO_DESC_T* tkl_gpio_desc_get(VOID_T);


/**
 * @brief the description of tuya kernel adapter layer flash
 */
typedef struct {
    OPERATE_RET     (*read)                 (CONST UINT_T addr, UCHAR_T *dst, CONST UINT_T size);
    OPERATE_RET     (*write)                (CONST UINT_T addr, CONST UCHAR_T *src, CONST UINT_T size);
    OPERATE_RET     (*erase)                (CONST UINT_T addr, CONST UINT_T size);
    OPERATE_RET     (*lock)                 (CONST UINT_T addr, CONST UINT_T size);
    OPERATE_RET     (*unlock)               (CONST UINT_T addr, CONST UINT_T size);
    OPERATE_RET     (*get_one_type_info)    (TUYA_FLASH_TYPE_E type, TUYA_FLASH_BASE_INFO_T* info);
} TKL_FLASH_DESC_T;

/**
 * @brief register flash description to tuya object manage
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
TKL_FLASH_DESC_T* tkl_flash_desc_get(VOID_T);


/**
 * @brief the description of tuya kernel adapter layer pwm
 */
typedef struct {
    OPERATE_RET     (*init)                 (TUYA_PWM_NUM_E ch_id, CONST TUYA_PWM_BASE_CFG_T *cfg);
    OPERATE_RET     (*deinit)               (TUYA_PWM_NUM_E ch_id);
    OPERATE_RET     (*start)                (TUYA_PWM_NUM_E ch_id);
    OPERATE_RET     (*stop)                 (TUYA_PWM_NUM_E ch_id);
    OPERATE_RET     (*info_set)             (TUYA_PWM_NUM_E ch_id, CONST TUYA_PWM_BASE_CFG_T *info);
    OPERATE_RET     (*info_get)             (TUYA_PWM_NUM_E ch_id, TUYA_PWM_BASE_CFG_T *info);
} TKL_PWM_DESC_T;

/**
 * @brief register pwm description to tuya object manage
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
TKL_PWM_DESC_T* tkl_pwm_desc_get(VOID_T);


/**
 * @brief the description of tuya kernel adapter layer i2c
 */
typedef struct {
    OPERATE_RET     (*init)                 (TUYA_I2C_NUM_E port, CONST TUYA_IIC_BASE_CFG_T *cfg);
    OPERATE_RET     (*deinit)               (TUYA_I2C_NUM_E port);
    OPERATE_RET     (*irq_init)             (TUYA_I2C_NUM_E port, CONST TUYA_I2C_IRQ_CB *cb);
    OPERATE_RET     (*irq_enable)           (TUYA_I2C_NUM_E port);
    OPERATE_RET     (*irq_disable)          (TUYA_I2C_NUM_E port);
    OPERATE_RET     (*master_send)          (TUYA_I2C_NUM_E port, UINT16_T dev_addr, CONST VOID_T *data, UINT32_T size, BOOL_T xfer_pending);
    OPERATE_RET     (*master_receive)       (TUYA_I2C_NUM_E port, UINT16_T dev_addr, VOID *data, UINT32_T size, BOOL_T xfer_pending);
    OPERATE_RET     (*set_slave_addr)       (TUYA_I2C_NUM_E port, UINT16_T dev_addr);
    OPERATE_RET     (*slave_send)           (TUYA_I2C_NUM_E port, CONST VOID *data, UINT32_T size);
    OPERATE_RET     (*slave_receive)        (TUYA_I2C_NUM_E port, VOID *data, UINT32_T size);
    OPERATE_RET     (*get_status)           (TUYA_I2C_NUM_E port, TUYA_IIC_STATUS_T *status);
    OPERATE_RET     (*reset)                (TUYA_I2C_NUM_E port);
} TKL_I2C_DESC_T;

/**
 * @brief register i2c description to tuya object manage
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
TKL_I2C_DESC_T* tkl_i2c_desc_get(VOID_T);


/**
 * @brief the description of tuya kernel adapter layer adc
 */
 typedef struct {
    OPERATE_RET     (*init)                 (TUYA_ADC_NUM_E port_num, TUYA_ADC_BASE_CFG_T *cfg);
    OPERATE_RET     (*deinit)               (TUYA_ADC_NUM_E port_num);
    OPERATE_RET     (*read_single_channel)  (TUYA_ADC_NUM_E port_num, UINT8_T ch_num, INT32_T *buf);
    OPERATE_RET     (*read_data)            (TUYA_ADC_NUM_E port_num, INT32_T *buff, UINT16_T len);
    UINT8_T         (*width_get)            (TUYA_ADC_NUM_E port_num);
    UINT32_T        (*ref_voltage_get)      (TUYA_ADC_NUM_E port_num);
    INT32_T         (*temperature_get)      (VOID_T);
    INT32_T         (*read_voltage)         (TUYA_ADC_NUM_E port_num, INT32_T *buff, UINT16_T len);
} TKL_ADC_DESC_T;

/**
 * @brief register adc description to tuya object manage
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
TKL_ADC_DESC_T* tkl_adc_desc_get(VOID_T);

/**
 * @brief the description of tuya kernel adapter layer dac   xbf
 */
 typedef struct {
    OPERATE_RET     (*init)                 (TUYA_DAC_NUM_E port_num);
    OPERATE_RET     (*deinit)               (TUYA_DAC_NUM_E port_num);
    OPERATE_RET     (*controller_config)    (TUYA_DAC_NUM_E port_num, TUYA_DAC_CMD_E cmd, VOID * argu);
    OPERATE_RET     (*dac_base_cfg_get)     (TUYA_DAC_NUM_E port_num, TUYA_DAC_BASE_CFG_T *cfg);
    OPERATE_RET     (*start)                (TUYA_DAC_NUM_E port_num);
    OPERATE_RET     (*stop)                 (TUYA_DAC_NUM_E port_num);
    OPERATE_RET     (*fifo_reset)           (TUYA_DAC_NUM_E port_num);
} TKL_DAC_DESC_T;

/**
 * @brief register adc description to tuya object manage
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
TKL_DAC_DESC_T* tkl_dac_desc_get(VOID_T);

/**
 * @brief the description of tuya kernel adapter layer spi
 */
typedef struct {
    OPERATE_RET     (*init)                 (TUYA_SPI_NUM_E port, CONST TUYA_SPI_BASE_CFG_T *cfg);
    OPERATE_RET     (*deinit)               (TUYA_SPI_NUM_E port);
    OPERATE_RET     (*transfer)                 (TUYA_SPI_NUM_E port, VOID_T* send_buf, VOID_T* receive_buf, UINT32_T length);
    OPERATE_RET     (*abort_transfer)           (TUYA_SPI_NUM_E port);
    OPERATE_RET     (*send)                 (TUYA_SPI_NUM_E port, VOID_T *data, UINT16_T size);
    OPERATE_RET     (*recv)                 (TUYA_SPI_NUM_E port, VOID_T *data, UINT16_T size);
    OPERATE_RET     (*get_status)           (TUYA_SPI_NUM_E port, TUYA_SPI_STATUS_T *status);
    OPERATE_RET     (*irq_init)             (TUYA_SPI_NUM_E port, CONST TUYA_SPI_IRQ_CB *cb);
    OPERATE_RET     (*irq_enable)           (TUYA_SPI_NUM_E port);
    OPERATE_RET     (*irq_disable)          (TUYA_SPI_NUM_E port);
} TKL_SPI_DESC_T;

/**
 * @brief register spi description to tuya object manage
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
TKL_SPI_DESC_T* tkl_spi_desc_get(VOID_T);

/**
 * @brief the description of tuya kernel adapter layer timer
 */
typedef struct {
    OPERATE_RET     (*init)                 (TUYA_TIMER_NUM_E timer_id, TUYA_TIMER_BASE_CFG_E *cfg);
    OPERATE_RET     (*deinit)               (TUYA_TIMER_NUM_E timer_id);
    OPERATE_RET     (*start)                (TUYA_TIMER_NUM_E timer_id, UINT_T us);
    OPERATE_RET     (*stop)                 (TUYA_TIMER_NUM_E timer_id);
    OPERATE_RET     (*get)                  (TUYA_TIMER_NUM_E timer_id, UINT_T *us);
} TKL_TIMER_DESC_T;

/**
 * @brief register timer description to tuya object manage
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
TKL_TIMER_DESC_T* tkl_timer_desc_get(VOID_T);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __TKL_INIT_COMMON_H__



