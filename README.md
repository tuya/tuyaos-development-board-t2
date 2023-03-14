# Tuya T2-U development board 
English | [中文](README_zh.md)

**T2-U development board** is designed for prototyping smart devices easily and quickly. You can use it with other purpose-built modules or development boards to evaluate your project.

<img alt="image.png" src="https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/content-platform/hestia/16781047011ffea0d5520.png" width="400">

## T2-U module Overview
T2-U development board consists of Tuya’s highly-integrated Wi-Fi and Bluetooth module [T2-U](https://developer.tuya.com/en/docs/iot/T2-U-module-datasheet?id=Kce1tncb80ldq) and peripherals including buttons, LED indicator, GPIOs, power, and USB-to-serial chip.

T2-U is built-in with a 32-bit MCU with a running speed of up to 120 MHz, 2 Mbyte flash, and 256-KB RAM。

### Features

- Embedded low-power 32-bit CPU, which can also function as an application processor
- The clock rate: 120 MHz
- Operating voltage: 3.0V to 3.6V
- Peripherals：
  - 6×PWM
  - 4xTimer
  - 2×UART
  - 1×SPI
  - 2xI2C
  - 1xADC
  - 19xGPIO
- Wi-Fi connectivity
  - 802.11 b/g/n
  - Channels 1 to 14@2.4 GHz
  - Support WEP, WPA/WPA2, WPA/WPA2 PSK (AES), WPA3 security modes
  - Up to +16 dBm output power in 802.11b mode
  - Support STA/AP/STA+AP working mode
  - Support SmartConfig and AP pairing manners for Android and iOS devices
  - Onboard PCB antenna with a gain of 2.2 dBi
  - Operating temperature: -40°C to 105°C
- Bluetooth LE connectivity
  - Support the Bluetooth LE V5.2
  - 6 dBm transmit power in Bluetooth mode
  - Complete Bluetooth coexistence interface
  - Onboard PCB antenna with a gain of 2.2 dBi

[ More T2-U module information](https://developer.tuya.com/en/docs/iot/T2-U-module-datasheet?id=Kce1tncb80ldq)

## Scenarios

T2-U development board applies to a wide range of scenarios.

- Develop with the board to build your IoT projects easily and fast.
- You can use this board for different development purposes.
  - Embedded program development and debugging
  - App development and debugging
  - Creating connected devices that can be controlled with a mobile phone
  - Getting started with IoT development and learning how the Wi-Fi-enabled control system works

[More T2-U development board hardware information](https://developer.tuya.com/en/docs/iot/t2-u-board?id=Kce6cq9e9vlmv) | [Download T2-U development board  schematic](https://github.com/tuya/tuyaos-development-board-t2/blob/main/hardware/Tuya%20T2%20Development%20Board_V1.0.0.pdf)



## TuyaOS software designing
Built on top of the RTOS, Linux, and Non-OS, TuyaOS is a distributed and platform-agnostic IoT operating system. 

Backed by the TuyaOS kernel and various development components, TuyaOS is designed to tackle the heterogeneity of platforms, systems, protocols, and applications in order to enable quick and reliable integration, interconnection, interoperability, and compliance. The tiered and plug-and-play architecture design allows you to quickly tailor a development framework based on your hardware resources and use cases hence reducing the cost of development with high cost performance. Tuya’s proprietary things data model and software bus allow all TuyaOS-based products to be interconnected.

[More TuyaOS information](https://developer.tuya.com/en/docs/iot-device-dev) | [More Tuyaos TuyaOS product developerment information](https://developer.tuya.com/en/docs/iot/development-overview?id=Ka3redtxl6g4k) | [Tuya Wind IDE information](https://developer.tuya.com/en/docs/iot-device-dev/tuyaos-wind-ide?id=Kbfy6kfuuqqu3)



### 1.Quick start

#### Dependency
Some package dependencies need to be installed before compiling.
``` bash
$ sudo apt-get install build-essential wget git python python3 libc6-i386 
```

Do not install `libc6-i386` will prompt `arm-none-eabi-gcc not found`, install `libc6-i386` and first call `compile clearup` command to clearup the generated intermediate files, then compile.



#### Compile

The compile project is in the [apps](software/TuyaOS/apps) directory, you can quickly compile, burn, and run the process through `tuyaos_demo_examples`. Use the following command in the root directory to compile: 

``` bash
$ sh build_app.sh apps/tuyaos_demo_examples tuyaos_demo_examples 1.0.0
```
apps/tuyaos_demo_examples：compile project path

tuyaos_demo_examples：compile project name

1.0.0：The version of the current compilation firmware



#### Compilation cleanup

You can clear the compiled intermediate files with the following commands:

``` bash
$ sh build_app.sh apps/tuyaos_demo_examples tuyaos_demo_examples 1.0.0 clean
```



#### Burning 

The compiled bin file is in apps/tuyaos_demo_examples/output/1.0.0. The file name and its functions are as follows:

| Firmware name                      | Usage scenario                                               |
| ---------------------------------- | ------------------------------------------------------------ |
| tuyaos_demo_examples_QIO_1.0.0.bin | Production firmware，includes bootLoader and application firmware, can be used for new module production |
| tuyaos_demo_examples_UA_1.0.0.bin  | Application firmware，not include bootload                   |
| tuyaos_demo_examples_UG_1.0.0.bin  | OTA firmware，can upload Doodle cloud platform to complete OTA function of the device |

Use `bk_writer_gui.exe` for serial port burning, burning instructions refer to [T2 burning Instructions](tools/README.md)



### 2. TuyaOS software development kit

The current application provides rich examples and industry application product development kits to help developers quickly familiarize themselves with TuyaOS software development based on T2 chips.

| Software development kit                                     | Descriptions                                                 |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| [tuyaos_demo_quickstart](software/TuyaOS/apps/tuyaos_demo_quickstart) | Quickly connect to the tuya cloud using TuyaOS               |
| [tuyaos_demo_examples](software/TuyaOS/apps/tuyaos_demo_examples) | The T2 chip includes examples of various capabilities such as peripherals, WiFi, and BLE |
| [tuyaos_ele_energy_demo_quickstart](software/TuyaOS/apps/tuyaos_ele_energy_demo_quickstart) | Battery statistics series chip quick use examples, including: bl0942、hlw8032、bl0937、hlw8012 |
| [tuyaos_pixel_driver_demo_quickstart](software/TuyaOS/apps/tuyaos_pixel_driver_demo_quickstart) | Magic Strip series chip quick use examples including: ws2812、ws2814、sk6812、sm16703p、sm16704pk、yx1903b |

[More applications in the industry product development kit introduction](https://developer.tuya.com/en/docs/iot-device-dev/tuyaos-package-networking-wifible?id=Kbfxri1h48k7f)



## Support and help

You can get support from Tuya through the following accesses: 

- [https://developer.tuya.com/en](https://developer.tuya.com/en)
- [https://support.tuya.com/en/help](https://support.tuya.com/en/help)
- [https://www.tuyaos.com](https://www.tuyaos.com)