

# KV FUZZY

##  简介

这个项目将会介绍如何使用 `tuyaos 3 database kv` 相关接口，如何模糊读取。

* 数据库模糊读取介绍

假设在存储器中有1块这样的内存，有1个名为 `Key1` 的键，我们想要获取这个键对应的值，只需要使用模糊读取接口，不需要输入完整名称。例如搜索 `Key` ，即可获取 `Key1` 对应的值。

![KV2.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/1655358340dadd1393468.png)


如果存储器中有多个名称相似的键，Key1和 Key2。此时在使用模糊读取函数，搜索 `Key`，那么就是先找到那个键就返回那个键的值。

![kv1.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/1655358271a430529ab78.png)
## 流程介绍
相关函数目录：
`software/TuyaOS/include/components/base_db/include/tuya_ws_db.h`

![KV fuzzy123488.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/1655642941d4c5e433e28.png)

## 运行结果
读取数据并打印。
```c
[01-01 00:00:05 TUYA D][lr:0x4a77d] index = 36
[01-01 00:00:05 TUYA D][lr:0x4a78f] out_value[0] = 1
[01-01 00:00:05 TUYA D][lr:0x4a78f] out_value[1] = 2
[01-01 00:00:05 TUYA D][lr:0x4a78f] out_value[2] = 3
```

## 技术支持

您可以通过以下方法获得涂鸦的支持:

- TuyaOS 论坛： https://www.tuyaos.com

- 开发者中心： https://developer.tuya.com

- 帮助中心： https://support.tuya.com/help

- 技术支持工单中心： https://service.console.tuya.com
