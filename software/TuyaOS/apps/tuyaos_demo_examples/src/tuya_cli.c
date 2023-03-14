/*============================================================================
*                                                                            *
* Copyright (C) by Tuya Inc                                                  *
* All rights reserved                                                        *
*                                                                            *
* @author  :   Linch                                                         *
* @date    :   2020-05-13                                                    *
* @brief   :                                                                 *
* @log     :                                                                 *
=============================================================================*/

/*============================ INCLUDES ======================================*/
#include <string.h>
#include <stdio.h>
#include "tuya_slist.h"
#include "tuya_uart.h"
#include "tuya_cli.h"
#include "tal_thread.h"
#include "tal_memory.h"
#include "tal_log.h"
#include "tal_mutex.h"

/*============================ MACROS ========================================*/
#ifndef CLI_BUFFER_SIZE
#define CLI_BUFFER_SIZE        100
#endif

#ifndef CLI_HISTORY_NUM
#define CLI_HISTORY_NUM        8
#endif

#ifndef CLI_ARGV_NUM
#define CLI_ARGV_NUM           8
#endif

#ifndef CLI_CMD_TABLE_NUM
#define CLI_CMD_TABLE_NUM      100
#endif

#ifndef CLI_CMD_NAME_MAX
#define CLI_CMD_NAME_MAX       30
#endif

#ifndef CLI_CMD_HELP_MAX
#define CLI_CMD_HELP_MAX       80
#endif
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct {
    UINT8_T             num;
    CLI_CMD_T          *cmd;
} CLI_CMD_TABLE_T;

typedef struct {
    SLIST_HEAD          next;
    CLI_CMD_TABLE_T     table;
} CLI_CMD_NODE_T;

// cli specific keys
typedef enum {
    CLI_NULL_KEY        = '\0',
    CLI_ESC_KEY         = 0x1b,
    CLI_ENTER_KEY       = '\r',
    CLI_BACKSPACE_KEY   = '\b',
    CLI_BACKSPACE2_KEY  = '\177',
    TUYA_CLI_TABLE_KEY  = '\t',
    CLI_FUNC_TAG_KEY    = 0x5b,
    CLI_UP_KEY          = 'A',
    CLI_DOWN_KEY        = 'B',
    CLI_RIGHT_KEY       = 'C',
    CLI_LETF_KEY        = 'D',
} CLI_KEY_T;

// cli history cmds
typedef CHAR_T history_data_t[CLI_BUFFER_SIZE + 1];
typedef struct {
    UINT16_T            write_index;
    UINT16_T            write_count;
    UINT16_T            read_index;
    UINT16_T            read_count;
    history_data_t      data[CLI_HISTORY_NUM];
} CLI_HISTORY_T;

// tuya cli module
typedef struct {
    TUYA_UART_NUM_E     port_id;
    THREAD_HANDLE       thread;
    CHAR_T               *prompt;
    UINT8_T             echo;
    UINT16_T            index;
    UINT16_T            insert;
    CLI_HISTORY_T       history;
    INT_T                 argc;
    CHAR_T               *argv[CLI_ARGV_NUM];
    CHAR_T                buffer[CLI_BUFFER_SIZE + 1];
} TUYA_CLI_T;

/*============================ PROTOTYPES ====================================*/
STATIC VOID __cli_print_prompt(TUYA_CLI_T *cli);

/*============================ LOCAL VARIABLES ===============================*/
STATIC TUYA_CLI_T          *s_cli_handle = NULL;
STATIC SLIST_HEAD           s_cli_dynamic_table;
STATIC CLI_CMD_TABLE_T      s_cli_static_table[CLI_CMD_TABLE_NUM];
STATIC MUTEX_HANDLE         s_mutex = NULL;
// STATIC const  CLI_CMD_T     s_cli_cmd[]  = {};

/*============================ IMPLEMENTATION ================================*/
STATIC INT_T __cli_output(TUYA_UART_NUM_E port_id, CHAR_T *out_str, UINT32_T len)
{
    INT_T ret = 0;
    tal_mutex_lock(s_mutex);
    ret = tal_uart_write(port_id,  (CONST UINT8_T*)out_str, len);
    tal_mutex_unlock(s_mutex);
    return ret;
}

