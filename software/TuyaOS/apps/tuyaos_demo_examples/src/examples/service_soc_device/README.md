# SoC 初始简介

该示例介绍了设备的配网、移除和五种类型 DP 点的上报。在示例中的数据上报部分会有一些字符串和十六进制的数组，那些是按照云端规定的灯的协议格式的数据，开发者在这里不用过于关心数据代表的具体含义。

## 简介

这个函数将会介绍 `tuya_iot_wf_soc_dev_init` 的使用方法。

* ` tuya_iot_wf_soc_dev_init ` 介绍

`tuya_iot_wf_soc_dev_init` 此接口是 `TuyaOS` 应用产品的初始化接口，通过开发者传递过来的 `PID` 、版本信息等到到涂鸦云激活，并向云端获取对应的 `PID` 模型。此接口还可以通过注册回调函数的方式通知应用，发生了哪些事件。此函数接口函数众多，接下来会逐一解释每个参数的含义。

* `GW_WF_CFG_MTHD_SEL cfg` `WiFi` 设备工作模式

![iot_soc_init-IOT工作模式.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/16575052456589f029748.png)

* `GW_WF_START_MODE start_mode` Wi-Fi 设备配网模式

![iot_soc_init.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/1657502479e374f463a03.png)

* `TY_IOT_CBS_S ` 回调函数集合

![iot_soc_init-IOT 回调函数.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/165750564213806fa38fa.png)

## 流程介绍

相关函数目录：
`software/TuyaOS/include/components/svc_api_wifi/include/tuya_iot_wifi_api.h`

![iot_soc_init-流程.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/165750589588c5205fe27.png)

## 运行结果

如果设备之前没有进行过配网操作，可以调用 `example_soc_init` 命令使设备会进入配网模式；如果之前设备已经配网成功，那么设备会去自动连接之前成功连接过的网络。您也可以使用 `example_soc_remove` 命令移除设备， 由于移除设后会重启设备，所以需要您在移除后调用 `example_soc_init` 命令使设备重新进入配网进入配网模式。

DP 上报后可以在面板中看上报的现象，其中 raw 类型的上报现象可以依次点击右下角四叶草（设置）->默认灯光，会看到每执行一次 `example_dp_raw` 命令用户定制的颜色就会变一下。

## 技术支持

您可以通过以下方法获得涂鸦的支持:

- TuyaOS 论坛： https://www.tuyaos.com

- 开发者中心： https://developer.tuya.com

- 帮助中心： https://support.tuya.com/help

- 技术支持工单中心： https://service.console.tuya.com
