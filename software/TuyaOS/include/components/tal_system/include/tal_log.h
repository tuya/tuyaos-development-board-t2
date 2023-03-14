/**
* @file uni_log.h
* @brief provide log management api
* @version 0.1
* @date 2018-11-09
*
* @copyright Copyright 2018-2021 Tuya Inc. All Rights Reserved.
*
*/
#ifndef _TAL_LOG_H_
#define _TAL_LOG_H_

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Definition of log level
 */
typedef enum {
    TAL_LOG_LEVEL_ERR,
    TAL_LOG_LEVEL_WARN,
    TAL_LOG_LEVEL_NOTICE,
    TAL_LOG_LEVEL_INFO,
    TAL_LOG_LEVEL_DEBUG,
    TAL_LOG_LEVEL_TRACE,
} TAL_LOG_LEVEL_E;

typedef TAL_LOG_LEVEL_E LOG_LEVEL;

#define DEF_LOG_BUF_LEN 	1024


OPERATE_RET tal_log_module_print(CHAR_T             *name, 
                                 CONST TAL_LOG_LEVEL_E     level,
                                 CHAR_T             *file, 
                                 CONST INT_T               line, 
                                 CHAR_T             *fmt,
                                ...);

OPERATE_RET tal_log_print(CONST TAL_LOG_LEVEL_E     level,
                          CHAR_T              *file, 
                          CONST INT_T               line, 
                          CHAR_T              *fmt,
                          ...);

// file name maybe define from complie parameter
#ifndef _THIS_FILE_NAME_
#define _THIS_FILE_NAME_ __FILE__
#endif

#if defined(ENABLE_LOG) && (ENABLE_LOG==1)

#define TAL_PR_ERR(fmt, ...)                        tal_log_print(TAL_LOG_LEVEL_ERR,     _THIS_FILE_NAME_,  __LINE__, fmt, ##__VA_ARGS__)
#define TAL_PR_WARN(fmt, ...)                       tal_log_print(TAL_LOG_LEVEL_WARN,    _THIS_FILE_NAME_,  __LINE__, fmt, ##__VA_ARGS__)
#define TAL_PR_NOTICE(fmt, ...)                     tal_log_print(TAL_LOG_LEVEL_NOTICE,  _THIS_FILE_NAME_,  __LINE__, fmt, ##__VA_ARGS__)
#define TAL_PR_INFO(fmt, ...)                       tal_log_print(TAL_LOG_LEVEL_INFO,    _THIS_FILE_NAME_,  __LINE__, fmt, ##__VA_ARGS__)
#define TAL_PR_DEBUG(fmt, ...)                      tal_log_print(TAL_LOG_LEVEL_DEBUG,    _THIS_FILE_NAME_,  __LINE__, fmt, ##__VA_ARGS__)
#define TAL_PR_TRACE(fmt, ...)                      tal_log_print(TAL_LOG_LEVEL_TRACE,    _THIS_FILE_NAME_,  __LINE__, fmt, ##__VA_ARGS__)

#define TAL_PR_HEXDUMP_ERR(title, buf, size)        tal_log_hex_dump(TAL_LOG_LEVEL_ERR,     _THIS_FILE_NAME_,  __LINE__, title, 64, buf, size)
#define TAL_PR_HEXDUMP_WARN(title, buf, size)       tal_log_hex_dump(TAL_LOG_LEVEL_WARN,    _THIS_FILE_NAME_,  __LINE__, title, 64, buf, size)
#define TAL_PR_HEXDUMP_NOTICE(title, buf, size)     tal_log_hex_dump(TAL_LOG_LEVEL_NOTICE,  _THIS_FILE_NAME_,  __LINE__, title, 64, buf, size)
#define TAL_PR_HEXDUMP_INFO(title, buf, size)       tal_log_hex_dump(TAL_LOG_LEVEL_INFO,    _THIS_FILE_NAME_,  __LINE__, title, 64, buf, size)
#define TAL_PR_HEXDUMP_DEBUG(title, buf, size)      tal_log_hex_dump(TAL_LOG_LEVEL_DEBUG,    _THIS_FILE_NAME_,  __LINE__, title, 64, buf, size)
#define TAL_PR_HEXDUMP_TRACE(title, buf, size)      tal_log_hex_dump(TAL_LOG_LEVEL_TRACE,    _THIS_FILE_NAME_,  __LINE__, title, 64, buf, size)

#else 

#define TAL_PR_ERR(fmt, ...)                        
#define TAL_PR_WARN(fmt, ...)                       
#define TAL_PR_NOTICE(fmt, ...)                     
#define TAL_PR_INFO(fmt, ...)                       
#define TAL_PR_DEBUG(fmt, ...)                      
#define TAL_PR_TRACE(fmt, ...)                      

#define TAL_PR_HEXDUMP_ERR(title, buf, size)        
#define TAL_PR_HEXDUMP_WARN(title, buf, size)       
#define TAL_PR_HEXDUMP_NOTICE(title, buf, size)     
#define TAL_PR_HEXDUMP_INFO(title, buf, size)       
#define TAL_PR_HEXDUMP_DEBUG(title, buf, size)      
#define TAL_PR_HEXDUMP_TRACE(title, buf, size)      


#endif

#define TAL_PR_DEBUG_RAW(fmt, ...)                  tal_log_print_raw(fmt, ##__VA_ARGS__)
#define TAL_PR_TRACE_ENTER()                        TAL_PR_TRACE("enter [%s]",  (const char*)__func__)
#define TAL_PR_TRACE_LEAVE()                        TAL_PR_TRACE(("leave [%s]", (const char*)__func__))

