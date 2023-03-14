

# KV TUYA DATABASE

##  简介

这个项目将会介绍如何使用 `tuyaos 3 database kv` 相关接口，读涂鸦数据库。

涂鸦数据库：

在存储器中有一块特殊内存，对其读写不需要键，因为这块内存中只有1个 `KV` 。这块内存常用于存储固件信息。例如你烧录1个免开发的固件，那么固件的配置表，固件流水表，固件版本号等信息都存在这块内存上，所以开发者对于这块内存不建议进行写操作，因为这是非常危险的，会破坏原有的存储信息。

![kv4.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/1655359556e5f448d405d.png)


## 流程介绍
相关函数目录：
`software/TuyaOS/include/components/base_db/include/tuya_ws_db.h`

![KV TUYA DB PROCESS.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/1655643137b83d94db840.png)

## 运行结果
读取数据并打印。
```c
[13:34:50.421]收←◆[01-01 00:00:05 TUYA D][lr:0x4a767] len = 103
[01-01 00:00:05 TUYA D][lr:0x4a779] out_value[0] = 123
[01-01 00:00:05 TUYA D][lr:0x4a779] out_value[1] = 114
[01-01 00:00:05 TUYA D][lr:0x4a779] out_value[2] = 101
[01-01 00:00:05 TUYA D][lr:0x4a779] out_value[3] = 115
[01-01 00:00:05 TUYA D][lr:0x4a779] out_value[4] = 101
[01-01 00:00:05 TUYA D][lr:0x4a779] out_value[5] = 116
[01-01 00:00:05 TUYA D][lr:0x4a779] out_value[6] = 95
[01-01 00:00:05 TUYA D][lr:0x4a779] out_value[7] = 112
[01-01 00:00:05 TUYA D][lr:0x4a779] out_value[8] = 105
[01-01 00:00:05 TUYA D][lr:0x4a779] out_value[9] = 110
/*......*/
```

## 技术支持

您可以通过以下方法获得涂鸦的支持:

- TuyaOS 论坛： https://www.tuyaos.com

- 开发者中心： https://developer.tuya.com

- 帮助中心： https://support.tuya.com/help

- 技术支持工单中心： https://service.console.tuya.com