STATIC VOID __cli_print_string(TUYA_CLI_T *cli, CHAR_T *string)
{
    __cli_output(cli->port_id, "\r\n", 2);
    while ('\0' != *string) {
        __cli_output(cli->port_id,  string++, 1);
    }
}

STATIC CLI_CMD_T *__cli_cmd_find_with_name(CHAR_T *name)
{
    INT_T i, j;
    CLI_CMD_T *cmd;

    if (NULL == name) {
        return NULL;
    }

    for (i = 0; i < CLI_CMD_TABLE_NUM; i++) {
        for (j = 0; j < s_cli_static_table[i].num; j++) {
            cmd = s_cli_static_table[i].cmd + j;
            if (0 != strcmp(cmd->name, name)) {
                continue;
            }
            return cmd;
        }
    }

    CLI_CMD_NODE_T  *node = NULL;
    SLIST_HEAD *pos       = NULL;

    SLIST_FOR_EACH_ENTRY(node, CLI_CMD_NODE_T, pos, &s_cli_dynamic_table, next) {
        for (i = 0; i < node->table.num; i++) {
            cmd = node->table.cmd + i;
            if (0 != strcmp(cmd->name, name)) {
                continue;
            }
            return cmd;
        }
    }

    return NULL;
}

STATIC VOID __cli_print_cmd(TUYA_CLI_T *cli, CLI_CMD_T *cmd)
{
    INT_T i = 0;
    __cli_output(cli->port_id, "\r\n", 2);
    __cli_output(cli->port_id, cmd->name, strlen(cmd->name)>(CLI_CMD_NAME_MAX)?(CLI_CMD_NAME_MAX):strlen(cmd->name));
    for (i = CLI_CMD_NAME_MAX-strlen(cmd->name); i > 0; i--) {
        __cli_output(cli->port_id, " ", 1);
    }
    __cli_output(cli->port_id, " | ", 3);

    __cli_output(cli->port_id, cmd->help, strlen(cmd->name)>(CLI_CMD_HELP_MAX)?(CLI_CMD_HELP_MAX):strlen(cmd->help));
    for (i = CLI_CMD_HELP_MAX-strlen(cmd->help); i > 0; i--) {
        __cli_output(cli->port_id, " ", 1);
    }
    //__cli_output(cli->port_id, "|", 1);
}

STATIC VOID __cli_print_all_cmd(TUYA_CLI_T *cli)
{
    INT_T i, j;
    CLI_CMD_T *cmd;

    for (i = 0; i < CLI_CMD_TABLE_NUM; i++) {
        for (j = 0; j < s_cli_static_table[i].num; j++) {
            cmd = s_cli_static_table[i].cmd + j;
            __cli_print_cmd(cli, cmd);
        }
    }

    CLI_CMD_NODE_T  *node = NULL;
    SLIST_HEAD *pos       = NULL;

    SLIST_FOR_EACH_ENTRY(node, CLI_CMD_NODE_T, pos, &s_cli_dynamic_table, next) {
        for (i = 0; i < node->table.num; i++) {
            cmd = node->table.cmd + i;
            __cli_print_cmd(cli, cmd);
        }
    }

    __cli_print_prompt(cli);
}

STATIC VOID __cli_print_cmd_title(TUYA_CLI_T *cli)
{
    INT_T i;

    __cli_output(cli->port_id,  "\r\n", 2);
    for (i = 0; i < CLI_CMD_NAME_MAX + CLI_CMD_HELP_MAX + 3; i++) {
        __cli_output(cli->port_id,  "-", 1);
    }

    __cli_output(cli->port_id,  "\r\ncmd", 5);
    for (i = 3; i < CLI_CMD_NAME_MAX; i++) {
        __cli_output(cli->port_id,  " ", 1);
    }
    __cli_output(cli->port_id,  " | ", 3);
    __cli_output(cli->port_id,  "help\r\n", 6);

    for (i = 0; i < CLI_CMD_NAME_MAX + CLI_CMD_HELP_MAX + 3; i++) {
        __cli_output(cli->port_id,  "-", 1);
    }    
}

