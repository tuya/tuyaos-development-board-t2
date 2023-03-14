

# UF FILE

##  简介

这个项目将会介绍如何使用 `tuyaos 3 database uf` 相关接口，进行文件编程。

* 文件编程介绍

1. `Uf` 的文件编程接口函数和 `Linux` 的文件编程接口函数类似，但并不完全相同。例如在 `Linux` 下，关闭文件在打开文件，文件光标会回到最前面，但是 `Uf` 的文件编程就不会。

2. 虚拟的文件编程。在 `Linux` 等操作系统下的文件编程，我们会根据文件路径创建一个个文件，这些文件存储在磁盘中。但在 `rtos` 下，所谓的文件路径和文件名，其实就是一个字符串。你可以理解为这个文件名就是一个键或者一个索引，你可以通过文件名获取到这个文件的数据。文件名和数据存储在 `Flash` 中。

3. 文件光标。当你在一个文本框或某种程序中输入文字时,总有一个光标来显示你将要输入文字的位置。大部分的光标都为一个小竖线,规则的闪动。在 `Uf` 文件编程中的光标接口 `ufseek` ，你可以通过这个函数改变光标的位置。文件的读取的是光标后面的数据，所以当光标在文件的尾，是读取不到数据的。

## 流程介绍
相关函数目录：
`software/TuyaOS/include/components/base_uf/include/tuya_uf_db.h`

![uf_file_process.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/16556445693b05ea1fd9e.png)

## 运行结果
打印文件编程相关信息。
```c
[01-01 00:00:00 TUYA D][lr:0x4a76d] fd : 1
[01-01 00:00:00 TUYA D][lr:0x4a781] write number : 3
[01-01 00:00:00 TUYA D][lr:0x4a795] offset is at the end
[01-01 00:00:00 TUYA D][lr:0x4a7ad] current offset : 0
[01-01 00:00:00 TUYA D][lr:0x4a7bb] file size: 3
[01-01 00:00:00 TUYA D][lr:0x4a7df] reads num: 3, read buff: 123
```

## 技术支持

您可以通过以下方法获得涂鸦的支持:

- TuyaOS 论坛： https://www.tuyaos.com

- 开发者中心： https://developer.tuya.com

- 帮助中心： https://support.tuya.com/help

- 技术支持工单中心： https://service.console.tuya.com
