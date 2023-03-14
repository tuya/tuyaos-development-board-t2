

# KV PROTECT

##  简介

这个项目将会介绍如何使用 `tuyaos 3 database kv` 相关接口，如何受保护的读写。

* 保护读写和普通读写的差别
1. 普通读写是把数据写入到通用的 `Flash` 区。
2. 保护读写是把数据写入到特定的 `Flash` 区，这部分的区域不会被破坏，从而数据不会因此而丢失。

非常重要的数据建议使用保护读写。 


## 流程介绍
相关函数目录：
`software/TuyaOS/include/components/base_db/include/tuya_ws_db.h`

![KV_PROTECT.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/1655641107c2ee3848af6.png)

## 运行结果
读取数据并打印。
```c
[01-01 00:00:05 TUYA D][lr:0x96447] protected read [key1]
[01-01 00:00:05 TUYA D][lr:0x964cd] protected read ret:0 length:3
[01-01 00:00:05 TUYA D][lr:0x4a77b] len = 3
[01-01 00:00:05 TUYA D][lr:0x4a78d] out_value[0] = 1
[01-01 00:00:05 TUYA D][lr:0x4a78d] out_value[1] = 2
[01-01 00:00:05 TUYA D][lr:0x4a78d] out_value[2] = 3
[01-01 00:00:05 TUYA D][lr:0x965e3] protected delete [key1]
```

## 技术支持

您可以通过以下方法获得涂鸦的支持:

- TuyaOS 论坛： https://www.tuyaos.com

- 开发者中心： https://developer.tuya.com

- 帮助中心： https://support.tuya.com/help

- 技术支持工单中心： https://service.console.tuya.com
