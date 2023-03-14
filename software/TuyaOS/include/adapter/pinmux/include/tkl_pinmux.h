#ifndef __TKL_PINMUX_H__
#define __TKL_PINMUX_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"

#define TUYA_IO_GET_PORT_ID(data)       (((data) >> 8) & 0xFF)
#define TUYA_IO_GET_CHANNEL_ID(data)    (((data) & 0xFF)
/**
 * @brief tuya pinmux type,default use
 */
typedef enum {
    TUYA_IO_PIN_0 = 0,
    TUYA_IO_PIN_1,
    TUYA_IO_PIN_2,
    TUYA_IO_PIN_3,
    TUYA_IO_PIN_4,
    TUYA_IO_PIN_5,
    TUYA_IO_PIN_6,
    TUYA_IO_PIN_7,
    TUYA_IO_PIN_8,
    TUYA_IO_PIN_9,
    TUYA_IO_PIN_10,
    TUYA_IO_PIN_11,
    TUYA_IO_PIN_12,
    TUYA_IO_PIN_13,
    TUYA_IO_PIN_14,
    TUYA_IO_PIN_15,
    TUYA_IO_PIN_16,
    TUYA_IO_PIN_17,
    TUYA_IO_PIN_18,
    TUYA_IO_PIN_19,
    TUYA_IO_PIN_20,
    TUYA_IO_PIN_21,
    TUYA_IO_PIN_22,
    TUYA_IO_PIN_23,
    TUYA_IO_PIN_24,
    TUYA_IO_PIN_25,
    TUYA_IO_PIN_26,
    TUYA_IO_PIN_27,
    TUYA_IO_PIN_28,
    TUYA_IO_PIN_29,
    TUYA_IO_PIN_30,
    TUYA_IO_PIN_31,
    TUYA_IO_PIN_32,
    TUYA_IO_PIN_33,
    TUYA_IO_PIN_34,
    TUYA_IO_PIN_35,
    TUYA_IO_PIN_36,
    TUYA_IO_PIN_37,
    TUYA_IO_PIN_38,
    TUYA_IO_PIN_39,
    TUYA_IO_PIN_40,
    TUYA_IO_PIN_41,
    TUYA_IO_PIN_42,
    TUYA_IO_PIN_43,
    TUYA_IO_PIN_44,
    TUYA_IO_PIN_45,
    TUYA_IO_PIN_46,
    TUYA_IO_PIN_47,
    TUYA_IO_PIN_48,
    TUYA_IO_PIN_49,
    TUYA_IO_PIN_50,
    TUYA_IO_PIN_51,
    TUYA_IO_PIN_52,
    TUYA_IO_PIN_53,
    TUYA_IO_PIN_54,
    TUYA_IO_PIN_55,
    TUYA_IO_PIN_56,
    TUYA_IO_PIN_57,
    TUYA_IO_PIN_58,
    TUYA_IO_PIN_59,
    TUYA_IO_PIN_60,
    TUYA_IO_PIN_MAX,
} TUYA_PIN_NAME_E;

/**
 * @brief tuya pin map function, default use
 */
typedef enum {
    TUYA_IIC0_SCL = 0x0,
    TUYA_IIC0_SDA  ,
    TUYA_IIC1_SCL  ,
    TUYA_IIC1_SDA  ,
    TUYA_IIC2_SCL  ,
    TUYA_IIC2_SDA  ,

    TUYA_UART0_TX = 0x100,
    TUYA_UART0_RX  ,
    TUYA_UART0_RTS ,
    TUYA_UART0_CTS ,
    TUYA_UART1_TX  ,
    TUYA_UART1_RX  ,
    TUYA_UART1_RTS ,
    TUYA_UART1_CTS ,
    TUYA_UART2_TX  ,
    TUYA_UART2_RX  ,
    TUYA_UART2_RTS ,
    TUYA_UART2_CTS ,
    TUYA_UART3_TX  ,
    TUYA_UART3_RX  ,
    TUYA_UART3_RTS ,
    TUYA_UART3_CTS ,

    TUYA_SPI0_MISO = 0x200,
    TUYA_SPI0_MOSI ,
    TUYA_SPI0_CLK  ,
    TUYA_SPI0_CS   ,
    TUYA_SPI1_MISO ,
    TUYA_SPI1_MOSI ,
    TUYA_SPI1_CLK  ,
    TUYA_SPI1_CS   ,
    TUYA_SPI2_MISO ,
    TUYA_SPI2_MOSI ,
    TUYA_SPI2_CLK  ,
    TUYA_SPI2_CS   ,

    TUYA_PWM0 = 0x300,
    TUYA_PWM1,
    TUYA_PWM2,
    TUYA_PWM3,
    TUYA_PWM4,
    TUYA_PWM5,

    TUYA_ADC0 = 0x400,
    TUYA_ADC1,
    TUYA_ADC2,
    TUYA_ADC3,
    TUYA_ADC4,
    TUYA_ADC5,

    TUYA_DAC0 = 0x500,
    TUYA_DAC1,
    TUYA_DAC2,
    TUYA_DAC3,
    TUYA_DAC4,
    TUYA_DAC5,

    TUYA_I2S0_SCK = 0x600,// bit clk 0
    TUYA_I2S0_WS    ,// lrck
    TUYA_I2S0_SDO_0 ,
    TUYA_I2S0_SDI_0 ,
    TUYA_I2S1_SCK   ,// bit clk 1
    TUYA_I2S1_WS    ,// lrck
    TUYA_I2S1_SDO_0 ,
    TUYA_I2S1_SDI_0 ,

    TUYA_GPIO = 0x700,
    TUYA_PIN_FUNC_MAX = 0xFFFF,
} TUYA_PIN_FUNC_E;

/**
 * @brief tuya pin type
 */
typedef enum {
    TUYA_IO_TYPE_PWM,
    TUYA_IO_TYPE_ADC,
    TUYA_IO_TYPE_DAC,
    TUYA_IO_TYPE_UART,
    TUYA_IO_TYPE_SPI,
    TUYA_IO_TYPE_I2C,
    TUYA_IO_TYPE_I2S,
    TUYA_IO_TYPE_GPIO,
    TUYA_IO_TYPE_MAX = 0xFFFF,
}TUYA_PIN_TYPE_E;
/**
 * @brief tuya io pinmux func
 *
 * @param[in] pin: pin number
 * @param[in] pin_func: pin function
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_io_pinmux_config(TUYA_PIN_NAME_E pin, TUYA_PIN_FUNC_E pin_func);

/**
 * @brief tuya io pin to port,
 * @brief historical legacy, can not use in pin muxtiplex mode
 * @param[in] pin: pin number
 * @param[in] pin: pin type
 * @return Pin Function : Port and Channel,err < 0.
 * @return        16        8       8
 * @return[out] | rsv   |  port | channel |
 */
INT32_T tkl_io_pin_to_func(UINT32_T pin, TUYA_PIN_TYPE_E pin_type);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __TKL_REGISTER_H__

