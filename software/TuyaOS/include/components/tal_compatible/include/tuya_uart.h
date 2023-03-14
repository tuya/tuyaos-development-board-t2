
#ifndef __TUYA_UART_H__
#define __TUYA_UART_H__

#ifdef __cplusplus
    extern "C" {
#endif

#include "tal_uart.h"
#include "tuya_uart_legacy.h"

#define tuya_uart_t             VOID
#define tuya_uart_cfg_t         TAL_UART_CFG_T

typedef enum {
    TUYA_UART0                    = 0x00,
    TUYA_UART1                    = 0x01,
    TUYA_UART_BAUDRATE_9600       = 9600,
    TUYA_UART_BAUDRATE_115200     = 115200,
    TUYA_UART_DATA_BIT8           = 0x08,
    TUYA_UART_STOP_BIT1           = 0x01,
    TUYA_UART_PARITY_NONE         = 0,  
    TUYA_DRV_UART                 = 0x02, 
    TUYA_DRV_CONFIG_CMD           = 0x06,
} tuya_uart_params_model_t;

#define TUYA_UART_8N1_CFG_INIT(__CFG, __BAUDRATE, __BUFSZ, __FLAG)          \
        memset((__CFG), 0, sizeof(TAL_UART_CFG_T));                         \
        (__CFG)->base_cfg.baudrate       = __BAUDRATE;                               \
        (__CFG)->base_cfg.databits       = TUYA_UART_DATA_BIT8;                      \
        (__CFG)->base_cfg.stopbits       = TUYA_UART_STOP_BIT1;                      \
        (__CFG)->base_cfg.parity         = TUYA_UART_PARITY_NONE;                    \
        (__CFG)->rx_buffer_size = __BUFSZ;                                  \


#define TUYA_UART_8N1_CFG(__UART, __BAUDRATE, __BUFSZ, __FLAG)              \
        TAL_UART_CFG_T cfg;                                                 \
        memset(&cfg, 0, sizeof(TAL_UART_CFG_T));                            \
        cfg.base_cfg.baudrate            = __BAUDRATE;                               \
        cfg.base_cfg.databits            = TUYA_UART_DATA_BIT8;                       \
        cfg.base_cfg.stopbits            = TUYA_UART_STOP_BIT1;                       \
        cfg.base_cfg.parity              = TUYA_UART_PARITY_NONE;                     \
        cfg.rx_buffer_size      = __BUFSZ


#define tuya_driver_find(__type, __port)                        (__port + 1)         
#define tuya_uart_init(__uart)                                  tal_uart_init(((UINT_T)__uart - 1), &cfg)
#define tuya_uart_deinit(__uart)                                tal_uart_deinit(((UINT_T)__uart - 1))
#define tuya_uart_read(__uart, __data, __len)                   tal_uart_read(((UINT_T)__uart - 1), (UINT8_T *)__data, (UINT_T)__len)
#define tuya_uart_write(__uart, __data, __len)                  tal_uart_write(((UINT_T)__uart - 1), (UINT8_T *)__data, (UINT_T)__len)


STATIC INLINE int tuya_uart_control (tuya_uart_t *uart, UINT8_T cmd, VOID *arg)
{
    int result = OPRT_COM_ERROR;

    if (cmd == TUYA_DRV_CONFIG_CMD) {
        tal_uart_deinit((TUYA_UART_NUM_E)uart - 1);
        result = tal_uart_init((TUYA_UART_NUM_E)uart - 1, arg);
        if (OPRT_OK != result) {
            PR_ERR("uart cfg fail", result);
        }
    }

    return result;
}


#ifdef __cplusplus
}
#endif
#endif
