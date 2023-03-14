# tuyaos_dimmer_energy_demo_quickstart

## 修改记录

| 版本 | 时间 | 修改内容 |
| :--- | :--- | :--- |
| v0.1 | 2022/07/11 | 初版 |

## 名词解释

| 名词 | 解释 |
| :--- | :--- |
| CW | 涂鸦定义的一种白光 PWM 驱动方案。一路 CW 接冷白光控制电路，一路 WW 接暖白光控制电路；无论调亮度还是调色温两路都变化，调色温时两路信号互补，总功率不变。 |
| CCT | 涂鸦定义的一种白光 PWM 驱动方案。一路 Bright 控制总电流大小 (亮度)，一路 CCT 分配冷暖电流比 (色温)。 |

## 基本功能

`tdl_dimmer_manage` 组件主要用于调光器驱动管理，其基本功能如下：

- 对应用提供统一的数据类型 (通道数、输出数据)；
- 对应用提供统一的操作接口 (查找、启动、停止、输出)；
- 对驱动提供统一的注册接口和驱动接口抽象 (启动、停止、输出)；
- 对驱动提供统一的数据预处理和其他通用功能处理方案；

## 组件依赖

| 组件 | 说明 |
| :--- | :--- |
| tal_mutex | 锁管理 |
| tal_semaphore | 信号管理 |
| tal_memory | 内存管理 |
| tkl_gpio | GPIO 驱动 |
| tal_log | 日志管理 |

## 资源依赖

| 资源 | 依赖情况 |
| :--- | :--- |
| 外设 | 一个调光器最多占用 1 个 GPIO 口；(控制引脚) |
| 内存 | 完成一个带控制引脚的 5 通道调光器注册并打开内存消耗约 12.5k。 |

## 目录结构

```xml
├── include
|    ├── tdl_dimmer_driver.h         /* 面向驱动接口 */
|    ├── tdl_dimmer_manage.h         /* 面向应用接口 */
|    └── tdu_light_types.h           /* 通用数据类型 */
└── src
     └── tdl_dimmer_manage.c         /* 调光器驱动管理程序 */
```

## 接口列表

| 接口名称 | 接口功能 | 接口类型 |
| :--- | :--- | :--- |
| tdl_dimmer_register | 注册调光设备 | 面向驱动 |
| tdl_dimmer_find     | 查找调光设备 | 面向应用 |
| tdl_dimmer_open     | 启动调光设备 | 面向应用 |
| tdl_dimmer_close    | 停止调光设备 | 面向应用 |
| tdl_dimmmer_output  | 调光设备输出 | 面向应用 |

## 使用说明

需配合 `tdd_dimmer_driver` 组件使用。

**基本步骤**

1. 调用 `tdd_dimmer_xxxx_register` 注册 xxxx 设备；
2. 调用 `tdl_dimmer_find` 查找 xxxx 设备（确认 xxxx 设备注册成功）；
3. 调用 `tdl_dimmer_open` 启动 xxxx 设备并注册开关状态获取回调函数（准备资源）；
4. 调用 `tdl_dimmmer_output` 将设定的调光数据下发到设备驱动层输出；
5. 调用 `tdl_dimmer_close` 停止 xxxx 设备（释放资源）。

## 常见问题

暂无。

## 相关链接

- [品类方案 - 照明](https://developer.tuya.com/cn/docs/iot/lighting?id=Kaiuzc60687wl)
- [照明业务 - 照明驱动方案](https://developer.tuya.com/cn/docs/iot-device-dev/Lighting-drive-scheme?id=Kb71gj7i2nwvk)
- [TuyaOS](https://developer.tuya.com/cn/docs/iot-device-dev/TuyaOS-Overview?id=Kbfjtwjcpn1gc)
- [TuyaOS - Wi-Fi设备接入](https://developer.tuya.com/cn/docs/iot-device-dev/wifi-sdk-overview?id=Kaiuyfbaezlzt)
- [TuyaOS 论坛](https://www.tuyaos.com/viewforum.php?f=11)
- [Wi-Fi 模组 SDK 二次开发教程](https://developer.tuya.com/cn/docs/iot/Module-SDK-development_tutorial?id=Kauqptzv5yo8a)
