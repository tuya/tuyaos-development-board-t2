#ifndef __TUYA_DEVOS_DATA_H__
#define __TUYA_DEVOS_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "gw_intf.h"
#include "tal_time_service.h"
#include "tal_system.h"
#include "tuya_svc_netmgr.h"

typedef BYTE_T  TY_DEVOS_DATA_TYPE;
/*配网数据*/
#define TY_DEVOS_NET_CONFIG_DATA            (1)
/*连网数据*/
#define TY_DEVOS_NET_CONNECT_DATA           (2)
/*设备激活数据*/
#define TY_DEVOS_DEV_ACTIVATE_DATA          (3)
/*设备复位数据*/
#define TY_DEVOS_RESET_DATA                 (4)
/*设备重启数据*/
#define TY_DEVOS_REBOOT_DATA                (5)
/*设备状态数据*/
#define TY_DEVOS_STATUS_DATA              (6)
/*设备连接mqtt数据*/
#define TY_DEVOS_LINK_MQTT_DATA             (7)
/*设备连接http数据*/
#define TY_DEVOS_LINK_HTTP_DATA             (8)
/*设备连接transport数据*/
#define TY_DEVOS_LINK_TRANSPORT_DATA             (9)
/*设备端应用数据*/
#define TY_DEVOS_DEV_APP_DATA               (10)

/*ty reset type*/
#define TY_DATA_RESET_LOCAL     (1)
#define TY_DATA_RESET_CLOUD     (2)
#define TY_DATA_RESET_FAST      (3)
#define TY_DATA_RESET_BLE       (4)
#define TY_DATA_RESET_FLASH     (5)
#define TY_DATA_RESET_TEST      (6)
#define TY_DATA_RESET_HEALTH    (7)

#define TY_DATA_RESET_NAME_STRING   "reset"
#define TY_DATA_REBOOT_NAME_STRING  "reboot"
#define TY_DATA_STATUS_NAME_STRING  "status"

#define TY_DATA_STATUS_WIFI         (10)
#define TY_DATA_LINK_MQTT_NAME_STRING  "mqtt"
#define TY_DATA_LINK_HTTP_NAME_STRING  "http"

/*连接数据类型 tp*/
#define TY_DATA_LINK_DNS                (1)
#define TY_DATA_LINK_TCP_CONNECT        (2)
#define TY_DATA_LINK_TLS_CONNECT        (3)
#define TY_DATA_LINK_TCP_SEND           (4)
#define TY_DATA_MQTT_TCP_RECV           (5)
#define TY_DATA_MQTT_PING               (6)
#define TY_DATA_LINK_TRANSPORTER_CONNECT    (7)
/*连接数据域名编码*/

/*配网步骤*/
#define TY_DATA_NETCFG_STEP1        (1)
#define TY_DATA_NETCFG_STEP2        (2)
#define TY_DATA_NETCFG_STEP3        (3)
#define TY_DATA_NETCFG_STEP4        (4)
#define TY_DATA_NETCFG_STEP5        (5)
#define TY_DATA_NETCFG_STEP6        (6)
#define TY_DATA_NETCFG_STEP7        (7)
#define TY_DATA_NETCFG_STEP8        (8)

/*net connect type*/
#define TY_DATA_NETCONNECT_TYPE_FAST     (0)
#define TY_DATA_NETCONNECT_TYPE_NORMAL   (1)
#define TY_DATA_NETCONNECT_TYPE_UNKOWN   (2)

/*net connect result code*/
#define TY_DATA_NETCONNECT_RESULT_CONNECT_DONE   (10)

/*activate type*/
#define TY_DATA_DEV_ACTIVATE_URL_CONFIG         (1)
#define TY_DATA_DEV_ACTIVATE_ACTIVE             (2)
#define TY_DATA_DEV_HTTP_POST                   (3)

#if defined ENABLE_DEVICE_DATA_COLLECT && (ENABLE_DEVICE_DATA_COLLECT==1)

void remove_ty_device_data_file();

/**
 * @brief tuya Device OS 本地数据采集 更新复位数据
 *
 * @param[in] ts: 数据时间戳
 * @param[in] dt: 数据类型
 * @param[in] source: 产生复位的业务
 * @param[in] stat: wifi联网状态
 * @param[in] free_mem: 剩余内存
 * @param[in] rssi: 信号强度
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档
 */
OPERATE_RET ty_devos_update_reset_data(TIME_T ts, TY_DEVOS_DATA_TYPE dt, const char* source, BYTE_T stat, int free_mem, int rssi);




/**
 * @brief tuya Device OS 本地数据采集 更新接口
 *
 * @param[in] dt: 数据类型
 * @param[in] dataStringToWrite: 待写入的字符串，json格式
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档
 */