// log output interface with module name
#define TAL_MPR_ERR(module,fmt, ...)                tal_log_module_print(module, TAL_LOG_LEVEL_ERR,     _THIS_FILE_NAME_,  __LINE__, fmt, ##__VA_ARGS__)
#define TAL_MPR_WARN(module,fmt, ...)               tal_log_module_print(module, TAL_LOG_LEVEL_WARN,    _THIS_FILE_NAME_,  __LINE__, fmt, ##__VA_ARGS__)
#define TAL_MPR_NOTICE(module,fmt, ...)             tal_log_module_print(module, TAL_LOG_LEVEL_NOTICE,  _THIS_FILE_NAME_,  __LINE__, fmt, ##__VA_ARGS__)
#define TAL_MPR_INFO(module,fmt, ...)               tal_log_module_print(module, TAL_LOG_LEVEL_INFO,    _THIS_FILE_NAME_,  __LINE__, fmt, ##__VA_ARGS__)
#define TAL_MPR_DEBUG(module,fmt, ...)              tal_log_module_print(module, TAL_LOG_LEVEL_DEBUG,   _THIS_FILE_NAME_,  __LINE__, fmt, ##__VA_ARGS__)
#define TAL_MPR_TRACE(module,fmt, ...)              tal_log_module_print(module, TAL_LOG_LEVEL_TRACE,   _THIS_FILE_NAME_,  __LINE__, fmt, ##__VA_ARGS__)


// prototype of log output function
typedef VOID (*TAL_LOG_OUTPUT_CB)(IN CONST  CHAR_T *str);

/**
 * @brief initialize log management.
 *
 * @param[in] level , set log level
 * @param[in] buf_len , set log buffer size
 * @param[in] output , log print function pointer
 *
 * @note This API is used for initializing log management.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_log_create_manage_and_init(CONST TAL_LOG_LEVEL_E level, CONST INT_T buf_len, CONST TAL_LOG_OUTPUT_CB output);

/**
 * @brief add one output terminal.
 *
 * @param[in] name , terminal name
 * @param[in] term , output function pointer
 *
 * @note This API is used for adding one output terminal.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_log_add_output_term(CONST CHAR_T *name, CONST TAL_LOG_OUTPUT_CB term);

/**
 * @brief delete one output terminal.
 *
 * @param[in] name , terminal name
 *
 * @note This API is used for delete one output terminal.
 *
 * @return NONE
 */
VOID tal_log_del_output_term(CONST CHAR_T *name);

/**
 * @brief set global log level.
 *
 * @param[in] curLogLevel , log level
 *
 * @note This API is used for setting global log level.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_log_set_manage_attr(CONST TAL_LOG_LEVEL_E level);

/**
 * @brief set log time whether show in millisecond.
 *
 * @param[in] if_ms_level, whether log time include millisecond
 *
 * @note This API is used for setting log time whether include milisecond.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_log_set_manage_ms_info(BOOL_T if_ms_level);

/**
 * @brief get global log level.
 *
 * @param[in] pCurLogLevel, global log level
 *
 * @note This API is used for getting global log level.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_log_get_log_manage_attr(TAL_LOG_LEVEL_E *level);
/**
 * @brief add one module's log level
 *
 * @param[in] module_name, module name
 * @param[in] logLevel, this module's log level
 *
 * @note This API is used for adding one module's log level.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_log_add_module_level(CONST PCHAR_T module_name, CONST TAL_LOG_LEVEL_E level);

/**
 * @brief add one module's log level
 *
 * @param[in] pModuleName, module name
 * @param[in] logLevel, this module's log level
 *
 * @note This API is used for adding one module's log level.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_log_set_module_level(CONST PCHAR_T module_name, TAL_LOG_LEVEL_E level);
/**
 * @brief get one module's log level
 *
 * @param[in] pModuleName, module name
 * @param[in] logLevel, this module's log level
 *
 * @note This API is used for getting one module's log level.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_log_get_module_level(CONST PCHAR_T module_name, TAL_LOG_LEVEL_E *level);

/**
 * @brief delete one module's log level
 *
 * @param[in] pModuleName, module name
 *
 * @note This API is used for deleting one module's log level.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_log_delete_module_level(CONST PCHAR_T module_name);

/**
 * @brief only print user log information
 *
 * @param[in] pFmt, format string
 * @param[in] ..., parameter
 *
 * @note This API is used for print only user log info.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_log_print_raw(CONST PCHAR_T pFmt,...);

/**
 * @brief destroy log management
 *
 * @param[in] pFmt, format string
 * @param[in] ..., parameter
 *
 * @note This API is used for destroy log management.
 *
 * @return NONE
 */
VOID tal_log_release_manager(VOID);


/**
 * @brief print a buffer in hex format
 *
 * @param[in] title, buffer title for print
 * @param[in] width, one line width
 * @param[in] buf, buffer address
 * @param[in] size, buffer size
 *
 * @note This API is used for print one buffer.
 *
 * @return NONE
 */
VOID tal_log_hex_dump(CONST TAL_LOG_LEVEL_E     level,
                      CHAR_T              *file,
                      CONST INT_T               line,
                      CHAR_T                    *title,
                      UINT8_T                   width,
                      UINT8_T                   *buf,
                      UINT16_T                  size);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif


