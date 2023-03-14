

# WIFI AP

##  简介

这个项目将会介绍如何使用 `tuyaos 3 wifi ap ` 相关接口，开启 `AP` 模式，随后手机等设备可以作为 `STATION` 直接连接 `AP` ，方便对用户设备进行控制。

![wifi sta and ap.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/1655624853b5f412495be.png)

* `WiFi AP` 相关介绍

  1.`AP` 一个无线网络的创作者，是网络的中心。一般生活中的路由器就是一个 `AP`。

  2.此例程中设置的 `WiFi` 模式为 `WWM_SOFTAP`，即软 `AP`。软 `AP` 通过驱动程序使其提供和传统 `AP` 一样的信号转换，路由等功能。在基本功能上，软 `AP` 和传统 `AP`，没有太大差别。只不过软 `AP` 使用软件来实现 `AP` 功能，但是接入能力和覆盖范围远不如传统 `AP`。

`AP` 模式参数配置介绍：

|参数|介绍|
|-|-|
|ssid |WiFi 用户名 |
|passwd|WiFi 密码|
|s_len|用户名长度|
|p_len|密码长度|
|chan|信道|
|md|环境类型|
|ip|网络配置|
|ms_interval|广播间隔|
|max_conn|站点最大连接数|

* `WiFi` 环境类型介绍

|类型|介绍|
|-|-|
|WAAM_OPEN|无需密码就可以连接|
|WAAM_WEP|初代的 `WiFi` 加密，使用 `CRC` 校验，加密方式已被破解，此类型非常不安全。|
|WAAM_WPA_PSK|`WPA` 在不升级硬件的情况下，使用 `TKIP` 协议来增强 `WEP` 的安全性。`WPA-PSK` 采用 `WPA2` 算法与 `STATION` 进行4次握手操作。握手成功才可以连接。|
|WAAM_WPA2_PSK|与 `WAAM_WPA_PSK` 类似，只不过使用的是 `WPA2` 加密算法。此加密方式最常用。|
|WAAM_WPA_WPA2_PSK|最安全的加密类型，需要安装 `Radius` 服务器，因此一般用户不会用到，只有企业为了无线更加安全才会使用此加密方式。|
|WAAM_WPA_WPA3_SAE|`WPA` 第3代加密算法，采用 `SAE` 握手。|

## 流程介绍
相关函数目录：
`software/TuyaOS/include/components/tal_wifi/include/tal_wifi.h`

![wifi ap process draw .png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/1655625747f141a352402.png)

## 运行结果
`WiFi AP` 模式相关信息打印。
```C
ap net info ip: 192.168.137.02, mask: 255.255.255.0, gw: 192.168.1.1
ssid:test_wifi, key:12345678, channel: 5
```
打开手机连接 `AP`。
```C
[15:11:45.937]收←◆hapd_intf_sta_add:1, vif:0
rc_init: station_id=0 format_mod=0 pre_type=0 short_gi=0 max_bw=0
rc_init: nss_max=0 mcs_max=255 r_idx_min=0 r_idx_max=11 no_samples=10
ctrl_port_hdl:1
```
打开手机断开 `AP`。
```C
STA_REMOVE
ap_free_sta
```

## 技术支持

您可以通过以下方法获得涂鸦的支持:

- TuyaOS 论坛： https://www.tuyaos.com

- 开发者中心： https://developer.tuya.com

- 帮助中心： https://support.tuya.com/help

- 技术支持工单中心： https://service.console.tuya.com
