# 涂鸦 T2-U 开发板
[English](https://github.com/tuya/tuyaos-development-board-t2/blob/main/README.md) | 中文

**涂鸦 T2-U 开发板** 是一款专为开发者打造的智能硬件产品原型开发板。它可与其他功能电路模组或电路板配合使用，实现各种智能硬件产品的开发。涂鸦 T2-U 开发板非常易于使用，您可以利用此开发板快速实现各种智能硬件 Demo。

<img alt="image.png" src="https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/16781047011ffea0d5520.png" width="400">

## T2-U模组概述
涂鸦 T2-U 开发板主控采用涂鸦智能开发的一款嵌入式 Wi-Fi+蓝牙模组 [T2-U](https://developer.tuya.com/en/docs/iot/T2-U-module-datasheet?id=Kce1tncb80ldq)。它由一个高集成度的无线射频模组 T2-U 和外围的按键、LED 指示灯、I/O 接口、电源和 USB 转串口芯片构成。

T2-U模组 内置运行速度最高可到 120 MHz 的 32-bit MCU，内置 2Mbyte 闪存和 256 KB RAM。

### 特性

- 内置低功耗 32 位 CPU，可以兼作应用处理器
- 主频支持 120MHz
- 工作电压：3.0V-3.6V
- 外设：
  - 6×PWM
  - 4xTimer
  - 2×UART
  - 1×SPI
  - 2xI2C
  - 1xADC
  - 19xGPIO
- Wi-Fi 连通性
  - 802.11 b/g/n
  - 通道1-14@2.4GHz
  - 支持 WEP、WPA/WPA2、WPA/WPA2 PSK (AES) 和 WPA3 安全模式
  - 802.11b 模式下最大 +16dBm 的输出功率
  - 支持 STA/AP/STA+AP 工作模式
  - 支持 SmartConfig 和 AP 两种配网方式（包括 Android 和 iOS 设备）
  - 板载 PCB 天线，天线峰值增益 2.2dBi
  - 工作温度：-40℃ 到 105℃
- 蓝牙连通性
  - 低功耗蓝牙 V5.2 完整标准
  - 蓝牙模式支持 6 dBm 发射功率
  - 完整的蓝牙共存接口
  - 板载 PCB 天线，天线峰值增益 2.2dBi

[更多T2-U模组资料](https://developer.tuya.com/cn/docs/iot/T2-U-module-datasheet?id=Kce1tncb80ldq)


## 应用场景
涂鸦 T2-U 开发板不仅适用于涂鸦 IoT 免开发方案中，也可用于以下场景。
- 利用此开发板，您可以快速实现各种智能硬件 Demo。
- 针对不同类型开发者，涂鸦 T2-U 开发板的常见场景如下：
    - 嵌入式工程师可以使用该开发板进行嵌入式程序前期开发和调试。
    - App 开发者可以在硬件设备开发前期，使用该开发板进行 App 的开发和调试。
    - 创客可以借助该开发板快速实现硬件产品 Demo，并通过手机实现设备控制。
    - IoT 技术爱好者可以了解 Wi-Fi 控制原理，学习智能硬件产品开发。

[更多T2-U开发板硬件资料](https://developer.tuya.com/cn/docs/iot/t2-u-board?id=Kce6cq9e9vlmv) | [下载T2开发板原理图](https://github.com/tuya/tuyaos-development-board-t2/blob/main/hardware/Tuya%20T2%20Development%20Board_V1.0.0.pdf)



## TuyaOS软件开发
TuyaOS 是一种基于 RTOS、Linux、Non-OS 等内核设计的、应用于 IoT 领域、面向全连接、全场景的分布式跨平台操作系统。

基于 TuyaOS Kernel 及 TuyaOS 丰富的开发组件，使开发人员能够从不同的芯片平台、系统、连接协议及应用软件等碎片化开发体验中得到解放，只需专注于自身业务的创新及开发，并且一次开发、多端部署、全球可用、安全合规。整体架构采用分层、可插拔组件的方式设计，可根据芯片资源及应用场景自由裁剪，生成符合要求的开发框架。通过 Tuya 特有的物模型及自研软总线，使得所有采用 TuyaOS 开发的不同通信协议的产品之间能够互联互通。

[更多TuyaOS介绍](https://developer.tuya.com/cn/docs/iot-device-dev) | [TuyaOS产品开发介绍](https://developer.tuya.com/cn/docs/iot/development-overview?id=Ka3redtxl6g4k) | [Tuya Wind IDE介绍](https://developer.tuya.com/cn/docs/iot-device-dev/tuyaos-wind-ide?id=Kbfy6kfuuqqu3)

### 1. 快速上手

#### 依赖
在Debian/Ubuntu等Linux系统上，编译前需要安装部分软件包依赖:
``` bash
$ sudo apt-get install build-essential wget git python python3 libc6-i386 
```

未安装`libc6-i386 `会提示`arm-none-eabi-gcc not found`，安装`libc6-i386`后并先调用`编译清除`命令清除已生成的中间文件，再编译。



#### 编译

示例工程位于[apps](software/TuyaOS/apps)目录下，你可以使用`tuyaos_demo_examples`工程快速完成编译、烧录、运行流程，在software/TuyaOS目录下使用以下命令编译：
``` bash
$ sh build_app.sh apps/tuyaos_demo_examples tuyaos_demo_examples 1.0.0
```
apps/tuyaos_demo_examples：编译工程路径

tuyaos_demo_examples：编译工程名称

1.0.0：当前编译固件版本，可根据实际情况自行修改



#### 编译清除

如需清除编译中间文件，可使用以下命令：

``` bash
$ sh build_app.sh apps/tuyaos_demo_examples tuyaos_demo_examples 1.0.0 clean
```



#### 烧录

编译后的bin文件位于apps/tuyaos_demo_examples/output/1.0.0目录下，对应文件名及作用：

| 固件名称                           | 使用场景                                               |
| ---------------------------------- | ------------------------------------------------------ |
| tuyaos_demo_examples_QIO_1.0.0.bin | 生产固件，包括bootLoader及应用区固件，可用于新模组生产 |
| tuyaos_demo_examples_UA_1.0.0.bin  | 应用区固件，不包含bootload                             |
| tuyaos_demo_examples_UG_1.0.0.bin  | OTA固件，可上传涂鸦云平台完成设备OTA功能               |


使用`bk_writer_gui.exe`进行串口烧录，烧录说明参考[T2烧录使用说明](tools/README_zh.md)



### 2. TuyaOS软件开发包

当前应用提供了丰富的example和行业应用产品开发包，可帮助开发者快速熟悉基于T2芯片的TuyaOS软件开发。

| 软件开发包                                                   | 说明                                                         |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| [tuyaos_demo_quickstart](software/TuyaOS/apps/tuyaos_demo_quickstart) | 快速使用TuyaOS连接tuya云                                     |
| [tuyaos_demo_examples](software/TuyaOS/apps/tuyaos_demo_examples) | T2芯片包括外设/WiFi/BLE等多种能力使用示例                    |
| [tuyaos_ele_energy_demo_quickstart](software/TuyaOS/apps/tuyaos_ele_energy_demo_quickstart) | 电量统计系列芯片快速使用示例，包括bl0942、hlw8032、bl0937、hlw8012 |
| [tuyaos_pixel_driver_demo_quickstart](software/TuyaOS/apps/tuyaos_pixel_driver_demo_quickstart) | 幻彩灯带系列芯片快速使用示例，包括ws2812、ws2814、sk6812、sm16703p、sm16704pk、yx1903b |

[更多行业应用产品开发包介绍](https://developer.tuya.com/cn/docs/iot-device-dev/tuyaos-package-networking-wifible?id=Kbfxri1h48k7f)



## 技术支持

您可以通过以下方法获得涂鸦的支持:

- 开发者中心：[https://developer.tuya.com/cn](https://developer.tuya.com/cn)
- 帮助中心: [https://support.tuya.com/cn/help](https://support.tuya.com/cn/help)
- 涂鸦开发者论坛: [https://www.tuyaos.com](https://www.tuyaos.com)