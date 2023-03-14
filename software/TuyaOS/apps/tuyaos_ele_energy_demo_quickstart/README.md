# 硬件信息（CBU模组）

| 脉冲类计量芯片  | I/O引脚       | 有效电平 |
| -------- | ----------- | ---- |
| 计量芯片CF脚  | pwm4        | IO输入 |
| 计量芯片CF1脚 | pwm5        | IO输入 |
| 计量切换引脚   | P14         | 高有效  |
| 硬件定时器    | TUYA_TIMER0 |      |

# 编译说明

1、cd 到包含build_app.sh的目录下

2、使用编译指令：sh build_app.sh apps/tuyaos_ele_energy_demo_quickstart tuyaos_ele_energy_demo_quickstart 1.0.0 例如:

```C
sh build_app.sh apps/tuyaos_ele_energy_demo_quickstart tuyaos_ele_energy_demo_quickstart 1.0.0
```

# 注意点

1、tdl_ele_energy_read_enery_value 接口每调用一次就会清空累计电量,每次调用的间隔不要太短，否则可能会一直是0

2、该 demo 没有进行计量芯片校准的功能，关于计量芯片校准，您可以[点击阅读相关文章](https://developer.tuya.com/cn/docs/iot-device-dev/Electrical-service-Wi-Fi?id=Kb6iu9lqwt0lf)获取更多内容

3、点击查看 [tdl_ele_energy 层 README.md](./src/tdl_ele_energy/README.md)、 [tdd_ele_energy 层 README.md](./src/tdd_ele_energy/README.md)