STATIC INT_T __tuya_cli_table_key(TUYA_CLI_T *cli)
{
    CLI_CMD_T  *cmd;
    CLI_CMD_T  *match_cmd = NULL;

    INT_T i, j ;
    INT_T count = 0;

    //! print all cmd
    if (0 == cli->index) {
        __cli_print_cmd_title(cli);
        __cli_print_all_cmd(cli);
        return OPRT_OK;
    }

    //! match one or more
    for (i = 0; i < CLI_CMD_TABLE_NUM; i++) {
        for (j = 0; j < s_cli_static_table[i].num; j++) {
            cmd = s_cli_static_table[i].cmd + j;
            if (0 != strncmp(cmd->name, cli->buffer, cli->index)) {
                continue;
            }
            match_cmd = cmd;
            count++;
        }
    }

    CLI_CMD_NODE_T  *node = NULL;
    SLIST_HEAD *pos       = NULL;

    SLIST_FOR_EACH_ENTRY(node, CLI_CMD_NODE_T, pos, &s_cli_dynamic_table, next) {
        for (i = 0; i < node->table.num; i++) {
            cmd = node->table.cmd + i;
            if (0 != strncmp(cmd->name, cli->buffer, cli->index)) {
                continue;
            }
            match_cmd = cmd;
            count++;
        }
    }

    if (count > 1) {    //! print more
        __cli_print_cmd_title(cli);
        for (i = 0; i < CLI_CMD_TABLE_NUM; i++) {
            for (j = 0; j < s_cli_static_table[i].num; j++) {
                cmd = s_cli_static_table[i].cmd + j;
                if (0 == strncmp(cmd->name, cli->buffer, cli->index)) {
                    __cli_print_cmd(cli, cmd);
                }
            }
        }
        SLIST_FOR_EACH_ENTRY(node, CLI_CMD_NODE_T, pos, &s_cli_dynamic_table, next) {
            for (i = 0; i < node->table.num; i++) {
                cmd = node->table.cmd + i;
                if (0 == strncmp(cmd->name, cli->buffer, cli->index)) {
                    __cli_print_cmd(cli, cmd);
                }
            }
        }
    } else if (count) { //! print one
        strcpy(cli->buffer, match_cmd->name);
        cli->index  = strlen(cli->buffer);
        cli->insert = cli->index;
    } else {
        cli->insert = cli->index;
    }

    __cli_print_prompt(cli);
    __cli_output(cli->port_id,  cli->buffer, cli->index);

    return OPRT_OK;
}

VOID command_list_output(INT_T argc, CHAR_T *argv[])
{
    __cli_print_all_cmd(s_cli_handle);
}


STATIC INT_T __cli_histroy_data_perv(TUYA_CLI_T *cli, history_data_t **data)
{
    CLI_HISTORY_T  *history = &cli->history;

    if (history->read_count == 0) {
        return false;
    }
    if (history->read_count != history->write_count) {
        if (0 == history->read_index) {
            history->read_index = CLI_HISTORY_NUM;
        }
        history->read_index--;
    }
    history->read_count--;
    *data = (history_data_t *)history->data[history->read_index];

    return true;
}

STATIC INT_T __cli_histroy_data_next(TUYA_CLI_T *cli, history_data_t **data)
{
    CLI_HISTORY_T  *history = &cli->history;

    if (history->write_count <= (history->read_count + 1)) {
        return false;
    }
    if (CLI_HISTORY_NUM == ++history->read_index) {
        history->read_index = 0;
    }
    history->read_count++;

    *data = (history_data_t *)history->data[history->read_index];

    return true;
}


STATIC INT_T __cli_histroy_data_save(TUYA_CLI_T *cli)
{
    history_data_t  *history_data = NULL;
    CLI_HISTORY_T   *history = &cli->history;

    history->read_count = history->write_count;
    history->read_index = history->write_index - 1;

    if (__cli_histroy_data_perv(cli, &history_data)) {
        if (0 == strcmp(cli->buffer, (CHAR_T *)history_data)) {
            history->read_count = history->write_count;
            history->read_index = history->write_index - 1;
            return true;
        }
    }

    if (CLI_HISTORY_NUM <= history->write_index) {
        history->write_index = 0;
    }
    history->read_index = history->write_index;
    if (CLI_HISTORY_NUM > history->write_count) {
        history->write_count++;
    }
    history->read_count = history->write_count;

    memcpy(history->data[history->write_index++], cli->buffer, cli->index + 1);

    return true;
}


