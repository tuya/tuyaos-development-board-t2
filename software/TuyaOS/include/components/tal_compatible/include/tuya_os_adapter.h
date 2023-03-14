/**
* @file tuya_os_adapter.h
* @brief Common process - Initialization
* @version 0.1
* @date 2020-11-09
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/
#ifndef _TUYA_OS_ADAPTER_H
#define _TUYA_OS_ADAPTER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "tuya_cloud_types.h"
#include "tuya_hal_mutex.h"
#include "tuya_hal_semaphore.h"
#include "tuya_hal_system.h"
#include "tkl_init.h"
#include "tuya_cloud_error_code.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(WIFI_GW) && (WIFI_GW==1)

#include "tal_wifi.h"
#include "ty_wifi_mgnt.h"
#include "wf_basic_intf.h"

#define WWM_LOWPOWER    WWM_POWERDOWN   
typedef FAST_WF_CONNECTED_AP_INFO_T FAST_WF_CONNECTED_AP_INFO_V2_S;

#define OPRT_SVC_WIFI_RECV_CONTINUE                        (-0x0b00)  //-2816, 继续接收配网包
#define OPRT_SVC_WIFI_DONOT_FOUND_MODULE                   (-0x0b01)  //-2817, 模块未找到
#define OPRT_SVC_WIFI_PEGASUS_DECODE_FAILED                (-0x0b02)  //-2818, 闪电包解析错误
#define OPRT_SVC_WIFI_NEED_FACTORY_RESET                   (-0x0b03)  //-2819, wifi初始化配置校验失败，需要恢复出厂设置

#endif

#define TY_MAC_ADDR_LEN                                    MAC_ADDR_LEN  

#define OPRT_DEVICE_IS_SUB_NODE_OR_IN_ELECTION             (-2016)

/**
* @brief Get version of platform
*
* @param VOID
*
* @note This API is used to get the version of platform.
*
* @return the version of platform
*/
#define tuya_os_adapter_get_platform_ver() tkl_get_version()

#ifdef __cplusplus
}
#endif

#endif /* _TUYA_OS_ADAPTER_H */
