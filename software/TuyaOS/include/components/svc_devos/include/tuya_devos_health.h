/**
 * @file tuya_devos_health.h
 * @author fangweng.hu@tuya.com
 * @brief Tuya Device OS设备健康监控
 * @version 0.2
 * @date 2022-03-18
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef __TUYA_DEVOS_HEALTH_H__
#define __TUYA_DEVOS_HEALTH_H__

#include "tuya_cloud_com_defs.h"
#include "tuya_hal_thread.h"
#include "tal_mutex.h"
#include "uni_pointer.h"

#ifdef __cplusplus
extern "C" {
#endif

// 默认health monitor检测时间间隔（暂定）
#define HEALTH_SLEEP_INTERVAL               (5)
// 默认系统上报健康状态时间
#define HEALTH_REPORT_INTERVAL   (60 * 60)
// 默认系统最小free内存门限设置为5K，正常访问云端、FLASH需要4K内存以上（暂定）
#define HEALTH_FREE_MEM_THRESHOLD           (1024*8)
// 默认系统最小内存块门限设置为5K，正常访问云端、FLASH需要一次性分配4K内存以上（暂定）
#define HEALTH_LARGEST_MEM_BLOK_THRESHOLD   (1024*5)
// 默认系统HTTP访问拒绝、sign校验错误次数
#define HEALTH_HTTP_FAILED_THRESHOLD   (5)

// 默认系统最大workq depth
#define HEALTH_WORKQ_THRESHOLD        (50)
// 默认系统最大msgq num
#define HEALTH_MSGQ_THRESHOLD         (50)
// 默认系统最大timeq num
#define HEALTH_TIMEQ_THRESHOLD        (100)

//health指标,必须在g_health_policy顺序定义,不然global type重新分配会不准
typedef INT_T HEALTH_MONITOR_RULE_E;
#define HEALTH_RULE_FREE_MEM_SIZE 0
#define HEALTH_RULE_MAX_MEM_SIZE 1
#define HEALTH_RULE_ATOP_REFUSE 2
#define HEALTH_RULE_ATOP_SIGN_FAILED 3
#define HEALTH_RULE_WORKQ_DEPTH 4
#define HEALTH_RULE_MSGQ_NUM 5
#define HEALTH_RULE_TIMER_NUM 6
#define HEALTH_RULE_FEED_WATCH_DOG 7
#define HEALTH_RULE_RUNTIME_REPT 8

typedef VOID (*health_notify_cb)();
typedef BOOL_T(*health_query_cb)();

typedef struct {
    INT_T  type;                   // 检测指标
    UINT_T threshold;              // 门限(发生次数)
    UINT_T detect_period;          // 检测周期
    health_query_cb query_cb;      // 指标查询回调,如果事件型可填NULL
    health_notify_cb notify_cb;    // 指标通知回调
} devos_health_policy_t;

typedef struct {
    INT_T type;
} health_alert_evt_data;

/**
 * @brief devos health init function
 *
 * @param[in] watch_dog_interval: 看门狗的默认时间，单位是秒，必须是20秒钟的倍数
 * @param[in] monitor_detect_interval: 健康监控扫描间隔，单位是秒，必须是20秒钟的倍数
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
INT_T tuya_devos_health_init_and_start(CONST INT_T watch_dog_interval, CONST INT_T monitor_detect_interval);

/**
 * @brief add health item
 *
 * @param[in] threshold threshold
 * @param[in] period period
 * @param[in] query query cb
 * @param[in] notify notify cb
 *
 * @return type id, success when large than 0,others failed
 */
INT_T tuya_devos_add_health_item(UINT_T threshold, UINT_T period, health_query_cb query, health_notify_cb notify);

/**
 * @brief delete health item
 *
 * @param[in] type type
 *
 */
VOID tuya_devos_delete_health_item(INT_T type);

/**
 * @brief update health item period
 *
 * @param[in] type type
 * @param[in] period period
 *
 */
VOID tuya_devos_update_health_item_period(INT_T type, UINT_T period);

/**
 * @brief update health item threshold
 *
 * @param[in] type type
 * @param[in] threshold threshold
 *
 */
VOID tuya_devos_update_health_item_threshold(INT_T type, UINT_T threshold);

/**
 * @brief dump health item
 *
 */
VOID tuya_devos_dump_health_item(VOID);

#ifdef __cplusplus
}
#endif

#endif