STATIC INT_T __cli_key_detect(TUYA_UART_NUM_E port_id, CHAR_T *data, CLI_KEY_T *key)
{
    CHAR_T ch;
    enum {
        CHECK_KEY,
        CHECK_FUNC_TAG,
        CHECK_FUNC_KEY,
    } state = CHECK_KEY;

    for (;;) {
        tal_uart_read(port_id, (UINT8_T*)&ch, 1);

        switch (state) {

        case CHECK_KEY:
            if (CLI_ENTER_KEY      == ch ||
                CLI_BACKSPACE_KEY  == ch ||
                CLI_BACKSPACE2_KEY == ch ||
                TUYA_CLI_TABLE_KEY      == ch) {
                *key = ch;
                return OPRT_OK;
            } else if (CLI_ESC_KEY == ch) {
                state = CHECK_FUNC_TAG;
            } else {
                *data = ch;
                *key  = CLI_NULL_KEY;
                return OPRT_OK;
            }
            break;

        case CHECK_FUNC_TAG:
            if (CLI_FUNC_TAG_KEY == ch) {
                state = CHECK_FUNC_KEY;
            } else {
                state = CHECK_KEY;
            }
            break;

        case CHECK_FUNC_KEY:
            if (CLI_UP_KEY    == ch ||
                CLI_DOWN_KEY  == ch ||
                CLI_LETF_KEY  == ch ||
                CLI_RIGHT_KEY == ch) {
                *key = ch;
                return OPRT_OK;
            } else {
                state = CHECK_KEY;
            }
            break;
        }
    }

    return OPRT_OK;
}


STATIC VOID __cli_print_prompt(TUYA_CLI_T *cli)
{
    __cli_output(cli->port_id,  "\r\n", 2);
    __cli_output(cli->port_id,  cli->prompt, strlen(cli->prompt));
}


STATIC INT_T __cli_parse_buffer(CHAR_T *buffer, INT_T *argc, CHAR_T **argv)
{
    enum {
        PARSE_SPACE,
        PARSE_ARG,
    } state = PARSE_SPACE;
    CHAR_T *readchar  = buffer;
    CHAR_T *writechar = buffer;

    *argc = 0;

    do {
        switch (state) {

        case PARSE_SPACE:
            if (' ' == *readchar) {
                readchar++;
            } else if ('\0' == *readchar) {
                *writechar = '\0';
                goto __exit;
            } else {
                state = PARSE_ARG;
            }
            break;

        case PARSE_ARG:
            if (' ' == *readchar) {
                *writechar++ = '\0';
                readchar++;
                (*argc)++;
                state = PARSE_SPACE;
            } else if ('\0' == *readchar) {
                *writechar = '\0';
                (*argc)++;
                goto __exit;
            }  else {
                *writechar++ = *readchar++;;
            }
            break;
        }
    } while (true);

__exit:

    if (CLI_ARGV_NUM < *argc) {
        *argc = CLI_ARGV_NUM;
    }

    INT_T i;
    for (i = 0; i < *argc; i++) {
        argv[i] = buffer;
        buffer += strlen(buffer) + 1;
    }

    return OPRT_OK;
}

STATIC INT_T __cli_cmd_exec(TUYA_CLI_T *cli, INT_T argc, CHAR_T **argv)
{
    CLI_CMD_T *cmd;

    INT_T i = 0;
    cmd = __cli_cmd_find_with_name(argv[0]);
    if (cmd) {
        __cli_output(cli->port_id, "\r\nfind comand : ", 16);
        __cli_output(cli->port_id, cmd->name, strlen(cmd->name));
        __cli_output(cli->port_id, "\r\ncomand help : ", 16);
        __cli_output(cli->port_id, cmd->help, strlen(cmd->help));
        __cli_output(cli->port_id, "\r\n", 2);
        for (i = 0; i < CLI_CMD_HELP_MAX; i++) {
            __cli_output(cli->port_id, "=", 1);
        }
        __cli_output(cli->port_id, "\r\n", 2);

        cmd->func(argc, argv);
        return OPRT_OK;
    } else {
        __cli_output(cli->port_id, "\r\ncomand not found!\r\n", 21);
    }

    return OPRT_NOT_FOUND;
}

