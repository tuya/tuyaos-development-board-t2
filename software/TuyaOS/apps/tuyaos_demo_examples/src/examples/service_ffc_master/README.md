# FFC  简介

这个项目将会介绍使用 `tuyaos 3.0 ffc` 相关接口，配置模组 `master` 模式。 

* `FFC` 的 `master` 和 `slaver` 模式介绍

`master` 可以理解为信息发送端，就比如 `WIFI` 遥控器；`slaver` 可以理解为信息接收端，就比如被控设备。

* `WIFI` 遥控器 `master` 相关函数介绍：

此例程只介绍如何配置模组的 `master` 模式。

配置 `master` 模式，要首先调用 `tuya_iot_wifi_ffc_init` 函数初始化 `master` 模式。随后需要调用 `tuya_iot_wifi_ffc_control` 函数，设置 `master` 的类型。
最后再次调用 `tuya_iot_wifi_ffc_control` 函数配置使其工作在信道扫描状态下。

`tuya_iot_wifi_ffc_control` 参数介绍如下：

![FFC-master.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/1657244778074b8b83e03.png)

`tuya_iot_wifi_ffc_bind` 函数作用是开始绑定流程。一般的 `master` 设备会长按某个键进入配网流程时才会调用这个函数。而 一般的 `slaver` 设备是重新上电后就会进入配网流程。

`tuya_iot_wifi_ffc_send` 调用这个函数发送数据。`tuyaos` 内部会对这部分数据进行封装，我们只需要关心应用部分的协议内容，如应用层的控制协议。 `Slaver` 收到后通知数据接收回调函数。

## 运行结果

长按配网键3s，遥控器进入配网模式。

```C
[07-08 10:25:00 TUYA D][lr:0x4bd2d] ---------------start bind----------------
[07-08 10:25:00 TUYA D][lr:0x4bbf3] -------------------ffc status callback-------------
[07-08 10:25:00 TUYA D][lr:0x4bbfb] FFC status : 2
```

同时，另外一个设备烧录 `ffc_slaver` 的示例代码，重新上电，使 `slaver` 设备也进入配网模式。

配网成功。

```C
[07-08 10:25:00 TUYA D][lr:0x4bbf3] -------------------ffc status callback-------------
[07-08 10:25:00 TUYA D][lr:0x4bbfb] FFC status : 3
```

配网结束。

```C
[07-08 10:25:00 TUYA D][lr:0x4bbf3] -------------------ffc status callback-------------
[07-08 10:25:00 TUYA D][lr:0x4bbfb] FFC status : 4
```

使用 `tuyaos ffc` 组件配置 `master` 模式的时候，需要更换 `WIFI` 遥控器的开发环境，否则不能和 `slaver` 设备正常配对。`WIFI` 遥控器的开发环境，开发者可向产品经理获取。
