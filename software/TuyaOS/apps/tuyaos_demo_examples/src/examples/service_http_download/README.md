

# HTTP FILE DOWNLOAD 简介

这个项目将会介绍使用 `http` 文件下载接口进行文件下载。

* `http_gw_download_file` 简介

`OTA` 升级中调用的就是这个接口，现在 `tuyaos` 把这个接口公开了出来。

可使用此函数进行文件下载。但是此函数的两个参数 `total_len`（文件大小） 和 `file_hmac`（密钥散列消息认证码）需要云端下发到设备，可以使用 `DP` 点下发这两个数据，也可以使用 `tuya_iot_wf_soc_dev_init` 函数注册 `pre_gw_ug_cb` 成员的回调函数，并在此回调函数中打印形参参数。在设备进行 `OTA` 更新时，就可以在这个回调函数中获取到这些信息了，可参考 `OTA example`。

密钥的获取，可使用 `http_pskkey_get` 函数获取。

函数参数介绍如下：

![http_gw_download_file参数介绍.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/1657017269d1da020117c.png)

* 文件下载回调函数介绍

在数据处理完成之后要将 `remain_len` 赋0，同时返回0。否则 `SDK` 内部不知道你这数据处理完成了。这个函数跟 `ota` 升级注册的数据下载回调函数的使用方法几乎一样。

![文件下载回调函数介绍.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/1657070246ce9c902b25a.png)

## 流程介绍
相关函数目录：
`software/TuyaOS/include/components/svc_tuya_cloud/include/http/iot_httpc.h`

`software/TuyaOS/include/components/svc_api_iot/include/tuya_iot_internal_api.h`

![文件下载函数流程.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/1657069916f6f3a11d6c8.png)

## 运行结果
首先确保设备要正确连接 `iot` 平台。

调用 `http_gw_download_file` 函数开始下载文件。

```C
[01-01 08:00:15 TUYA D][lr:0x4babb] --------------------http get data callback-----------------
[01-01 08:00:15 TUYA D][lr:0x4bac3] Total len:599408
[01-01 08:00:15 TUYA D][lr:0x4bacf] Offset:590 , len:1024
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[0]:106
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[1]:57
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[2]:238
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[3]:255
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[4]:178
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[5]:59
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[6]:39
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[7]:136
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[8]:116
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[9]:119
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[10]:51
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[11]:173
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[12]:198
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[13]:233
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[14]:170
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[15]:187
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[16]:37
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[17]:115
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[18]:91
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[19]:253
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[20]:69
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[21]:156
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[22]:128
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[23]:37
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[24]:190
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[25]:197
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[26]:87
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[27]:200
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[28]:170
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[29]:70
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[30]:17
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[31]:110
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[32]:66
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[33]:220
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[34]:100
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[35]:10
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[36]:115
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[37]:203
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[38]:237
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[39]:136
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[40]:138
[01-01 08:00:15 TUYA D][lr:0x4bae1] data[41]:56

/*......*/
```


## 技术支持

您可以通过以下方法获得涂鸦的支持:

- TuyaOS 论坛： https://www.tuyaos.com

- 开发者中心： https://developer.tuya.com

- 帮助中心： https://support.tuya.com/help

- 技术支持工单中心： https://service.console.tuya.com