OPERATE_RET ty_devos_data_update(TY_DEVOS_DATA_TYPE dt, const char* dataStringToWrite);



/**
 * @brief tuya Device OS 本地数据采集 数据上传接口
 *
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档
 */
OPERATE_RET ty_devos_data_upload(TY_DEVOS_DATA_TYPE dt);


/**
 * @brief tuya Device OS 本地数据采集 更新复位，重启和状态数据
 *
 * @param[in] ts: 数据时间戳
 * @param[in] dt: 数据类型
 * @param[in] source: 产生复位的业务
 * @param[in] stat: 联网状态
 * @param[in] free_mem: 剩余内存
 * @param[in] rssi: 信号强度
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档
 */
OPERATE_RET ty_devos_update_status_data(TIME_T ts, TY_DEVOS_DATA_TYPE dt, int free_mem, int source, BYTE_T stat, int rssi);

/**
 * @brief tuya Device OS 本地数据采集 更新复位，重启和状态数据
 *
 * @param[in] ts: 数据时间戳
 * @param[in] type: 数据类型
 * @param[in] url: http请求或mqtt 的url
 * @param[in] duration: 连接时长
  * @param[in] times: 重试次数
 * @param[in] rssi: 信号强度
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档
 */
OPERATE_RET ty_devos_update_link_data(TIME_T ts, TY_DEVOS_DATA_TYPE dt, int type, const char * url, int duration, int times, int rssi);

/**
 * @brief tuya Device OS 本地数据采集 更新配网数据
 *
 * @param[in] ts: 数据时间戳
 * @param[in] type: 数据类型
 * @param[in] mode: 配网模式
 * @param[in] strategy: 配网策略
 * @param[in] duration: 连接时长
 * @param[in] steps: 配网步骤
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档
 */
OPERATE_RET ty_devos_update_netcfg_data(TIME_T ts, TY_DEVOS_DATA_TYPE dt, uint8_t type, uint8_t mode, uint8_t strategy, uint8_t steps, TIME_T du_current);


/**
 * @brief tuya Device OS 本地数据采集 更新连网数据
 *
 * @param[in] ts: 数据时间戳
 * @param[in] dt: 数据类型
 * @param[in] type: 连接类型，快连或慢连
 * @param[in] du_current: 当前时间戳
 * @param[in] result_code: 连接结果
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档
 */
OPERATE_RET ty_devos_update_netconnect_data(TIME_T ts, TY_DEVOS_DATA_TYPE dt, uint8_t type, TIME_T du_current, int8_t result_code);

/**
 * @brief tuya Device OS 本地数据采集 更新激活数据
 *
 * @param[in] ts: 数据时间戳
 * @param[in] dt: 数据类型
 * @param[in] type: 连接类型，快连或慢连
 * @param[in] du_current: 当前时间戳
 * @param[in] result_code: 连接结果
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档
 */
OPERATE_RET ty_devos_update_activate_data(TIME_T ts, TY_DEVOS_DATA_TYPE dt, uint8_t type, TIME_T du_current, int result_code);


/**
 * @brief tuya Device OS 本地数据采集 更新应用数据
 *
 * @param[in] dataJson: json格式数据
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档
 */
OPERATE_RET ty_devos_update_app_data(ty_cJSON * dataJson);


#define CAPTURE_RESET_DATA(source)                                                                                                                                \
    do                                                                                                                                                            \
    {                                                                                                                                                             \
        OPERATE_RET rt = OPRT_OK;                                                                                                                                 \
        BYTE_T cur_gw_nw_status;                                                                                                                       \
        tuya_svc_netmgr_cfg_get(LINKAGE_CFG_STATUS, &cur_gw_nw_status);                                                                                                                   \
        SCHAR_T rssi = 0;                                                                                                                                         \
        tuya_svc_netmgr_cfg_get(LINKAGE_CFG_RSSI, &rssi);                                                                                                                       \
        TUYA_CALL_ERR_LOG(ty_devos_update_status_data(tal_time_get_posix(), TY_DEVOS_RESET_DATA, tal_system_get_free_heap_size(), source, cur_gw_nw_status, rssi)); \
    } while (0)
#define CAPTURE_REBOOT_DATA(source)                                                                                                                                \
    do                                                                                                                                                             \
    {                                                                                                                                                              \
        OPERATE_RET rt = OPRT_OK;                                                                                                                                  \
        BYTE_T cur_gw_nw_status;                                                                                                                        \
        tuya_svc_netmgr_cfg_get(LINKAGE_CFG_STATUS, &cur_gw_nw_status);                                                                                                                    \
        SCHAR_T rssi = 0;                                                                                                                                          \
        tuya_svc_netmgr_cfg_get(LINKAGE_CFG_RSSI, &rssi);                                                                                                                        \
        TUYA_CALL_ERR_LOG(ty_devos_update_status_data(tal_time_get_posix(), TY_DEVOS_REBOOT_DATA, tal_system_get_free_heap_size(), source, cur_gw_nw_status, rssi)); \
    } while (0)