STATIC VOID __cli_enter_key(TUYA_CLI_T *cli)
{
    INT_T result;

    if (0 == cli->index) {
        __cli_print_prompt(cli);
        return;
    }
    cli->buffer[cli->index] = 0;
    __cli_histroy_data_save(cli);
    __cli_parse_buffer(cli->buffer, &cli->argc, cli->argv);
    result = __cli_cmd_exec(cli, cli->argc, cli->argv);
    if (OPRT_OK != result) {
        __cli_print_string(cli, "No command or file name");
    }
    __cli_print_prompt(cli);
    cli->index  = 0;
    cli->insert = 0;
    memset(cli->buffer, 0, sizeof(cli->buffer));
}

STATIC VOID __cli_backspace_key(TUYA_CLI_T *cli)
{
    CHAR_T ch = '\b';

    if (0 == cli->index) {
        return;
    } else if (cli->index > cli->insert) {
        cli->index--;
        cli->insert--;
        memmove(&cli->buffer[cli->insert], &cli->buffer[cli->insert + 1], cli->index - cli->insert);
        cli->buffer[cli->index] = '\0';
        __cli_output(cli->port_id, &ch, 1);
        __cli_output(cli->port_id, &cli->buffer[cli->insert], cli->index - cli->insert);
        __cli_output(cli->port_id, " \b", 2);
        INT_T i;
        for (i = 0; i < (cli->index - cli->insert); i++) {
            __cli_output(cli->port_id, &ch, 1);
        }
    } else {
        cli->index--;
        cli->insert--;
        cli->buffer[cli->insert] = '\0';
        __cli_output(cli->port_id, "\b \b", 3);
    }
}

STATIC VOID __cli_up_key(TUYA_CLI_T *cli)
{
    INT_T i;
    CHAR_T ch;
    history_data_t  *history_data;

    if (__cli_histroy_data_perv(cli, &history_data)) {
        ch = '\r';
        __cli_output(cli->port_id, &ch, 1);
        ch = ' ';
        for (i = 0; i < cli->index + strlen(cli->prompt); i++) {
            __cli_output(cli->port_id, &ch, 1);
        }
        ch = '\r';
        __cli_output(cli->port_id, &ch, 1);
        __cli_output(cli->port_id, cli->prompt,  strlen(cli->prompt));
        __cli_output(cli->port_id, (CHAR_T *)history_data, strlen((CHAR_T *)history_data));
        strcpy(cli->buffer, (CHAR_T *)history_data);
        cli->index  = strlen(cli->buffer);
        cli->buffer[cli->index] = '\0';
        cli->insert = cli->index;
    }
}

STATIC VOID __cli_down_key(TUYA_CLI_T *cli)
{
    INT_T i;
    CHAR_T ch;
    history_data_t  *history_data;

    if (__cli_histroy_data_next(cli, &history_data)) {
        ch = '\r';
        __cli_output(cli->port_id, &ch, 1);
        ch = ' ';
        for (i = 0; i < cli->index + strlen(cli->prompt); i++) {
            __cli_output(cli->port_id, &ch, 1);
        }
        ch = '\r';
        __cli_output(cli->port_id, &ch, 1);
        __cli_output(cli->port_id, cli->prompt,  strlen(cli->prompt));
        __cli_output(cli->port_id, (CHAR_T *)history_data, strlen((CHAR_T *)history_data));
        strcpy(cli->buffer, (CHAR_T *)history_data);
        cli->index  = strlen(cli->buffer);
        cli->buffer[cli->index] = '\0';
        cli->insert = cli->index;
    }
}

STATIC VOID __cli_letf_key(TUYA_CLI_T *cli)
{
    CHAR_T ch = '\b';

    if (cli->insert) {
        __cli_output(cli->port_id, &ch, 1);
        cli->insert--;
    }
}

