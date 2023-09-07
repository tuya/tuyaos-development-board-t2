# tdd_sensor_temp_humi

## 修改记录

| 版本| 时间| 修改内容 |
| :--- | :--- | :--- |
| v0.1 | 2022/09/08 | 初版 |

## 名词解释

| 名词 | 解释 |
| :--- | :--- |
| 元素 | 传感器采集的某一类数据可以根据其物理含义抽象为一种元素 (element)，如温度。 |

## 基本功能

- 对应用提供统一的传感元素。

| 元素名称 | 元素 ID | 单位 | 范围 |
| :---: | :---: | :---: | :---: |
| 温度 | 0 | ℃ | -40℃ ~ 125℃ |
| 湿度 | 1 | %RH | 0% ~ 100% |

- 对应用提供独立但命名方式统一的注册接口。

| 接口名称 | 说明 |
| :--- | :--- |
| tdd_sensor_xxx_register | 注册 xxx 型号的温湿度传感器。 |

- 对应用提供统一的控制命令。

| 控制命令 | 命令值 | 命令参数 |
| :--- | :--- | ---- |
| 软件复位 | 0 | 无 |
| 读序列号 | 1 | [out] 序列号值 |
| 设置采样精度 | 2 | [in] 采样精度选择 (高 / 中 / 低) |
| 设置采样频率 | 3 | [in] 采样频率选择 (0.5Hz / 1Hz / 2Hz / 4Hz / 10Hz) |
| 设置报警阈值 | 4 | [in] 温度上限报警阈值<br/>[in] 温度上限报警清除阈值<br/>[in] 温度下限报警阈值<br/>[in] 温度下限报警清除阈值<br/>[in] 湿度上限报警阈值<br/>[in] 湿度上限报警清除阈值<br/>[in] 湿度下限报警阈值<br/>[in] 湿度下限报警清除阈值 |
| 获取报警阈值 | 5 | [out] 温度上限报警阈值<br/>[out] 温度上限报警清除阈值<br/>[out] 温度下限报警阈值<br/>[out] 温度下限报警清除阈值<br/>[out] 湿度上限报警阈值<br/>[out] 湿度上限报警清除阈值<br/>[out] 湿度下限报警阈值<br/>[out] 湿度下限报警清除阈值 |
| 启用报警引脚 | 6 | [in] 报警引脚编号<br>[in] 报警引脚中断配置 |
| 禁用报警引脚 | 7 | [in] 报警引脚编号 |
| 读状态寄存器 | 8 | [out] 状态寄存器值 |
| 清状态寄存器 | 9 | 无 |
| 启用加热器 | 10 | 无 |
| 禁用加热器 | 11 | 无 |
| 激活加热器一段时间 | 12 | [in] 工作功耗选择 (高 / 中 / 低)<br/>[in] 激活时间选择 (长 / 短) |

## 组件依赖

| 组件 | 说明 |
| :--- | :--- |
| tdl_sensor_hub | 传感器驱动管理 |
| tkl_gpio | GPIO 驱动 |
| tal_log | 日志管理 |
| tal_system | 系统接口 |

## 目录结构

```xml
├── include                      /* 头文件目录 */
|    └── tdd_sensor_temp_humi.h  /* 温湿度传感通用接口 */
└── src                          /* 源文件目录 */
     ├── tdd_sensor_cht8305.c    /* CHT8305 驱动程序 */
     ├── tdd_sensor_sht3x.c      /* SHT3x 系列驱动程序 */
     └── tdd_sensor_sht4x.c      /* SHT4x 系列驱动程序 */
```

## 适配情况

