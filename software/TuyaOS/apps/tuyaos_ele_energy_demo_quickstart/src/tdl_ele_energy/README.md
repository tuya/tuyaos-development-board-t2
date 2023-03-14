# 修改记录

| 版本   | 时间         | 修改内容 |
| ---- | ---------- | ---- |
| v2.0 | 2022.04.20 | 初版   |

# 名词解释

| 名词   | 解释           |
| ---- | ------------ |
| 电量统计 | 对电量的使用情况进行统计 |

# 基本功能

1. 支持4款电量统计芯片驱动。
2. 支持220V或者120V额定电压下的，电压、电流、功率采样。
3. 在参数校准之后，参数的采样精度大约3%。

# 组件依赖

| sdk组件依赖     | 说明    |
| ----------- | ----- |
| mem_pool.h  | 内存池   |
| uni_log.h   | 日志管理  |
| sys_timer.h | 软件定时器 |

| 应用组件依赖         | 说明                                          |
| -------------- | ------------------------------------------- |
| tdd_ele_energy | 电量统计tdd层。hlw8032、hlw8012、bl0937、bl0942的芯片驱动 |

# 资源依赖

| 资源         | 大小     | 说明          |
| ---------- | ------ | ----------- |
| tdl电量统计初始化 | 约184字节 | 使用hlw8032芯片 |

# 接口列表说明

* 电量统计管理、操作接口，详细使用说明参考
  `tdl_ele_energy_ops.h`

| 接口                                                                                           | 说明           |
| -------------------------------------------------------------------------------------------- | ------------ |
| OPERATE_RET tdl_ele_energy_driver_register(TDL_ELE_ENERGY_DRIVER_T *chip_info);              | 芯片驱动注册到TDD层  |
| OPERATE_RET tdl_ele_energy_ctrl(BOOL_T state);                                               | 开启或关闭数据采集    |
| OPERATE_RET tdl_ele_energy_read_fac_test_data(OUT TDL_ELE_ENERGY_FAC_TEST_T *fac_test_data); | 读产测数据校准      |
| BOOL_T tdl_ele_energy_read_fac_test_state(VOID_T);                                           | 读产测状态        |
| VOID_T tdl_ele_energy_read_pvi_value(OUT UINT_T *pwr, OUT UINT_T *volt, OUT UINT_T *curr);   | 获取电压、电流、功率数据 |
| VOID_T tdl_ele_energy_read_enery_value(OUT UINT_T *enry);                                    | 获取累计电量值      |
| BOOL_T tdl_ele_energy_rang_judge(IN UINT_T src_value, IN UINT_T tag_value, IN UINT_T range); | 电量统计范围判断     |

* 驱动芯片接口，详细使用说明参考
  `tdd_ele_energy_hlw8032.h` `tdd_ele_energy_bl0942` `tdd_ele_energy_bl0937_hlw8012.h`
  
  | 接口                                                                                                                                              | 说明            |
  | ----------------------------------------------------------------------------------------------------------------------------------------------- | ------------- |
  | OPERATE_RET tdd_ele_energy_hlw8032_init(TDL_ELE_ENERGY_CFG_T *drv_cfg, TDD_HLW8032_DRV_RESOURCE_T *dev_info);                                   | 注册hlw8032驱动芯片 |
  | OPERATE_RET tdd_ele_energy_bl0942_init(TDL_ELE_ENERGY_CFG_T *drv_cfg, TDD_BL0942_DRV_RESOURCE_T *platform_resource);                            | 注册bl0942驱动芯片  |
  | OPERATE_RET tdd_ele_energy_hlw8012_init(IN CONST TDL_ELE_ENERGY_CFG_T *drv_cfg, IN CONST TDD_BL0937_HLW8012_DRV_RESOURCE_T *platform_resource); | 注册hlw8012驱动芯片 |
  | OPERATE_RET tdd_ele_energy_bl0937_init(IN CONST TDL_ELE_ENERGY_CFG_T *drv_cfg, IN CONST TDD_BL0937_HLW8012_DRV_RESOURCE_T *platform_resource);  | 注册bl0937驱动芯片  |

# 使用说明

电量采集的数据采集，目前主流就分为两种，一种是串口通讯，另一种是脉冲采集
本readme只展示 hlw8032 和 hlw8012 . 前者通过串口读取数据，后者通过对脉冲采集读取数据。

* hlw8032 demo
* HLW8032 与 CBU开发板接线表

