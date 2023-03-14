

# UF DATABASE

##  简介

这个项目将会介绍如何使用 `tuyaos 3 database uf` 相关接口，从文件数据库中读写取数据。

* 涂鸦文件数据库

在存储器中有一块文件数据库的内存。文件数据库的读写操作和键值数据库的读写操作类似，只不过文件数据库是以 `block` 为单位保存的，所以文件数据库什么数据都可以写，写的数据也可以更大。

![uf1.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/165536134868ec3360117.png)

## 流程介绍
相关函数目录：
`software/TuyaOS/include/components/base_uf/include/tuya_uf_db.h`

![kV_UF.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/1657781430549bb0233f3.png)

## 运行结果
打印存储相关信息。
```c
[01-01 00:00:02 TUYA D][lr:0x4a76b] sector size : 4096
[01-01 00:00:02 TUYA D][lr:0x4a773] uf partition number : 1
[01-01 00:00:02 TUYA D][lr:0x4a77b] uf_partition_start_addr :0x1d2000
[01-01 00:00:02 TUYA D][lr:0x4a783] uf_partiton_flash_sz :98304
```

读取数据并打印。数据的最后会默认增加一位 `\0`，为了保证字符串尾部都有 `\0`。
```c
[01-01 00:00:02 TUYA D][lr:0x4a7a1] read len 6
[01-01 00:00:02 TUYA D][lr:0x4a7b3] out_value[0] = 1
[01-01 00:00:02 TUYA D][lr:0x4a7b3] out_value[1] = 2
[01-01 00:00:02 TUYA D][lr:0x4a7b3] out_value[2] = 3
[01-01 00:00:02 TUYA D][lr:0x4a7b3] out_value[3] = 4
[01-01 00:00:02 TUYA D][lr:0x4a7b3] out_value[4] = 5
[01-01 00:00:02 TUYA D][lr:0x4a7b3] out_value[5] = 0
```

## 技术支持

您可以通过以下方法获得涂鸦的支持:

- TuyaOS 论坛： https://www.tuyaos.com

- 开发者中心： https://developer.tuya.com

- 帮助中心： https://support.tuya.com/help

- 技术支持工单中心： https://service.console.tuya.com