| 传感器型号 | 供应商 | 数据手册 | 驱动方式 | 无效命令 |
| :---: | :---: | :---: | :---: | :---: |
| *SHT3x* | *Sensirion* | *[Datasheet](https://cdn.sensirion.com.cn/media/documents/213E6A3B/61641DC3/Sensirion_Humidity_Sensors_SHT3x_Datasheet_digital.pdf)* | *I2C* | *12* |
| *SHT4x* | *Sensirion* | *[Datasheet](https://cdn.sensirion.com.cn/media/documents/33FD6951/624C4357/Datasheet_SHT4x.pdf)* | *I2C* | *3~11* |
| *CHT8305* | *Sensylink* | *[Datasheet](http://www.sensylink.com/a/products/lm2/CHT8305.html)* | *I2C* | *3,12* |

## 使用说明

### 基本步骤

1. 调用 `tdd_sensor_xxx_register` 注册 xxx 设备；
2. 调用 `tdl_sensor_hub` 组件提供的接口控制 xxx 设备。

### 应用示例

这里仅介绍如何注册温湿度传感器驱动，其他通用操作请参考 `tdl_sensor_hub` 组件配套的说明文档。

- SHT3x

```c
/**
 * @brief 注册SHT3x设备
 * @param 无
 * @return 操作结果
 */
STATIC OPERATE_RET __sht3x_register(VOID_T)
{
    OPERATE_RET op_ret;

    // 配置注册参数
    SR_I2C_GPIO_T i2c_gpio = {
        .scl = SHT3X_SCL_PIN,
        .sda = SHT3X_SDA_PIN
    };
    SR_TH_I2C_CFG_T sht3x_i2c_cfg = {
        .port = 0,
        .addr = SR_I2C_ADDR_SHT3X_A,
        .gpio = i2c_gpio
    };

    SR_TH_MEAS_CFG_T sht3x_meas_cfg = {
        .prec = SR_TH_PREC_HIGH,
        .freq = SR_TH_FREQ_1HZ
    };

    // 调用注册设备接口：注册SHT3x设备
    op_ret = tdd_sensor_sht3x_register("SHT3x", sht3x_i2c_cfg, sht3x_meas_cfg);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("tdd_sensor_sht3x_register, error code: %d.", op_ret);
        return op_ret;
    }

    return OPRT_OK;
}
```

- SHT4x

```c
/**
 * @brief 注册SHT4x设备
 * @param 无
 * @return 操作结果
 */
STATIC OPERATE_RET __sht4x_register(VOID_T)
{
    OPERATE_RET op_ret;

    // 配置注册参数
    SR_I2C_GPIO_T i2c_gpio = {
        .scl = SHT4X_SCL_PIN,
        .sda = SHT4X_SDA_PIN
    };
    SR_TH_I2C_CFG_T sht4x_i2c_cfg = {
        .port = 0,
        .addr = SR_I2C_ADDR_SHT4X_A,
        .gpio = i2c_gpio
    };

    // 调用注册设备接口：注册SHT4x设备
    op_ret = tdd_sensor_sht4x_register("SHT4x", sht4x_i2c_cfg, SR_TH_PREC_HIGH);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("tdd_sensor_sht4x_register, error code: %d.", op_ret);
        return;
    }

    return OPRT_OK;
}
```

- CHT8305

```c
/**
 * @brief 注册CHT8305设备
 * @param 无
 * @return 操作结果
 */
STATIC OPERATE_RET __cht8305_register(VOID_T)
{
    OPERATE_RET op_ret;

    // 配置注册参数
    SR_I2C_GPIO_T i2c_gpio = {
        .scl = CHT8305_SCL_PIN,
        .sda = CHT8305_SDA_PIN
    };
    SR_TH_I2C_CFG_T cht8305_i2c_cfg = {
        .port = 0,
        .addr = SR_I2C_ADDR_CHT8305_A,
        .gpio = i2c_gpio
    };

    // 调用注册设备接口：注册SHT3x设备
    op_ret = tdd_sensor_cht8305_register("CHT8305", cht8305_i2c_cfg);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("tdd_sensor_cht8305_register, error code: %d.", op_ret);
        return;
    }

    return OPRT_OK;
}
```

## 常见问题

暂无。
