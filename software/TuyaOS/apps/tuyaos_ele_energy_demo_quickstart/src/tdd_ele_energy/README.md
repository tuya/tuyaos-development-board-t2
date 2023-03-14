# 修改记录

| 版本   | 时间         | 修改内容 |
| ---- | ---------- | ---- |
| v2.0 | 2022.04.20 | 初版   |

# 名词解释

# 基本功能

1. hlw8032、hlw8012、bl0942、bl0937芯片驱动。
2. 向tdl层提供统一的芯片接口。

# 组件依赖

| sdk组件依赖           | 说明        |
| ----------------- | --------- |
| mem_pool.h        | 内存池       |
| uni_log.h         | 日志管理      |
| sys_timer.h       | 软件定时器     |
| tuya_hal_system.h | 系统相关接口    |
| tuya_uart.h       | 串口相关接口    |
| tuya_timer.h      | 硬件定时器相关接口 |
| uni_time_queue.h  | 队列相关接口    |

# 资源依赖

| 资源         | 大小     | 说明          |
| ---------- | ------ | ----------- |
| tdd电量统计初始化 | 约148字节 | 使用hlw8032芯片 |

* hlw8032和bl0942 采样串口通讯，需要用到串口外设
* hlw8012和bl0937采用硬件定时器采集脉冲，需要定时器外设
* 使用硬件定时器脉冲采集的芯片所需要的内存空间，略大于使用串口的芯片

# 接口列表说明

* 电量统计芯片注册接口，详细使用说明参考
  `tdd_ele_energy_hlw8032.h` `tdd_ele_energy_bl0942` `tdd_ele_energy_bl0937_hlw8012.h`

| 接口                                                                                                                                              | 说明            |
| ----------------------------------------------------------------------------------------------------------------------------------------------- | ------------- |
| OPERATE_RET tdd_ele_energy_hlw8032_init(TDL_ELE_ENERGY_CFG_T *drv_cfg, TDD_HLW8032_DRV_RESOURCE_T *dev_info);                                   | 注册hlw8032驱动芯片 |
| OPERATE_RET tdd_ele_energy_bl0942_init(TDL_ELE_ENERGY_CFG_T *drv_cfg, TDD_BL0942_DRV_RESOURCE_T *platform_resource);                            | 注册bl0942驱动芯片  |
| OPERATE_RET tdd_ele_energy_hlw8012_init(IN CONST TDL_ELE_ENERGY_CFG_T *drv_cfg, IN CONST TDD_BL0937_HLW8012_DRV_RESOURCE_T *platform_resource); | 注册hlw8012驱动芯片 |
| OPERATE_RET tdd_ele_energy_bl0937_init(IN CONST TDL_ELE_ENERGY_CFG_T *drv_cfg, IN CONST TDD_BL0937_HLW8012_DRV_RESOURCE_T *platform_resource);  | 注册bl0937驱动芯片  |

# 使用说明

* 注册HLW8032驱动芯片
* HLW8032 与 CBU开发板接线表

| hlw8032 | CBU开发板 | 说明  |
| ------- | ------ | --- |
| RX      | TX     |     |
| TX      | RX     |     |

```c
//hlw8032芯片驱动注册
OPERATE_RET __hlw8032_init(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    TDL_ELE_ENERGY_CFG_T ele_energy_cfg;
    memset(&ele_energy_cfg, 0x00, SIZEOF(ele_energy_cfg));
    ele_energy_cfg.params.resval = 2;       // Reference hardware circuit
    ele_energy_cfg.params.err_percent = 10; // 采用芯片内部默认产测误差

    TDD_HLW8032_DRV_RESOURCE_T bl8032_resource;
    bl8032_resource.uart_no = TUYA_UART0;

    /*判断是否进入产测模式*/
    if (TRUE) {
        ele_energy_cfg.mode = TDL_ELE_TEST_MODE;
    } else {
        ele_energy_cfg.mode = TDL_ELE_NORMAL_MODE;
        PR_DEBUG("ele_energy_cfg.fac_test, ret:%d, v_coe:%d, i_coe%d, p_coe%d, e_coe%d", \
            ele_energy_cfg.fac_test.test_result, ele_energy_cfg.fac_test.coe_data.v_coe, \
            ele_energy_cfg.fac_test.coe_data.i_coe, ele_energy_cfg.fac_test.coe_data.p_coe,\
            ele_energy_cfg.fac_test.coe_data.e_coe);
    }
    ele_energy_cfg.params.volt_type = TDL_ELE_VOLT_220V;  //Supply voltage 220V or 120V
    op_ret = tdd_ele_energy_hlw8032_init(&ele_energy_cfg, &bl8032_resource);
    return op_ret;   
}
```

* 注册HLW8012驱动芯片
* HLW8012 与 CBU开发板接线表

| hlw8012 | CBU开发板      | 说明                            |
| ------- | ----------- | ----------------------------- |
| CF      | TY_GPIOA_8  | 功率检测引脚                        |
| CF1     | TY_GPIOA_17 | 电压电流检测引脚                      |
| SEL     | TY_GPIOA_14 | 选择引脚，改变此引脚状态，就改变CF1读取电压或者读取电流 |

```c
OPERATE_RET __hwl8012_init(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;
    TDL_ELE_ENERGY_CFG_T ele_energy_cfg;
    memset(&ele_energy_cfg, 0x00, SIZEOF(ele_energy_cfg));
    ele_energy_cfg.params.volt_type = TDL_ELE_VOLT_220V;   // 220 or120  V
    ele_energy_cfg.params.resval = 1;
    ele_energy_cfg.params.err_percent = 0; // 采用芯片内部默认产测误差
    /*判断是否进入产测模式*/
    if (FALSE) {
        ele_energy_cfg.mode = TDL_ELE_TEST_MODE;
    } else {
        ele_energy_cfg.mode = TDL_ELE_NORMAL_MODE;
        PR_DEBUG("ele_energy_cfg.fac_test, ret:%d, v_coe:%d, i_coe%d, p_coe%d, e_coe%d", \
            ele_energy_cfg.fac_test.test_result, ele_energy_cfg.fac_test.coe_data.v_coe, \
            ele_energy_cfg.fac_test.coe_data.i_coe, ele_energy_cfg.fac_test.coe_data.p_coe,\
            ele_energy_cfg.fac_test.coe_data.e_coe);
    }

    TDD_BL0937_HLW8012_DRV_RESOURCE_T bl8012_resource;
    //脉冲的采集使用硬件定时器来进行采集
    bl8012_resource.timer_no = TUYA_TIMER0;     
    bl8012_resource.epin = TY_GPIOA_8;//功率引脚
    bl8012_resource.ivpin = TY_GPIOA_17;//电压电流引脚
    bl8012_resource.ivcpin.pin = TY_GPIOA_14;//选择引脚
    bl8012_resource.ivcpin.type = TDL_ELE_IO_DRV_HIGH; //选择引脚有效电平
    op_ret = tdd_ele_energy_hlw8012_init(&ele_energy_cfg, &bl8012_resource);

    return op_ret;
}
```

# 常见问题

A：

Q：

# 相关链接

- [Tuya IoT 文档中心](https://developer.tuya.com/cn/docs/iot)
- [涂鸦IoTCBU开发板](https://developer.tuya.com/cn/docs/iot/tuya-sandwich-wifi-and-ble-soc-board-soc?id=Kalj9yvvnhd8p)
- [Wi-Fi 模组 SDK 二次开发教程](https://developer.tuya.com/cn/docs/iot/Module-SDK-development_tutorial?id=Kauqptzv5yo8a)
