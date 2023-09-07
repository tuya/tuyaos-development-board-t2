# tdd_sensor_voltage

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
| 电压 | 0 | V | 由参考电压和配置的分压系数确定 |

- 对应用提供统一的注册接口。

| 接口名称 | 说明 |
| :--- | :--- |
| tdd_sensor_voltage_register | 注册电压传感器。 |

## 组件依赖

| 组件 | 说明 |
| :--- | :--- |
| tdl_sensor_hub | 传感器驱动管理 |
| tkl_adc | ADC 驱动 |
| tal_log | 日志管理 |
| tkl_memory | 内存管理 |

## 目录结构

```xml
├── include                      /* 头文件目录 */
|    └── tdd_sensor_voltage.h    /* 电压传感驱动接口 */
└── src                          /* 源文件目录 */
     └── tdd_sensor_voltage.c    /* 电压传感驱动程序 */
```

## 适配情况

| 传感器型号 | 供应商 | 数据手册 | 驱动方式 | 无效命令 |
| :---: | :---: | :---: | :---: | :---: |
| *通用* | - | - | *ADC* | - |

## 使用说明

### 基本步骤

1. 调用 `tdd_sensor_voltage_register` 注册 xxx 设备；
2. 调用 `tdl_sensor_hub` 组件提供的接口控制 xxx 设备。

### 应用示例

这里仅介绍如何注册电压传感器驱动，其他通用操作请参考 `tdl_sensor_hub` 组件配套的说明文档。

```c
/**
 * @brief 注册电压检测设备
 * @param 无
 * @return 操作结果
 */
STATIC OPERATE_RET __volt_detect_init(VOID_T)
{
    OPERATE_RET op_ret;

    // 配置ADC参数，使用通道2
    TUYA_ADC_BASE_CFG_T adc_cfg;
    tkl_system_memset(&adc_cfg, 0, SIZEOF(TUYA_ADC_BASE_CFG_T));
    adc_cfg.ch_list.bits.ch_2 = TRUE;
    adc_cfg.ch_nums = 1;
    adc_cfg.width = 12;
    adc_cfg.type = TUYA_ADC_INNER_SAMPLE_VOL;
    adc_cfg.mode = TUYA_ADC_SINGLE;
    adc_cfg.conv_cnt = 1;

    // 调用注册设备接口：注册电压检测设备，分压系数设置3.0 (ADC=BAT/3)
    op_ret = tdd_sensor_voltage_register("volt_detect", TUYA_ADC_NUM_0, &adc_cfg, 3.0);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("tdd_sensor_voltage_register, error code: %d.", op_ret);
        return op_ret;
    }

    return OPRT_OK;
}
```

## 常见问题

暂无。
