#ifndef _UNI_LOG_H
#define _UNI_LOG_H

#include "tal_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TY_LOG_LEVEL_ERR                     TAL_LOG_LEVEL_ERR
#define TY_LOG_LEVEL_WARN                    TAL_LOG_LEVEL_WARN
#define TY_LOG_LEVEL_NOTICE                  TAL_LOG_LEVEL_NOTICE
#define TY_LOG_LEVEL_INFO                    TAL_LOG_LEVEL_INFO
#define TY_LOG_LEVEL_DEBUG                   TAL_LOG_LEVEL_DEBUG
#define TY_LOG_LEVEL_TRACE                   TAL_LOG_LEVEL_TRACE

#define AddOutputTerm                        tal_log_add_output_term
#define DelOutputTerm                        tal_log_del_output_term
#define SetLogManageAttr                     tal_log_set_manage_attr
#define SetLogManagemtime                    tal_log_set_manage_ms_info
#define GetLogManageAttr                     tal_log_get_log_manage_attr
#define PrintLogRaw                          tal_log_print_raw

#define CreateLogManageAndInit(level, buf_len, output)     \
     tal_log_create_manage_and_init(level, buf_len, (TAL_LOG_OUTPUT_CB)output)

#define tuya_debug_hex_dump(title, width, buf, size)       \
     tal_log_hex_dump(TAL_LOG_LEVEL_NOTICE, __FILE__, __LINE__, title, width, buf, size)


// default global log level
#if defined(TUYA_IOT_DEBUG) && (TUYA_IOT_DEBUG==1)
#define TY_DEAULT_LOG_LEVEL                  TY_LOG_LEVEL_DEBUG
#else
#define TY_DEAULT_LOG_LEVEL                  TY_LOG_LEVEL_INFO
#endif

// file name maybe define from complie parameter
#ifndef _THIS_FILE_NAME_
#define _THIS_FILE_NAME_ __FILE__
#endif


