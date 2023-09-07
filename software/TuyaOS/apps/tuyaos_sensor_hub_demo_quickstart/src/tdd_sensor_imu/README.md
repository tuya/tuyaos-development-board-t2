# tdd_sensor_imu

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
| 角速度 X 轴| 0 | dps | 取决于设置的陀螺仪满量程范围 |
| 角速度 Y 轴| 1 | dps | 取决于设置的陀螺仪满量程范围 |
| 角速度 Z 轴| 2 | dps | 取决于设置的陀螺仪满量程范围 |
| 加速度 X 轴| 3 | g | 取决于设置的加速度计满量程范围 |
| 加速度 Y 轴| 4 | g | 取决于设置的加速度计满量程范围 |
| 加速度 Z 轴| 5 | g | 取决于设置的加速度计满量程范围 |
| 温度 | 6 | ℃ | -40℃ ~ 125℃ |

- 对应用提供独立但命名方式统一的注册接口。

| 接口名称 | 说明 |
| :--- | :--- |
| tdd_sensor_xxx_register | 注册 xxx 型号的惯性传感器。 |

- 对应用提供统一的控制命令。

| 控制命令 | 命令值 | 命令参数 |
| :--- | :--- | ---- |
| 软件复位 | 0 | 无 |
| 读设备 ID | 1 | [out] 设备 ID 值 |
| 设备初始化 | 2 | [in] 时钟源 (内部 / X 轴 / Y 轴 / Z 轴 / 外部 32K / 外部 19M)<br/>[in] 陀螺仪满量程范围 (250 / 500 / 1000 / 2000 dps)<br/>[in] 加速度计满量程范围 (2 / 4 / 8 / 16 g)<br/>[in] 采样频率 |
| 设置中断引脚 | 3 | [in] 有效电平 (1 - 低有效 / 0 - 高有效) |

## 组件依赖

| 组件 | 说明 |
| :--- | :--- |
| tdl_sensor_hub | 传感器驱动管理 |
| tal_log | 日志管理 |
| tal_system | 系统接口 |

## 目录结构

```xml
├── include                      /* 头文件目录 */
|    └── tdd_sensor_imu.h        /* 惯性传感通用接口 */
└── src                          /* 源文件目录 */
     └── tdd_sensor_mpu60x0.c    /* MPU60x0 驱动程序 */
```

## 适配情况

| 传感器型号 | 供应商 | 数据手册 | 驱动方式 | 无效命令 |
| :---: | :---: | :---: | :---: | :---: |
| *MPU60x0* | *InvenSense* | *[Datasheet](https://3cfeqx1hf82y3xcoull08ihx-wpengine.netdna-ssl.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf)* | *I2C* | - |

## 使用说明

### 基本步骤

1. 调用 `tdd_sensor_xxx_register` 注册 xxx 设备；
2. 调用 `tdl_sensor_hub` 组件提供的接口控制 xxx 设备。

### 应用示例

这里仅介绍如何注册惯性传感器驱动，其他通用操作请参考 `tdl_sensor_hub` 组件配套的说明文档。

- MPU60x0

```c
/**
 * @brief 注册MPU60x0设备
 * @param 无
 * @return 操作结果
 */
STATIC OPERATE_RET __mpu60x0_register(VOID_T)
{
    OPERATE_RET op_ret;

    // 配置注册参数
    SR_I2C_GPIO_T i2c_gpio = {
        .scl = MPU60X0_SCL_PIN,
        .sda = MPU60X0_SDA_PIN
    };
    SR_IMU_I2C_CFG_T mpu60x0_i2c_cfg = {
        .port = 0,
        .addr = SR_I2C_ADDR_MPU60X0_A,
        .gpio = i2c_gpio
    };

    // 调用注册设备接口：注册MPU60x0设备
    op_ret = tdd_sensor_mpu60x0_register("MPU60x0", mpu60x0_i2c_cfg);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("tdd_sensor_mpu60x0_register, error code: %d.", op_ret);
        return;
    }

    // 调用查找设备接口：查找MPU60x0设备
    SENSOR_HANDLE_T mpu60x0_handle;
    op_ret = tdl_sensor_dev_find("MPU60x0", &mpu60x0_handle);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("tdl_sensor_dev_find, error code: %d.", op_ret);
        return op_ret;
    }

    // 配置MPU60x0初始化命令参数
    SR_IMU_BASE_CFG_T mpu60x0_cfg = {
        .clk_src = SR_IMU_CLK_PLL_XGYRO,
        .gyro_fsr = SR_IMU_GYRO_FSR_250,
        .accel_fsr = SR_IMU_ACCEL_FSR_16,
        .freq_hz = 200
    };
    // 调用配置设备接口：配置MPU60x0设备初始化
    op_ret = tdl_sensor_dev_config(mpu60x0_handle, SR_IMU_CMD_INIT_DEV, &mpu60x0_cfg);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("SR_IMU_CMD_INIT_DEV, error code: %d.", op_ret);
        return;
    }

    return OPRT_OK;
}
```

## 常见问题

暂无。

