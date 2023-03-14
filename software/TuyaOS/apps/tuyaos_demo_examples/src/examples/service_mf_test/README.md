# mf test

该功能会在您将代码中的 UUID 和 AUTHKEY 注释掉后打开，这时你就可以使用产测功能通过上位机进行授权和进行其他测试了。

##  简介

这个项目将会介绍什么是涂鸦烧录授权以及烧录授权的使用方法。

* 涂鸦烧录授权简介
  
涂鸦烧录授其实就是使用涂鸦上位机对涂鸦模组进行烧录授权，它可以对涂鸦模组、`PCBA` 进行测试，固件烧录、授权、国家码设置等基本出厂默认操作，是规模化投产的关键一步。

* 涂鸦烧录授权相关函数介绍

最主要的就是 `mf_test` 函数。使用方法如下:
```c
mf_init(&intf,APP_BIN_NAME,USER_SW_VER,TRUE);
```
![mf_test2.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/16617588599c552164814.png)

请开发者注意这些回调函数，`UART` 相关的回调函数，`uart_init`、`uart_free`、`uart_send`、`uart_reve`，这些回调函数开发者最好不要修改 。除`UART`相关的回调函数外，例如 `user_enter_mf_callback` 是进入产测前回调函数，如果开发者想在产品进入产测前做些东西，那就在此回调函数中实现，如果开发者什么都不想做，那就空函数。所有回调函数如下图所示：

![mf_test6.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/1661761987a0f62c1689a.png)


其他函数介绍如下：

![mf_test3.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/1661759702fea067598ab.png)

* 进入产测失败的建议解决方法
  
在涂鸦云模组烧录授权的过程中，经常遇到无法进入产测的问题。

![973.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/16617598018a09068bc79.png)

1. 15min产测保护

这个是最常见的产测问题。需要注意的是，为了保护这些产测数据，涂鸦 `SDK` 做了一层逻辑，即模组工作15min后，将关闭等待串口接收，不允许继续写入产测数据，不能再次授权。

解决方法：考虑开发者使用不同芯片且不同产品，涂鸦建议开发者直接使用原厂烧录工具擦除数据（别全擦了,就擦除 `tuya data` 这部分就可以了）。在 `tkl_flash.c` 文件中可查看此芯片的 `flash` 分区表。

2. 查看硬件是否连接正确

解决方法：查看串口是否正常，上位机接收不到模组的返回信息，也会提示进入产测失败。

## 流程介绍
相关函数目录：
`software/TuyaOS/include/components/svc_mf_test/include/`

![mf_test1.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/166177338011fbf7913e6.png)

## 运行结果
15分钟产测保护，请开发者查看有没有这个log，如果有那就是15分钟产测保护了。
```c
[01-01 00:00:00 TUYA N][lr:0x785df] have actived over 15 min, not enter mf_init
```

## 技术支持

您可以通过以下方法获得涂鸦的支持:

- TuyaOS 论坛： https://www.tuyaos.com

- 开发者中心： https://developer.tuya.com

- 帮助中心： https://support.tuya.com/help

- 技术支持工单中心： https://service.console.tuya.com
