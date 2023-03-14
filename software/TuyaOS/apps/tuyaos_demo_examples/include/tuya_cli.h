/**
* @file tuya_cli.h
* @brief Common process - cli
* @version 0.1
* @date 2020-05-13
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/

#ifndef __TUYA_CLI__
#define __TUYA_CLI__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief callback for one cli command
 *
 * @param[in] argc The number of argumnet
 * @param[in] argv The point date of argument
 *
 */
typedef VOID (*CLI_CMD_CB)(INT_T argc, CHAR_T *argv[]);

typedef struct {
    /** cli command name */
    CHAR_T                *name;
    /** cli command help */
    CHAR_T                *help;
    /** the callback of one command */
    CLI_CMD_CB    func;
} CLI_CMD_T;

/**
 * @brief cli init function,default uart0
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 *
 */
INT_T tuya_cli_init(VOID_T);

/**
 * @brief cli command register
 *
 * @param[in] cmd Info of one command
 * @param[in] num Number
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 *
 */
INT_T tuya_cli_cmd_register(CONST CLI_CMD_T *cmd, UINT8_T num);

/**
 * @brief uart cli init
 *
 * @param[in] uart_num The number of UART
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 *
 */
INT_T tuya_cli_init_with_uart(UINT8_T uart_num);

/**
 * @brief tuya_cli_printf
 *
 * @param[in] format ARGUMENT(s) according to FORMAT
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 *
 */
VOID tuya_cli_printf(CONST CHAR_T *format, ...);

#ifdef __cplusplus
}
#endif
#endif