#define CAPTURE_STATUS_DATA(source)                                                                                                                                \
    do                                                                                                                                                             \
    {                                                                                                                                                              \
        OPERATE_RET rt = OPRT_OK;                                                                                                                                  \
        BYTE_T cur_gw_nw_status;                                                                                                                        \
        tuya_svc_netmgr_cfg_get(LINKAGE_CFG_STATUS, &cur_gw_nw_status);                                                                                                                    \
        SCHAR_T rssi = 0;                                                                                                                                          \
        tuya_svc_netmgr_cfg_get(LINKAGE_CFG_RSSI, &rssi);                                                                                                                        \
        TUYA_CALL_ERR_LOG(ty_devos_update_status_data(tal_time_get_posix(), TY_DEVOS_STATUS_DATA, tal_system_get_free_heap_size(), source, cur_gw_nw_status, rssi)); \
    } while (0)

#define CAPTURE_LINK_MQTT_DATA(type, url, duration, times)                                                                             \
    do                                                                                                                                 \
    {                                                                                                                                  \
        OPERATE_RET rt = OPRT_OK;                                                                                                      \
        SCHAR_T rssi = 0;                                                                                                              \
        tuya_svc_netmgr_cfg_get(LINKAGE_CFG_RSSI, &rssi);                                                                                            \
        TUYA_CALL_ERR_LOG(ty_devos_update_link_data(tal_time_get_posix(), TY_DEVOS_LINK_MQTT_DATA, type, url, duration, times, rssi)); \
    } while (0)

#define CAPTURE_LINK_HTTP_DATA(type, url, duration, times)                                                                             \
    do                                                                                                                                 \
    {                                                                                                                                  \
        OPERATE_RET rt = OPRT_OK;                                                                                                      \
        SCHAR_T rssi = 0;                                                                                                              \
        tuya_svc_netmgr_cfg_get(LINKAGE_CFG_RSSI, &rssi);                                                                                            \
        TUYA_CALL_ERR_LOG(ty_devos_update_link_data(tal_time_get_posix(), TY_DEVOS_LINK_HTTP_DATA, type, url, duration, times, rssi)); \
    } while (0)

#define CAPTURE_LINK_TRANSPORT_DATA(type, url, duration, times)                                                                             \
        do                                                                                                                                 \
        {                                                                                                                                  \
            OPERATE_RET rt = OPRT_OK;                                                                                                      \
            SCHAR_T rssi = 0;                                                                                                              \
            tuya_svc_netmgr_cfg_get(LINKAGE_CFG_RSSI, &rssi);                                                                                            \
            TUYA_CALL_ERR_LOG(ty_devos_update_link_data(tal_time_get_posix(), TY_DEVOS_LINK_TRANSPORT_DATA, type, url, duration, times, rssi)); \
        } while (0)

#define CAPTURE_NETCONNECT_DATA(type, result_code)                                                                                                    \
    do                                                                                                                                                \
    {                                                                                                                                                 \
        OPERATE_RET rt = OPRT_OK;                                                                                                                     \
        TUYA_CALL_ERR_LOG(ty_devos_update_netconnect_data(tal_time_get_posix(), TY_DEVOS_NET_CONNECT_DATA, type, tal_time_get_posix(), result_code)); \
    } while (0)

#define CAPTURE_ACTIVATE_DATA(type, result_code)                                                                                                     \
    do                                                                                                                                               \
    {                                                                                                                                                \
        OPERATE_RET rt = OPRT_OK;                                                                                                                    \
        TUYA_CALL_ERR_LOG(ty_devos_update_activate_data(tal_time_get_posix(), TY_DEVOS_DEV_ACTIVATE_DATA, type, tal_time_get_posix(), result_code)); \
    } while (0)

#else
#define CAPTURE_RESET_DATA(source)   do { \
}while (0)
#define CAPTURE_REBOOT_DATA(source)  do{    \
}while (0)

#define CAPTURE_STATUS_DATA(source)   do { \
}while (0)

#define CAPTURE_LINK_MQTT_DATA(type, url, duration, times)   do { \
}while (0)

#define CAPTURE_LINK_HTTP_DATA(type, url, duration, times)   do { \
}while (0)

#define CAPTURE_LINK_TRANSPORT_DATA(type, url, duration, times)   do { \
}while (0)

#define CAPTURE_NETCONNECT_DATA(type,result_code)   do { \
}while (0)

#define CAPTURE_ACTIVATE_DATA(type,result_code)   do { \
}while (0)

#endif


#ifdef __cplusplus
}
#endif

#endif

