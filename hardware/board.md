

# T2开发板硬件介绍

## I/O 口及接口功能定义

<img alt="image.png" src="https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/16781044523402e0804f7.png" width="650">

- 1：MICROUSB（CN1）：即是 5V DC 输入口，也扩展了 2 个串口功能。

- 2：拨码开关（S1）：拨码切到 ON 方向导通，通断 USB 转串口芯片的双串口和芯片串口之间的链路。

- 3：指示灯（D3）：3.3V 电源指示灯。

- 4：按键（S2）：通过 P7 检测，初始化高电平，按下为低电平。

- 5：指示灯（D2）：通过 P26 控制，低电平点亮。

- 6：按键（RST）：复位按键，按下后模组复位。

- 开发板引脚说明：

    | 序号 | 符号 | 说明 |
    | ---- | ---- | ---- |
    | 1 | NC | 引脚悬空。 |
    | 2 | 3.3V | 电源 3.3V 电源引脚。 |
    | 3 | NC | 引脚悬空。 |
    | 4 | NC | 引脚悬空。 |
    | 5 | NC | 引脚悬空。 |
    | 6 | P26 | 支持硬件 PWM5（对应 IC 的 Pin 15）。 |
    | 7 | P24 | 支持硬件 PWM4（对应 IC 的 Pin 16）。 |
    | 8 | P28 | T2-U 的 普通 GPIO（对应 IC 的 Pin 10）。 |
    | 9 | P9 | T2-U 的普通 GPIO（对应 IC 的 Pin 25）。 |
    | 10 | P17 | T2-U 的普通 GPIO，可以复用为 SPI_MISO（对应 IC 的 Pin 14）。 |
    | 11 | P15 | T2-U 的普通 GPIO，可以复用为 SPI_CS，非 SPI 应用不建议使用该引脚（对应 IC 的 Pin 13）。 |
    | 12 | 5V | 电源 5V 电源引脚。 |
    | 13 | RST | T2-U 的 CEN 引脚。芯片使能，高电平使能。 |
    | 14 | GND | 电源参考地引脚。 |
    | 15 | NC | 引脚悬空。 |
    | 16 | TX1 | T2-U 的 UART_TX1，用户数据发送口，对应 IC 的 Pin 27。 |
    | 17 | RX1 | T2-U 的 UART_RX1，用户数据接收口，对应 IC 的 Pin 26。 |
    | 18 | RST | T2-U 的 CEN 引脚，高电平使能。 |
    | 19 | GND | 电源参考地引脚。 |
    | 20 | P14 | T2-U 的 普通 GPIO，可以复用为 SPI_SCK（对应 IC 的 Pin 11）。 |
    | 21 | P16 | T2-U 的 普通 GPIO，可以复用为 SPI_MOSI（对应 IC 的 Pin 12）。 |
    | 22 | RX2 | T2-U 的 UART_RX2，对应 IC 的 P1（对应 IC 的 Pin 28）。 |
    | 23 | TX2 | T2-U 的 UART_TX2，打印日志口，对应 IC 的 P0（对应 IC 的 Pin 29）。 |
    | 24 | P20 | T2-U 的 普通 GPIO（对应 IC 的 Pin 20）。 |
    | 25 | P22 | T2-U 的 普通 GPIO（对应 IC 的 Pin 18）。 |
    | 26 | ADC | T2-U 的 ADC 口，对应 IC 的 P23（对应 IC 的 Pin 17）。 |
    | 27 | P8 | 支持硬件 PWM2（对应 IC 的 Pin 24）。 |
    | 28 | P7 | 支持硬件 PWM1（对应 IC 的 Pin 23）。 |
    | 29 | P6 | 支持硬件 PWM0（对应 IC 的 Pin 22）。 |
    | 30 | NC | 引脚悬空。 |

## 电源带载能力

- 在 MICROUSB（CN1）输入端口输入 5V DC 条件下，开发板可对外输出电源的能力

    | 电源引脚 | 额定电压/额定电流 |
    | ---- | ---- |
    | 5V | 参考 DC-005 输入端适配器输入电流 |
    | 3.3V | 3.3V/0.6A |

- 输出电压特性

    | 输出电流 | 0A | 0.15A | 0.3A | 0.45A | 0.6A | 0.75A |
    | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
    | 输出电压 | 3.34V | 3.36V | 3.37V | 3.37V | 3.38V | 3.38V |

    :::info
    该数据是在 5V 无输出的情况下测试所得。
    :::

## 原理图及 PCB

- 涂鸦 T2-U 开发板的原理图如下所示：

    <img alt="image.png" src="https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/167802965710c1335272e.png" width="450">

- 涂鸦 T2-U 开发板的 PCB 如下图所示：

    <img alt="image.png" src="https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/16780298140ad461dbced.png" width="450">

## USB 转串口

涂鸦 T2-U 开发板内置 USB 转串口芯片，单路 USB 口可扩展出 2 个串口。可通过拨码开关（SW1）切换模组的两路串口是否连接到 USB 转串口芯片上。拨码开关引脚说明如下表所示。

| 拨码开关序号 | 1 | 2 | 3 | 4 |
| ---- | ---- | ---- | ---- | ---- |
| 芯片引脚 | UART1_RX | UART1_TX | UART2_RX | UART2_TX |
| USB 芯片 | USB-TX0 | USB-RX0 | USB-TX1 | USB-RX1 |

:::info
- 拨码切到 ON 方向导通，通断 USB 转串口芯片的双串口和芯片串口之间的链路。

- 芯片 UART 引脚用于与 MCU 通信或普通 I/O 口使用时，需将相应的拨码位置拨到断开的位置，即数字字母方向。

- 不同电脑对应的 COM 口号不一定相同。
:::

## 烧录授权接线方式

使用涂鸦烧录授权软件进行烧录授权，可参考下面的操作说明。将拨码开关（SW1）的 1、2 路都拨至 ON 方向，T2-U 的串口 UART1 与上位机链路导通。

<img alt="image.png" src="https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/167810526177059320bbf.png" width="450">

:::info
该方式下的模组是已预烧录固件，开发板出厂是默认带预烧录固件的。
:::

## 上位机查看 Wi-Fi 工作日志的接线方式

将拨码开关（SW1）的 3、4 路都拨至 ON 方向，T2-U 的串口 UART2 与上位机链路导通。

<img alt="image.png" src="https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/16781055094aced6f852d.png" width="450">

## USB 转串口芯片驱动程序

USB 转串口芯片驱动程序如下所示：

- [Windows 版本](http://www.wch.cn/downloads/CH343SER_ZIP.html)
- [Linux 版本](https://github.com/WCHSoftGroup/ch343ser_linux)

## 注意事项

- 本方案开发板内置电源接口和电路，无需搭配电源板使用。
- 本方案开发板只支持 USB 端口（5V）。