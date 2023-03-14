# 蓝牙遥控器简介

这个项目将会介绍如何打开模组（被控方）的蓝牙（beacon）遥控器功能，能够使设备接收到蓝牙 beacon 数据，从而使开发的设备可以被蓝牙（beacon）遥控器进行控制。

## 蓝牙 beacon 遥控器简介

蓝牙 beacon 是基于蓝牙 BLE 技术的一种应用，是蓝牙 BLE 的诸多应用之一。BLE 的通信包括两个主要部分：advertising（广播）和connecting（连接），Beacon 设备只使用了广播通信信道，不和任何低功耗蓝牙主机进行连接，正如 beacon（信标、灯塔）的字面意思。当你在 beacon 遥控器上按下按键后 beacon 遥控器便会发送一个对应的数据包。

## 蓝牙遥控器相关函数介绍

---

```c
//callback function for advertisement scanning data processing
typedef VOID (*TUYA_BLE_APP_SCAN_HANDLE)(UCHAR_T *data, UCHAR_T len,  UCHAR_T type, UCHAR_T* mac);

OPERATE_RET tuya_ble_reg_app_scan_adv_cb(TUYA_BLE_APP_SCAN_HANDLE cb);
```

> 蓝牙 beacon 数据接收回调注册函数。

**Parameters:**

+ <font color="#dd0000">`cb`:</font> 注册的蓝牙 beacon 接收数据回调函数。接收到的 beacon 数据会通过注册的回调函数传出。

**Return:**

+ <font color="#dd0000">`OPRT_OK`:</font>  函数执行成功。
+ <font color="#dd0000">`other`:</font>  函数执行失败。

---

```c
typedef struct {
    BLE_SCAN_ADV_BIND_CHECK_CB       bind_check;
    BLE_SCAN_ADV_BIND_RSLT_NOTIFY_CB bind_notify;
} TUYA_BLE_SCAN_ADV_HANDLE_CBS;

/**
 * @brief Register callback function for advertisement scanning data processing
 *
 * @param[in] cbs: callback function
 *
 * @note This API is used for registering callback function for advertisement scanning data processing
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_ble_reg_app_scan_adv_handle_cbs(TUYA_BLE_SCAN_ADV_HANDLE_CBS* cbs);
```

> 注册蓝牙遥控器绑定检查，绑定结果通知函数。通过注册的两个函数可以进行蓝牙遥控器绑定判断和得到是否成功绑定的结果。

**Parameters:**

+ <font color="#dd0000">`cbs`:</font> 绑定检查，绑定结果通知函数结构体。

**Return:**

+ <font color="#dd0000">`OPRT_OK`:</font>  函数执行成功。
+ <font color="#dd0000">`other`:</font>  函数执行失败。

---

```c
/**
 * @brief Set timeout window for binding remote controller
 *
 * @param[in] time_out: time out in second, 0: no window
 *
 * @note This API is used for setting timeout window for binding remote controller
 *
 * @return VOID
 */
VOID_T tuya_ble_set_bind_window(UINT_T time_out);
```

> 蓝牙绑定窗口时间设置函数。该函数也可以不调用，默认绑定窗口时间为 30s。 

**Parameters:**

+ <font color="#dd0000">`time_out`:</font> 绑定窗口时间。

**Return:**

+ <font color="#dd0000">`none`</font> 

---

```c
VOID_T tuya_ble_open_bind_window(VOID_T);
```

> 打开蓝牙绑定窗口。

**Parameters:**

+ <font color="#dd0000">`none`</font> 

**Return:**

+ <font color="#dd0000">`none`</font> 

---

## 运行结果

设备连接 `IOT` 平台，设备重新上电，进入蓝牙遥控器配对模式。

同时长按蓝牙遥控器的分组键，（如果你的蓝牙遥控器没有分组键，那么开关键就是默认的分组1），蓝牙遥控器开始和设备配对。

```C
[07-08 16:53:27 TUYA D][lr:0x4babb] ----------------scan adv bind check cb-----------------
[07-08 16:53:27 TUYA D][lr:0x4bac3] bt type : 1
[07-08 16:53:27 TUYA D][lr:0x4bad3] data[0] : 11
[07-08 16:53:27 TUYA D][lr:0x4bad3] data[1] : 7
[07-08 16:53:27 TUYA D][lr:0x4bad3] data[2] : 1
[07-08 16:53:27 TUYA D][lr:0x4bad3] data[3] : 255
[07-08 16:53:27 TUYA D][lr:0x4bad3] data[4] : 2
[07-08 16:53:27 TUYA D][lr:0x4bad3] data[5] : 1
[07-08 16:53:27 TUYA D][lr:0x4bad3] data[6] : 255
[07-08 16:53:27 TUYA D][lr:0x4bad3] data[7] : 221
[07-08 16:53:27 TUYA D][lr:0x4bad3] data[8] : 171
[07-08 16:53:27 TUYA D][lr:0x4bad3] data[9] : 0
[07-08 16:53:27 TUYA D][lr:0x4bad3] data[10] : 177
[07-08 16:53:27 TUYA D][lr:0x4bad3] data[11] : 144
```

蓝牙状态发送变化，进入蓝牙事件回调函数中。

```c
[07-08 16:53:28 TUYA D][lr:0x4baf5] ------------------ble scan adv bind notify cb----------------
[07-08 16:53:28 TUYA D][lr:0x4bafd] bt type : 1
[07-08 16:53:28 TUYA D][lr:0x4bb05] bt rslt : 0
```

按下蓝牙遥控器的控制键。进入数据处理回调函数中。

```c
[07-08 16:57:41 TUYA D][lr:0x4bbb3] ble remote(new) send data len: 9
[07-08 16:57:41 TUYA D][lr:0x4bbc3] data[0] : 11
[07-08 16:57:41 TUYA D][lr:0x4bbc3] data[1] : 7
[07-08 16:57:41 TUYA D][lr:0x4bbc3] data[2] : 1
[07-08 16:57:41 TUYA D][lr:0x4bbc3] data[3] : 1
[07-08 16:57:41 TUYA D][lr:0x4bbc3] data[4] : 11
[07-08 16:57:41 TUYA D][lr:0x4bbc3] data[5] : 2
[07-08 16:57:41 TUYA D][lr:0x4bbc3] data[6] : 20
[07-08 16:57:41 TUYA D][lr:0x4bbc3] data[7] : 0
[07-08 16:57:41 TUYA D][lr:0x4bbc3] data[8] : 0
```

同时长按蓝牙遥控器的亮度-和配对时的分组键，开始解绑流程。

设备解绑，状态发生变化，进入蓝牙事件回调函数中。

```c
[07-08 16:58:52 TUYA D][lr:0x4baf5] ------------------ble scan adv bind notify cb----------------
[07-08 16:58:52 TUYA D][lr:0x4bafd] bt type : 0
[07-08 16:58:52 TUYA D][lr:0x4bb05] bt rslt : 0
```