#define SAK_PRINT_LOG(module,level,fmt, ...)           \
     tal_log_module_print(module, level, _THIS_FILE_NAME_ , __LINE__, fmt, ##__VA_ARGS__)

// this level, log dont has line_number information 
#define TUYA_REDUCE_BIN_LEVEL1  1
// this level, log dont has file_name and line_number information, and trace information
#define TUYA_REDUCE_BIN_LEVEL2  2
// this level, log dont has file_name , line_number and no tare and debug information
#define TUYA_REDUCE_BIN_LEVEL3  3

// all type of log output interface
#if defined(TUYA_REDUCE_BIN) && (TUYA_REDUCE_BIN >= TUYA_REDUCE_BIN_LEVEL2)
OPERATE_RET PrintErrLog(CHAR_T* pFmt,...);
OPERATE_RET PrintWarnLog(CHAR_T* pFmt,...);
OPERATE_RET PrintInfoLog(CHAR_T* pFmt,...);
OPERATE_RET PrintNoticeLog(CHAR_T* pFmt,...);
OPERATE_RET PrintDebugLog(CHAR_T* pFmt,...);
OPERATE_RET PrintTraceLog(CHAR_T* pFmt,...);

#define SAK_PRINT_ERR_LOG(fmt, ...)          PrintErrLog(fmt,##__VA_ARGS__)
#define SAK_PRINT_WARN_LOG(fmt, ...)         PrintWarnLog(fmt,##__VA_ARGS__)
#define SAK_PRINT_INFO_LOG(fmt, ...)         PrintInfoLog(fmt,##__VA_ARGS__)
#define SAK_PRINT_NOTICE_LOG(fmt, ...)       PrintNoticeLog(fmt,##__VA_ARGS__)
#define SAK_PRINT_DEBUG_LOG(fmt, ...)        PrintDebugLog(fmt,##__VA_ARGS__)
#define SAK_PRINT_TRACE_LOG(fmt, ...)        PrintTraceLog(fmt,##__VA_ARGS__)

#elif defined(TUYA_REDUCE_BIN) && (TUYA_REDUCE_BIN >= TUYA_REDUCE_BIN_LEVEL1)
OPERATE_RET PrintErrLog(CHAR_T* pFile, CHAR_T* pFmt,...);
OPERATE_RET PrintWarnLog(CHAR_T* pFile, CHAR_T* pFmt,...);
OPERATE_RET PrintInfoLog(CHAR_T* pFile, CHAR_T* pFmt,...);
OPERATE_RET PrintNoticeLog(CHAR_T* pFile, CHAR_T* pFmt,...);
OPERATE_RET PrintDebugLog(CHAR_T* pFile, CHAR_T* pFmt,...);
OPERATE_RET PrintTraceLog(CHAR_T* pFile, CHAR_T* pFmt,...);

#define SAK_PRINT_ERR_LOG(fmt, ...)          PrintErrLog(_THIS_FILE_NAME_, fmt,##__VA_ARGS__)
#define SAK_PRINT_WARN_LOG(fmt, ...)         PrintWarnLog(_THIS_FILE_NAME_, fmt,##__VA_ARGS__)
#define SAK_PRINT_INFO_LOG(fmt, ...)         PrintInfoLog(_THIS_FILE_NAME_, fmt,##__VA_ARGS__)
#define SAK_PRINT_NOTICE_LOG(fmt, ...)       PrintNoticeLog(_THIS_FILE_NAME_, fmt,##__VA_ARGS__)
#define SAK_PRINT_DEBUG_LOG(fmt, ...)        PrintDebugLog(_THIS_FILE_NAME_, fmt,##__VA_ARGS__)
#define SAK_PRINT_TRACE_LOG(fmt, ...)        PrintTraceLog(_THIS_FILE_NAME_, fmt,##__VA_ARGS__)

#else
OPERATE_RET PrintErrLog(CHAR_T* pFile, UINT_T line, CHAR_T* pFmt,...);
OPERATE_RET PrintWarnLog(CHAR_T* pFile, UINT_T line, CHAR_T* pFmt,...);
OPERATE_RET PrintInfoLog(CHAR_T* pFile, UINT_T line, CHAR_T* pFmt,...);
OPERATE_RET PrintNoticeLog(CHAR_T* pFile, UINT_T line, CHAR_T* pFmt,...);
OPERATE_RET PrintDebugLog(CHAR_T* pFile, UINT_T line, CHAR_T* pFmt,...);
OPERATE_RET PrintTraceLog(CHAR_T* pFile, UINT_T line, CHAR_T* pFmt,...);

#define SAK_PRINT_ERR_LOG(fmt, ...)          PrintErrLog(_THIS_FILE_NAME_, __LINE__, fmt,##__VA_ARGS__)
#define SAK_PRINT_WARN_LOG(fmt, ...)         PrintWarnLog(_THIS_FILE_NAME_, __LINE__,fmt,##__VA_ARGS__)
#define SAK_PRINT_INFO_LOG(fmt, ...)         PrintInfoLog(_THIS_FILE_NAME_, __LINE__,fmt,##__VA_ARGS__)
#define SAK_PRINT_NOTICE_LOG(fmt, ...)       PrintNoticeLog(_THIS_FILE_NAME_, __LINE__,fmt,##__VA_ARGS__)
#define SAK_PRINT_DEBUG_LOG(fmt, ...)        PrintDebugLog(_THIS_FILE_NAME_, __LINE__,fmt,##__VA_ARGS__)
#define SAK_PRINT_TRACE_LOG(fmt, ...)        PrintTraceLog(_THIS_FILE_NAME_, __LINE__,fmt,##__VA_ARGS__)
#endif

// there is no trace and debug information when REDUCE_BIN leven bigger than LEVEL3
#if defined(TUYA_REDUCE_BIN) && (TUYA_REDUCE_BIN >= TUYA_REDUCE_BIN_LEVEL3)
#define PR_TRACE(fmt, ...)
#define PR_TRACE_ENTER() 
#define PR_TRACE_LEAVE()
#define MPR_TRACE(module,fmt, ...) 
#define PR_DEBUG(fmt, ...) 
#define MPR_DEBUG(module,fmt, ...) 
// there is no trace information when REDUCE_BIN leven bigger than LEVEL2
#elif defined(TUYA_REDUCE_BIN) && (TUYA_REDUCE_BIN >= TUYA_REDUCE_BIN_LEVEL2)
#define PR_TRACE(fmt, ...)
#define PR_TRACE_ENTER() 
#define PR_TRACE_LEAVE()
#define MPR_TRACE(module,fmt, ...) 
#define PR_DEBUG(fmt, ...)                   SAK_PRINT_DEBUG_LOG(fmt, ##__VA_ARGS__)
#define MPR_DEBUG(module,fmt, ...)           SAK_PRINT_LOG(module,TY_LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#else
#define PR_TRACE(fmt, ...)                   SAK_PRINT_TRACE_LOG(fmt, ##__VA_ARGS__)
#define PR_TRACE_ENTER()                     PR_TRACE("enter [%s]", (const char*)__func__)
#define PR_TRACE_LEAVE()                     PR_TRACE("leave [%s]", (const char*)__func__)
#define MPR_TRACE(module,fmt, ...)           SAK_PRINT_LOG(module,TY_LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)
#define PR_DEBUG(fmt, ...)                   SAK_PRINT_DEBUG_LOG(fmt, ##__VA_ARGS__)
#define MPR_DEBUG(module,fmt, ...)           SAK_PRINT_LOG(module,TY_LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#endif

// log interface for user
#define PR_ERR(fmt, ...)                     SAK_PRINT_ERR_LOG(fmt, ##__VA_ARGS__)
#define PR_WARN(fmt, ...)                    SAK_PRINT_WARN_LOG(fmt, ##__VA_ARGS__)
#define PR_NOTICE(fmt, ...)                  SAK_PRINT_NOTICE_LOG(fmt, ##__VA_ARGS__)
#define PR_INFO(fmt, ...)                    SAK_PRINT_INFO_LOG(fmt, ##__VA_ARGS__)

//used for auto test
#define PR_AUTOTEST PR_NOTICE

// set log level
#define SET_PR_DEBUG_LEVEL(level)            SetLogManageAttr(level)

// log output interface with module name
#define MPR_ERR(module,fmt, ...)             SAK_PRINT_LOG(module,TY_LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)
#define MPR_WARN(module,fmt, ...)            SAK_PRINT_LOG(module,TY_LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)
#define MPR_NOTICE(module,fmt, ...)          SAK_PRINT_LOG(module,TY_LOG_LEVEL_NOTICE, fmt, ##__VA_ARGS__)
#define MPR_INFO(module,fmt, ...)            SAK_PRINT_LOG(module,TY_LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

#if defined(TUYA_REDUCE_BIN) && (TUYA_REDUCE_BIN >= TUYA_REDUCE_BIN_LEVEL3)
#define PR_DEBUG_RAW(fmt, ...)
#else
#if defined(TUYA_IOT_DEBUG) && (TUYA_IOT_DEBUG==1)
#define PR_DEBUG_RAW(fmt, ...)               PrintLogRaw(fmt, ##__VA_ARGS__)
#else
#define PR_DEBUG_RAW(fmt, ...)
#endif
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