STATIC VOID __cli_right_key(TUYA_CLI_T *cli)
{
    CHAR_T ch;

    if (cli->insert < cli->index) {
        ch = cli->buffer[cli->insert];
        __cli_output(cli->port_id, &ch, 1);
        cli->insert++;
    }
}

STATIC VOID __cli_key_app(TUYA_CLI_T *cli, CLI_KEY_T key)
{
    switch (key) {

    case TUYA_CLI_TABLE_KEY:
        __tuya_cli_table_key(cli);
        break;

    case CLI_ENTER_KEY:
        __cli_enter_key(cli);
        break;

    case CLI_BACKSPACE_KEY:
    case CLI_BACKSPACE2_KEY:
        __cli_backspace_key(cli);
        break;

    case CLI_UP_KEY:
        __cli_up_key(cli);
        break;

    case CLI_DOWN_KEY:
        __cli_down_key(cli);
        break;

    case CLI_LETF_KEY:
        __cli_letf_key(cli);
        break;

    case CLI_RIGHT_KEY:
        __cli_right_key(cli);
        break;

    default:
        break;
    }
}

STATIC VOID __tuya_cli_welcome(TUYA_CLI_T *cli)
{
    __cli_output(cli->port_id, "*************************************************************\r\n", 64);
    __cli_output(cli->port_id, "*    ##########  ######  ##########         ######  ######  *\r\n", 64);
    __cli_output(cli->port_id, "*      ##   ##  ##  ##  ##  ##  ##          ##  ##  ##      *\r\n", 64);
    __cli_output(cli->port_id, "*     ##   ##  ##  ##  ##  ##  ##           ##  ##  ######  *\r\n", 64);
    __cli_output(cli->port_id, "*    ##   ##  ##  ######  ######            ##  ##  ######  *\r\n", 64);
    __cli_output(cli->port_id, "*   ##   ##  ##    ##    ##  ##             ##  ##      ##  *\r\n", 64);
    __cli_output(cli->port_id, "*  ##   ######    ##    ##  ##              ######  ######  *\r\n", 64);
    __cli_output(cli->port_id, "*************************************************************\r\n", 64);
    __cli_output(cli->port_id, "*         Welcome to TuyaOS examples! TAB for help!         *\r\n", 64);
    __cli_output(cli->port_id, "*************************************************************\r\n", 64);

    return;
}

STATIC VOID __tuya_cli_task(VOID  *parameter)
{
    TUYA_CLI_T  *cli;
    CHAR_T  data;
    CLI_KEY_T   key;

    // show welcom info
    cli = (TUYA_CLI_T *) parameter;
    cli->prompt = "tuya>";
    __tuya_cli_welcome(cli);
    __cli_print_prompt(cli);
    cli->echo = 1;

    for (;;) {
        __cli_key_detect(cli->port_id, &data, &key);
        if (CLI_NULL_KEY != key) {
            __cli_key_app(cli, key);
            continue;
        }
        if (!((32 <= data) && (127 >= data))) {
            continue;
        }
        if (CLI_BUFFER_SIZE - 1 < cli->index) {
            continue;
        }
        if (cli->insert != cli->index) {
            memmove(&cli->buffer[cli->insert + 1],  &cli->buffer[cli->insert], cli->index - cli->insert);
            cli->buffer[cli->insert] = data;
            cli->index++;
            __cli_output(cli->port_id, &cli->buffer[cli->insert], cli->index - cli->insert);
            INT_T i;
            CHAR_T ch = '\b';
            cli->insert++;
            for (i = 0; i < (cli->index - cli->insert); i++) {
                __cli_output(cli->port_id, &ch, 1);
            }
            continue;
        } else {
            cli->buffer[cli->index++] = data;
            cli->insert = cli->index;
        }
        if (cli->echo) {
            __cli_output(cli->port_id, &data, 1);
        }
    }
}