| hlw8032 | CBU开发板 | 说明  |
| ------- | ------ | --- |
| RX      | TX     |     |
| TX      | RX     |     |

```c
OPERATE_RET __hwl8032_init(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;
    //hlw8032参数配置结构体。
    TDL_ELE_ENERGY_CFG_T ele_energy_cfg;
    memset(&ele_energy_cfg, 0x00, SIZEOF(ele_energy_cfg));
    //采集电路上的电阻    1  or   2
    ele_energy_cfg.params.resval = 2;       
    ele_energy_cfg.params.err_percent = 10; // 采用芯片内部默认产测误差
    //由于串口通讯，hwl8032只需要配置一下串口就行。
    TDD_HLW8032_DRV_RESOURCE_T bl8032_resource;
    //注意CBU开发板上的 串口1，对应此处的串口0
    bl8032_resource.uart_no = TUYA_UART0;

    /*判断是否进入产测模式*/
    if (FLASE) {
        ele_energy_cfg.mode = TDL_ELE_TEST_MODE;
    } else {
        ele_energy_cfg.mode = TDL_ELE_NORMAL_MODE;
        PR_DEBUG("ele_energy_cfg.fac_test, ret:%d, v_coe:%d, i_coe%d, p_coe%d, e_coe%d", \
            ele_energy_cfg.fac_test.test_result,   ele_energy_cfg.fac_test.coe_data.v_coe, \
            ele_energy_cfg.fac_test.coe_data.i_coe, ele_energy_cfg.fac_test.coe_data.p_coe,\
            ele_energy_cfg.fac_test.coe_data.e_coe);
    }
    //根据供电电压选择 220V 还是 120 V
    ele_energy_cfg.params.volt_type = TDL_ELE_VOLT_220V;  
    //调用初始化函数
    op_ret = tdd_ele_energy_hlw8032_init(&ele_energy_cfg, &bl8032_resource);
    return op_ret;    
}

//定时器回调函数，读取参数
VOID_T hlw8032_timer_cb(UINT_T timerID, PVOID_T pTimerArg)
{
    PR_DEBUG("-----------hwl8032 read value callback function  ------------");
    STATIC UINT_T sg_power = 0;
    STATIC UINT_T sg_voltage = 0;
    STATIC UINT_T sg_current = 0;
    STATIC UINT_T sg_enery = 0;

    tdl_ele_energy_read_pvi_value(&sg_power, &sg_voltage, &sg_current);
    tdl_ele_energy_read_enery_value(&sg_enery);

    PR_DEBUG("power = %d", sg_power);
    PR_DEBUG("voltage = %d", sg_voltage);
    PR_DEBUG("current = %d", sg_current);
    PR_DEBUG("enery = %d", sg_enery);  //Read the electricity in a short time, the result is 0

}


VOID_T app_hlw8032_test(VOID_T)
{
    //初始化hlw8032
    OPERATE_RET op_ret = OPRT_OK;
    op_ret = __hlw8032_init();
    if(OPRT_OK != op_ret) {
        PR_ERR("err<%d>,hwl8032 init fial !!!");
        return ;
    }
    //打开定时器 5s采集一次数据
    //*注意，电量的统计在短时间内是统计出来的，读取的值会一直都是0 
    op_ret = sys_add_timer(hlw8032_timer_cb, NULL, &hlw8032_timer);
    if (OPRT_OK != op_ret) {
        PR_ERR("ele_energy timer init err:%d", op_ret);
        return ;
    }
    sys_start_timer(hlw8032_timer, 5000, TIMER_CYCLE);
}
```

* hlw8012 demo

* HLW8012 与 CBU开发板接线表

| hlw8012 | CBU开发板      | 说明                            |
| ------- | ----------- | ----------------------------- |
| CF      | TY_GPIOA_8  | 功率检测引脚                        |
| CF1     | TY_GPIOA_17 | 电压电流检测引脚                      |
| SEL     | TY_GPIOA_14 | 选择引脚，改变此引脚状态，就改变CF1读取电压或者读取电流 |

```c
/*hlw8012 只是tdd层的初始化不同。
其余和hlw8032demo一样！所以本demo只介绍 hlw8012初始化*/

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

- [Wi-Fi 模组 SDK 二次开发教程](https://developer.tuya.com/cn/docs/iot/Module-SDK-development_tutorial?id=Kauqptzv5yo8a)
