

# FFC  简介

这个项目将会介绍使用 `tuyaos 3.0 ffc` 相关接口，配置模组 `slaver` 模式。

* `FFC` 简介

`FFC` 是 `WIFI` 遥控器通信技术的简称。`FFC` 的产品需求最早来自于终端部分用户习惯于传统遥控器，来控制智能家居。

`WIFI` 遥控器经过不断发展，主要发展为本地遥控器（离线遥控器）和在线遥控器，两者的最大差别就是是否有涂鸦云端的参与。

* `WIFI` 遥控器通讯协议及实现原理介绍

`FFC` 是一种无需通过路由器连接即可实现 `WIFI` 设备之间通信的技术，主要是对无线管理帧（802.11）中的 `Probe request` 帧进行了扩展。利用了`Probe request` 帧 `Frame body` 中的 `Vendor Specific`（221 厂商自定义元素字段）可以自由编辑且不定长度的特性，自定义了一层通信协议，`Probe request` 帧可以简单的理解为探测请求，扫描区域内有哪些802.11网络。通信协议如下图所示：

![FFC-ffc通讯协议.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/1657184597c35e6ebe40e.png)

下图为一帧 `Probe reques` 数据

![FFC- 具体数据分析  12138  123.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/1657181638299b39cd1e7.png)


正常情况下的配对流程如下：

![FFC-配网具体实现 12138 123.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/1657180435b86eae4ee83.png)

* `WIFI` 遥控器 `slaver` 相关函数介绍：

此例程只介绍如何配置模组的 `slave` 模式。

`tuyaos` 内部帮我们配置好了，在应用层我们先需要调用`tuya_iot_wifi_ffc_init` 函数初始化遥控器相关配置，随后调用
`tuya_iot_wifi_ffc_control` 使其工作在扫描信道模式下。最后调用`tuya_iot_wifi_ffc_bind` 函数开始绑定流程即可。

`tuya_iot_wifi_ffc_init` 的形参介绍如下：

![FFC-tuya_iot_wifi_ffc_init 参数介绍 12138  .png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/1657182377632954a6d88.png)

`tuya_iot_wifi_ffc_unbind` 为遥控器接绑函数，在线遥控器无需关心。

`tuya_iot_wifi_ffc_send` 在模组为 `master` 的情况下才会用到，这里不过多介绍。

## 流程介绍

相关函数目录：
`software/TuyaOS/include/components/svc_wifi_ffc/include/ffc_app.h`

![ffc_流程介绍.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/165718318865628822965.png)

## 运行结果

此例程使用的是在线遥控器，所以待配对设备需要联网。

待配对设备重新上电后会进入配对流程。

设备 `ffc` 初始化。

```C
[01-01 08:00:01 TUYA D][lr:0x4bab7] -------------------ffc status callback-------------
[01-01 08:00:01 TUYA D][lr:0x4bac1] FFC status : 0
```
设备开始 `ffc `配对。

```C
[01-01 08:00:01 TUYA D][lr:0x4bab7] -------------------ffc status callback-------------
[01-01 08:00:01 TUYA D][lr:0x4bac1] FFC status : 2
```

长按 `WIFI` 遥控器的配网键（一般为 `ON` 键）。此时 `WIFI` 遥控器会与待配对设备开始配对。

配对成功。

```C
[01-01 08:00:01 TUYA D][lr:0x4bab7] -------------------ffc status callback-------------
[01-01 08:00:01 TUYA D][lr:0x4bac1] FFC status : 3

```

配对结束。

```C
[01-01 08:00:01 TUYA D][lr:0x4bab7] -------------------ffc status callback-------------
[01-01 08:00:01 TUYA D][lr:0x4bac1] FFC status : 4

```

按下 `WIFI` 遥控器的 `ON` 键。

```C
[07-07 16:47:30 TUYA D][lr:0x9b87f] ffc forward timer random 37ms, packet type -> cmd
[07-07 16:47:30 TUYA D][lr:0x4bafb] -------------------ffc receive callback-------------
[07-07 16:47:30 TUYA D][lr:0x4bb09] data_cmd [0] : 255
[07-07 16:47:30 TUYA D][lr:0x4bb09] data_cmd [1] : 1
[07-07 16:47:30 TUYA D][lr:0x4bb09] data_cmd [2] : 1
[07-07 16:47:30 TUYA D][lr:0x9ab2b] ffc slaver probe request send
[07-07 16:47:30 TUYA D][lr:0x9ab55] ffc slaver probe request finish
```

按下 `WIFI` 遥控器的 `OFF` 键。

```c
[07-07 16:48:00 TUYA D][lr:0x9b87f] ffc forward timer random 29ms, packet type -> cmd
[07-07 16:48:00 TUYA D][lr:0x4bafb] -------------------ffc receive callback-------------
[07-07 16:48:00 TUYA D][lr:0x4bb09] data_cmd [0] : 255
[07-07 16:48:00 TUYA D][lr:0x4bb09] data_cmd [1] : 1
[07-07 16:48:00 TUYA D][lr:0x4bb09] data_cmd [2] : 0
[07-07 16:48:00 TUYA D][lr:0x9ab2b] ffc slaver probe request send
[07-07 16:48:01 TUYA D][lr:0x9ab55] ffc slaver probe request finish
```

按下 `WIFI` 遥控器的亮度减少键。

```C
[07-07 16:48:41 TUYA D][lr:0x9b87f] ffc forward timer random 23ms, packet type -> cmd
[07-07 16:48:41 TUYA D][lr:0x4bafb] -------------------ffc receive callback-------------
[07-07 16:48:42 TUYA D][lr:0x4bb09] data_cmd [0] : 1
[07-07 16:48:42 TUYA D][lr:0x4bb09] data_cmd [1] : 9
[07-07 16:48:42 TUYA D][lr:0x4bb09] data_cmd [2] : 0
[07-07 16:48:42 TUYA D][lr:0x9ab2b] ffc slaver probe request send
[07-07 16:48:42 TUYA D][lr:0x9ab55] ffc slaver probe request finish
```

按下 `WIFI` 遥控器的亮度增加键。

```c
[07-07 16:49:08 TUYA N][lr:0x9b6c5] ffc cmd packet sequence 53973 is new, master mac[68:57:2d:3c:98:ea]
[07-07 16:49:08 TUYA D][lr:0x9b87f] ffc forward timer random 26ms, packet type -> cmd
[07-07 16:49:08 TUYA D][lr:0x4bafb] -------------------ffc receive callback-------------
[07-07 16:49:08 TUYA D][lr:0x4bb09] data_cmd [0] : 1
[07-07 16:49:08 TUYA D][lr:0x4bb09] data_cmd [1] : 9
[07-07 16:49:08 TUYA D][lr:0x4bb09] data_cmd [2] : 1
[07-07 16:49:08 TUYA D][lr:0x9ab2b] ffc slaver probe request send
[07-07 16:49:08 TUYA D][lr:0x9ab55] ffc slaver probe request finish
```