STATIC INT_T __cli_cmd_register(CLI_CMD_T *cmd, UINT8_T num)
{
    INT_T i = 0;

    for (i = 0; i < CLI_CMD_TABLE_NUM; i++) {
        if (s_cli_static_table[i].cmd) {
            continue;
        }
        s_cli_static_table[i].cmd = cmd;
        s_cli_static_table[i].num = num;
        return OPRT_OK;
    }
    CLI_CMD_NODE_T *node = Malloc(sizeof(CLI_CMD_NODE_T));
    if (NULL == node) {
        return OPRT_MALLOC_FAILED;
    }
    node->table.cmd = cmd;
    node->table.num = num;
    tuya_slist_add_head(&s_cli_dynamic_table, &node->next);

    return OPRT_OK;
}

/**
 * @brief cli command register
 *
 * @param[in] cmd Info of one command
 * @param[in] num Number
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 *
 */
INT_T tuya_cli_cmd_register(CONST CLI_CMD_T *cmd, UINT8_T num)
{
    if (NULL == cmd || 0 == num) {
        return OPRT_INVALID_PARM;
    }

    return __cli_cmd_register((CLI_CMD_T *)cmd, num);
}

/**
 * @brief uart cli init
 *
 * @param[in] uart_num The number of UART
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 *
 */
INT_T tuya_cli_init_with_uart(UINT8_T uart_num)
{
    INT_T result = OPRT_OK;

    if (s_cli_handle) {
        return OPRT_OK;
    }
    s_cli_handle = Malloc(sizeof(TUYA_CLI_T));
    if (NULL == s_cli_handle) {
        return OPRT_MALLOC_FAILED;
    }
    if (s_mutex == NULL) {
        tal_mutex_create_init(&s_mutex);
    }
    if (NULL == s_mutex) {
        Free(s_cli_handle);
        return OPRT_OS_ADAPTER_MUTEX_CREAT_FAILED;
    }
    memset(s_cli_handle, 0, sizeof(TUYA_CLI_T));
    s_cli_handle->port_id = uart_num;
    TAL_UART_CFG_T cfg = {0};
    cfg.base_cfg.baudrate = 115200;
    cfg.base_cfg.databits = TUYA_UART_DATA_LEN_8BIT;
    cfg.base_cfg.stopbits = TUYA_UART_STOP_LEN_1BIT;
    cfg.base_cfg.parity = TUYA_UART_PARITY_TYPE_NONE;
    cfg.rx_buffer_size = 256;
    cfg.open_mode = O_BLOCK;
    result = tal_uart_init(uart_num,&cfg);
    if (OPRT_OK != result) {
        TAL_PR_ERR("uart init failed", result);
        goto __exit;
    }
    //tuya_cli_cmd_register((CLI_CMD_T *)&s_cli_cmd, 1);

    THREAD_CFG_T   param;

    param.priority   = THREAD_PRIO_3;
    param.stackDepth = 1024*4;
    param.thrdname   = "cli";

    result = tal_thread_create_and_start(&s_cli_handle->thread, NULL, NULL, __tuya_cli_task, s_cli_handle, &param);
    if (OPRT_OK != result) {
        TAL_PR_ERR("tuya cli create thread failed %d", result);
        goto __exit;
    }

    return OPRT_OK;

__exit:
    Free(s_cli_handle);
    s_cli_handle = NULL;

    return OPRT_COM_ERROR;
}

/**
 * @brief cli init function,default uart0
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 *
 */
INT_T tuya_cli_init(VOID_T)
{
    return  tuya_cli_init_with_uart(UART_NUM_0);
}

/**
 * @brief tuya_cli_printf
 *
 * @param[in] format ARGUMENT(s) according to FORMAT
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 *
 */
VOID tuya_cli_printf(CONST CHAR_T *format, ...)
{
    if (format == ((VOID *)(0)))
        return;

    INT_T len;
    va_list ap;
    CHAR_T *output_buf =  Malloc(512);
    if(output_buf == NULL) {
        return;
    }

    memset(output_buf,0,512);
    va_start(ap, format);
    len = vsprintf(output_buf,format,ap);
    va_end(ap);
    if (len > 0) {
        __cli_output(s_cli_handle->port_id,output_buf,len);
        __cli_output(s_cli_handle->port_id,  "\r\n", 2);
    }
    Free(output_buf);
}
