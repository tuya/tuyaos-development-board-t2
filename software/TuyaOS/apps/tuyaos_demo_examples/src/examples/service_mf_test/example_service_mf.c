/**
 * @file example_service_mf.c
 * @author www.tuya.com
 * @brief example_service_mf module is used to 
 * @version 0.1
 * @date 2022-09-20
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include "tuya_cloud_types.h"
#include "tuya_uart.h"
#include "mf_test.h"

#include "tal_log.h"

/***********************************************************
************************macro define************************
***********************************************************/
#define MF_UART_ID  UART_NUM_0

#ifndef APP_BIN_NAME
#define APP_BIN_NAME "tuyaos_examples"
#endif

#ifndef USER_SW_VER
#define USER_SW_VER "1.0.0"
#endif

/***********************************************************
***********************typedef define***********************
***********************************************************/


/***********************************************************
********************function declaration********************
***********************************************************/


/***********************************************************
***********************variable define**********************
***********************************************************/


/***********************************************************
***********************function define**********************
***********************************************************/

/**
 * @brief callback for URAT init function
 *
 * @param[in] baud data bits:8bit,parity:none,stop bits:1
 * @param[in] bufsz Size of buff
 *
 */
STATIC VOID __mf_uart_init_callback(UINT_T baud, UINT_T bufsz)
{
    TAL_UART_CFG_T cfg;
    memset(&cfg, 0, sizeof(TAL_UART_CFG_T));
    cfg.base_cfg.baudrate = baud;
    cfg.base_cfg.databits = TUYA_UART_DATA_LEN_8BIT;
    cfg.base_cfg.parity = TUYA_UART_PARITY_TYPE_NONE;
    cfg.base_cfg.stopbits = TUYA_UART_STOP_LEN_1BIT;
    cfg.rx_buffer_size = bufsz;

    tal_uart_init(MF_UART_ID, &cfg);

    return;
}

/**
 * @brief callback for URAT send function
 *
 * @param[in] data Send data buf
 * @param[in] len Send data buf len
 *
 */
STATIC VOID __mf_uart_send_callback(IN BYTE_T *data, IN CONST UINT_T len)
{
    tal_uart_write(MF_UART_ID, data, len);

    return;
}

/**
 * @brief callback for URAT receive function
 *
 * @param[in] len buf len
 * @param[out] buf Received data buf
 *
 * @return read data len
 */
STATIC UINT_T __mf_uart_recv_callback(OUT BYTE_T *buf, IN CONST UINT_T len)
{
    return tal_uart_read(MF_UART_ID, buf, len);
}

/**
 * @brief callback for UART free function
 *
 * @param[in] none: 
 *
 * @return none
 */
STATIC VOID __mf_uart_free_callback(VOID)
{
    tal_uart_deinit(MF_UART_ID);

    return;
}

/**
 * @brief callback for user product test
 *
 * @param[in] cmd Test command
 * @param[in] data Test data
 * @param[in] len Test data len
 * @param[out] ret_data Test return data
 * @param[out] ret_len Test return data len
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
STATIC OPERATE_RET __mf_user_product_test_callback(USHORT_T cmd, UCHAR_T *data, UINT_T len, OUT UCHAR_T **ret_data, OUT USHORT_T *ret_len)
{
    return OPRT_OK;
}

/**
 * @brief callback for user
 *
 * @param[in] none: 
 *
 * @return none
 */
STATIC VOID __mf_user_callback(VOID)
{
    return;
}

/**
 * @brief user enter mf callback
 *
 * @param[in] none: 
 *
 * @return none
 */
STATIC VOID __mf_user_enter_mf_callback(VOID)
{
    return;
}

BOOL_T __mf_pre_gpio_test_cb(VOID)
{
    return TRUE;
}

BOOL_T __mf_gpio_test_cb(IN CONST CHAR_T *in, OUT CHAR_T *out)
{
    return TRUE;
}

/**
 * @brief Enable the production test function
 *
 * @param[in] none: 
 *
 * @return none
 */
VOID example_mf_init(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;

    MF_IMPORT_INTF_S _mf_intf = {
        .uart_init = __mf_uart_init_callback,
        .uart_send = __mf_uart_send_callback,
        .uart_recv = __mf_uart_recv_callback,
        .uart_free = __mf_uart_free_callback,
        .mf_user_product_test = __mf_user_product_test_callback,
        .user_callback = __mf_user_callback,
        .user_enter_mf_callback = __mf_user_enter_mf_callback,
        .gpio_test = __mf_gpio_test_cb,
        .user_pre_gpio_test = __mf_pre_gpio_test_cb,
    };

    TUYA_CALL_ERR_LOG(mf_init(&_mf_intf, APP_BIN_NAME, USER_SW_VER, TRUE));
    TAL_PR_NOTICE("mf already initialized");

    return;
}
