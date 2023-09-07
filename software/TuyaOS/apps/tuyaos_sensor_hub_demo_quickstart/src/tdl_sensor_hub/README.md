# tdl_sensor_hub

## 修改记录

| 版本 | 时间 | 修改内容 |
| :--- | :--- | :--- |
| v0.1 | 2022/09/07 | 初版 |

## 名词解释

| 名词 | 解释 |
| :--- | :--- |
| 元素 | 传感器采集的某一类数据可以根据其物理含义抽象为一种元素 (element)，如温度。 |
| 透传 | 即透明传输，透传型 (raw) 数据就是以二进制形式进行存储并传输的数据，存储格式可自定义。 |
| 订阅 | 应用程序向传感器管理程序预定传感数据的方式，使传感器管理程序根据设定的订阅规则上报数据。 |
| 过滤 | 滤除部分数据的过程，传感器管理程序会根据应用程序设置的规则上报过滤后的数据。 |

## 基本功能

`tdl_sensor_hub` 组件用于对传感器驱动进行统一管理，其功能设计如下：

- 对应用和传感驱动提供通用的传感数据抽象类型。

| 数据类型 | 说明（采用哪一种类型由传感器实例决定） |
| :--- | :--- |
| 元素型 | 以元素为单位进行数据传输，元素型数据包括：<br/>1）元素 ID，由传感器实例为每个元素分配 ID；<br/>2）元素值，由传感器实例为每个元素定义元素值的类型，包括整型和浮点型。 |
| 透传型 | 以二进制形式进行数据传输，透传型数据包括：<br/>1）数据长度，以字节为单位，由传感器实例设定；<br/>2）透传数据，由传感器实例设定每一位的存储内容。 |

- 对传感驱动提供统一的注册接口和操作接口抽象类型。

| 接口功能 | 说明 |
| :--- | :--- |
| 注册设备 | 用于为不同类型传感器提供统一的注册通道。<br/>Sensor Hub 会创建新的设备节点来存储驱动层上报的信息，并将其挂载到驱动管理列表中；<br/>传感驱动层需上报的信息包括：<br/>1）设备名称（由应用定义并下发给驱动层）；<br/>2）元素信息（该传感器固有的元素及其数据类型）；<br>3）操作接口（需符合 Sensor Hub 定义的接口抽象类型）；<br>4）资源信息（外设驱动配置等需要存储的信息）。 |

- 对应用提供统一的传感交互接口。

| 接口功能 | 说明 |
| :--- | :--- |
| 查找设备 | 用于确认设备是否已成功注册。<br/>Sensor Hub 会返回已注册设备的操作句柄给应用，以用于后续的设备操作。<br/>注：这里不涉及 Sensor Hub 与驱动层的交互。 |
| 打开设备 | 用于打开设备的采集功能。<br/>Sensor Hub 会为设备准备采集过程中需要的资源，并通知驱动启动采集工作。 |
| 关闭设备 | 用于关闭设备的采集功能。<br/>Sensor Hub 会通知驱动停止采集工作，并释放不再使用的资源。 |
| 配置设备 | 用于对设备进行配置，同时也可作为读取设备状态的接口，这取决于传感器其自身所具备的能力。<br/>Sensor Hub 仅作为应用和驱动交互的传输通道，不会进行其他操作。 |
| 读取设备 | 用于读取传感器的实时数据，以应对一些需要即时获取数据的应用场景。<br/>Sensor Hub 会通知驱动层采集并即时返回数据。 |

- 对应用提供通用的采集策略。

| 采集策略 | 说明 |
| :--- | :--- |
| 触发模式 | 触发传感器管理程序读取传感数据的模式，分别为以下 3 种：<br/>1）软件定时轮询，应用可设置轮询间隔；<br/>2）硬件定时轮询，应用可设置轮询间隔；<br/>3）外部引脚中断，应用可设置中断引脚和中断模式。 |
| 通知回调 | 数据就绪时通知应用的回调函数，按数据类型配置；<br/>默认模式下按传感器实例注册的元素顺序上报所有元素，订阅模式下按应用配置的订阅规则上报。 |
| 缓存大小<br>（可选） | FIFO 缓存区大小，溢出时将数据上报给应用，不设置则默认为 1。 |
| 订阅模式<br>（可选） | 数据上报规则，应用可通过该模式设置想要订阅的元素、上报方式和过滤规则等。**注：仅用于元素型数据。** |

- 对应用提供可自由配置数据上报规则的订阅模式。

| 订阅配置 | 说明 |
| :--- | :--- |
| 订阅类型 | 要求传感器管理程序上报应用订阅数据的方式，分别为以下 2 种：<br/>1）组合订阅，以组的形式上报应用订阅的所有元素的数据；<br/>2）单独订阅，单独上报应用订阅的每个元素的数据。 |
| 订阅个数 | 订阅规则的个数，同时也是订阅元素的个数，每个元素对应一套规则。 |
| 订阅规则 | 应用为每个订阅的元素设置的规则，规则内容包括：<br/>1）元素 ID，用于指定该规则绑定的元素；<br/>2）元素值个数，即每次上报的个数，不可超过“缓存大小”；不设置则默认为 1；<br/>&emsp;&nbsp; 组合订阅时无效，上报个数以“缓存大小”为标准；<br/>3）过滤规则，可设置最大值、最小值和步进值；不设置则默认为不过滤；<br/>&emsp;&nbsp; 组合订阅时，只要其中一个元素的值满足其过滤规则时，同一时刻下采集的所有元素的值都将被过滤。 |

## 组件依赖

| 组件 | 说明 |
| :--- | :--- |
| tkl_mutex | 锁管理 |
| tkl_thread | 线程管理 |
| tkl_semaphore | 信号管理 |
| tkl_memory | 内存管理 |
| tkl_gpio | GPIO 驱动 |
| tkl_timer | 硬件定时器驱动 |
| tal_sw_timer | 系统定时器管理 |
| tal_log | 日志管理 |

## 资源依赖

| 资源 | 占用情况 |
| :--- | :--- |
| 外设 | GPIO：每个采用中断采集策略的传感器占用 1 个；TIMER：最多 1 个；|
| 内存 | 完成一个传感注册并打开 (软件定时触发、缓存大小为 1、订阅元素 2 个)，内存消耗约 1.6k。 |

## 目录结构

```xml
├── include         /* 头文件目录 */
|    ├── tdd_sensor_i2c.h                      /* 软件I2C驱动接口(临时存放) */
|    ├── tdl_sensor_driver.h                   /* SensorHub 面向驱动接口 */
|    ├── tdl_sensor_hub.h                      /* SensorHub 面向应用接口 */
|    └── tdl_sensor_type.h                     /* SensorHub 数据类型抽象 */
└── src             /* 源文件目录 */
     ├── tdd_sensor_i2c.c                      /* 软件I2C驱动(临时存放) */
     └── tdl_sensor_hub.c                      /* SensorHub 驱动 */
```

## 接口列表

- tdl_sensor_driver.h（面向驱动）

| 接口名称 | 说明 |
| :--- | :--- |
| tdl_sensor_dev_register | 注册传感设备。<br/>>> [in] 设备名称；<br/>>> [in] 接口抽象 (open / close / control / read_ele / read_raw)；<br/>>> [in] 元素个数 (元素个数为 0 时表示采用透传型数据)；<br/>>> [in] 元素配置 (元素 ID / 元素值)；<br/>>> [in] 资源信息 (资源类型 / 资源端口 / 资源句柄 / 其他自定义信息)。 |

- tdl_sensor_hub.h （面向应用）

| 接口名称 | 说明 |
| :--- | :--- |
| tdl_sensor_dev_find | 查找传感设备。<br/>>> [in] 设备名称；<br/>>> [out] 设备句柄。 |
| tdl_sensor_dev_open | 启动传感设备。<br/>>> [in] 设备句柄；<br/>>> [in] 设备配置 (采集策略)。 |
| tdl_sensor_dev_close | 停止传感设备。<br/>>> [in] 设备句柄。 |
| tdl_sensor_dev_config | 配置/控制传感设备。<br/>>> [in] 设备句柄；<br/>>> [in] 控制命令；<br/>>> [inout] 命令参数。 |
| tdl_sensor_dev_read | 读取传感设备。<br/>>> [in] 设备句柄；<br/>>> [in] 元素个数；<br/>>> [inout] 元素型数据；<br/>>> [out] 透传型数据。 |

## 使用说明

需配合 `tdd_sensor_xxx` 组件使用。

### 基本步骤

1. 调用 `tdd_sensor_xxx_register` 注册 xxx 设备；
2. 调用 `tdl_sensor_dev_find` 查找 xxx 设备（确认 xxx 设备注册成功，或者在使用其他接口前通过 find 接口获得设备句柄）；
3. 调用 `tdl_sensor_dev_config` 配置 xxx 设备（可用的配置命令取决于 `tdd_sensor_xxx` 组件的设计）；
4. 调用 `tdl_sensor_dev_open` 启动 xxx 设备并注册数据通知回调函数（需要启动采集时）；在回调时处理采集到的数据；
5. 调用 `tdl_sensor_dev_read` 读取 xxx 设备的实时数据（有需要时）；
6. 调用 `tdl_sensor_dev_close` 停止 xxx 设备（需要停止采集时）。

### 适配说明

如果在产品开发包中没有找到合适的传感驱动组件，也可自行编写驱动层代码，可参考现有的组件代码，并按照以下步骤进行适配：

1. 确定传感数据存储方式，如果选择元素型，则需确定有哪些元素及每个元素值的数据类型；
2. 确定传感驱动的外设 类型和需要配置的内容，比如 I2C、SPI ...；
3. 确定注册设备时需要做哪些处理，有哪些资源信息需要存至 Sensor Hub，实现 `tdd_sensor_xxx_register` 接口；
4. 确定传感是否有启动测量和停止测量命令，实现 `open` 和 `close` 接口；
5. 确定传感读数据的过程，实现 `read_ele` 或 `read_raw` 接口；
6. 确定除读数据外是否需要增加其他控制操作，实现 `control` 接口。

### 应用示例

以温湿度传感器 SHT30 为例，需搭配 `tdd_sensor_temp_humi` 组件使用。

- Demo 整体框架

```c
/**
 * @brief 需包含的头文件
 */
#include "tdl_sensor_hub.h"
#include "tdd_sensor_temp_humi.h"
#include "tkl_memory.h"
#include "tal_log.h"
#include "tal_sw_timer.h"

/**
 * @brief 全局变量定义
 */
STATIC SENSOR_HANDLE_T sg_sht30_handle = NULL;
STATIC TIMER_ID sg_sht30_timer;

/**
 * @brief 定时回调
 * @param[in] timer_id: 定时器ID
 * @param[in] arg: 定时器参数
 * @return 无
 */
STATIC VOID_T __sht30_timer_cb(TIMER_ID timer_id, VOID_T *arg)
{
    // 配置SHT30设备
    __sht30_config();
    // 读取实时数据
    __sht30_read();
    // 停止SHT30设备
    __sht30_close();
}

/**
 * @brief 温湿度应用程序初始化
 * @param 无
 * @return 操作结果
 */
OPERATE_RET app_temp_humi_init(VOID_T)
{
    OPERATE_RET op_ret;

    // 注册并启动SHT30设备
    op_ret = __sht30_register();
    if (OPRT_OK != op_ret) {
        return op_ret;
    }
    op_ret = __sht30_open();
    if (OPRT_OK != op_ret) {
        return op_ret;
    }

    // 创建定时器并启动：20秒后停止SHT30设备
    op_ret = tal_sw_timer_create(__sht30_timer_cb, NULL, &sg_sht30_timer);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("tal_sw_timer_create -- sg_sht30_timer, error code: %d.", op_ret);
        return op_ret;
    } else {
        tal_sw_timer_start(sg_sht30_timer, 20*1000, TIMER_ONCE);
    }

    return OPRT_OK;
}
```

- 注册 SHT30 设备

```c
/**
 * @brief 注册SHT30设备
 * @param 无
 * @return 操作结果
 */
STATIC OPERATE_RET __sht30_register(VOID_T)
{
    OPERATE_RET op_ret;

    // 配置注册SHT30设备时需传入的参数 (取决于tdd层组件设计，此处省略)

    // 调用注册设备接口：注册SHT30设备
    op_ret = tdd_sensor_sht3x_register("SHT30", sht30_i2c_cfg, sht30_meas_cfg);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("tdd_sensor_sht3x_register, error code: %d.", op_ret);
        return op_ret;
    }

    // 调用查找设备接口：查找SHT30设备
    op_ret = tdl_sensor_dev_find("SHT30", &sg_sht30_handle);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("tdl_sensor_dev_find, error code: %d.", op_ret);
        return op_ret;
    }

    return OPRT_OK;
}
```

- 打开 SHT30 设备（一般模式）

```c
/**
 * @brief 启动SHT30设备
 * @param 无
 * @return 操作结果
 */
STATIC OPERATE_RET __sht30_open(VOID_T)
{
    OPERATE_RET op_ret;

    // 配置注册参数：软件定时轮询，间隔1秒，缓存大小1，不使用订阅
    SR_DEV_CFG_T sht30_cfg;
#if 0
    // 写法1（fifo_size=0时SensorHub内部会默认设置为1）
    tkl_system_memset(&sht30_cfg, 0, SIZEOF(SR_DEV_CFG_T));
    sht30_cfg.mode.trig_mode = SR_MODE_POLL_SOFT_TM;
    sht30_cfg.mode.poll_intv_ms = 1000;
    sht30_cfg.inform_cb.ele = __sht30_inform_cb;
#else
    // 写法2
    sht30_cfg.mode.trig_mode = SR_MODE_POLL_SOFT_TM;
    sht30_cfg.mode.poll_intv_ms = 1000;
    sht30_cfg.inform_cb.ele = __sht30_inform_cb;
    sht30_cfg.fifo_size = 1;
    sht30_cfg.ele_sub = NULL;
#endif

    // 调用启动设备接口：启动SHT30设备
    op_ret = tdl_sensor_dev_open(sg_sht30_handle, &sht30_cfg);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("tdl_sensor_dev_open, error code: %d.", op_ret);
        return op_ret;
    }

    TAL_PR_INFO("Open device successfully.");
    return OPRT_OK;
}
```

- 打开 SHT30 设备（订阅模式）

```c
/**
 * @brief 启动SHT30设备
 * @param 无
 * @return 操作结果
 */
STATIC OPERATE_RET __sht30_open(VOID_T)
{
    OPERATE_RET op_ret;

    // 配置注册参数：软件定时轮询，间隔1秒，缓存大小5，使用订阅
    SR_DEV_CFG_T sht30_cfg;
    sht30_cfg.mode.trig_mode = SR_MODE_POLL_SOFT_TM;
    sht30_cfg.mode.poll_intv_ms = 1000;
    sht30_cfg.inform_cb.ele = __sht30_inform_cb;
    sht30_cfg.fifo_size = 5;

    // 选择订阅2个元素
    SR_ELE_SUB_CFG_T ele_sub;
    ele_sub.num = 2;

    SR_ELE_SUB_RULE_T ele_sub_rule[2];
    tkl_system_memset(ele_sub_rule, 0, SIZEOF(ele_sub_rule));

    ele_sub.rule = ele_sub_rule;
    sht30_cfg.ele_sub = &ele_sub;

#if 1
    // 单独订阅，val_num省略时默认为1，filter省略时默认为不过滤
    ele_sub.tp = SR_ELE_SUB_TP_SINGLE;

    ele_sub_rule[0].id = SR_TH_ELE_ID_TEMP;
    ele_sub_rule[0].val_num = 2;
    ele_sub_rule[0].filter.f.max = 60.0f;
    ele_sub_rule[0].filter.f.min = 10.0f;
    ele_sub_rule[0].filter.f.step = 0.0f;

    ele_sub_rule[1].id = SR_TH_ELE_ID_HUMI;
    ele_sub_rule[1].val_num = 5;
    ele_sub_rule[1].filter.f.max = 80.0f;
    ele_sub_rule[1].filter.f.min = 20.0f;
    ele_sub_rule[1].filter.f.step = 0.5f;
#else
    // 组合订阅，val_num无效，以fifo_size为准，filter省略时默认为不过滤
    // 组合订阅下的filter规则：只要其中一个元素满足其过滤规则时，同一时刻采集到的所有元素的值都过滤
    ele_sub.tp = SR_ELE_SUB_TP_GROUP;
    ele_sub_rule[0].id = SR_TH_ELE_ID_TEMP;
    ele_sub_rule[1].id = SR_TH_ELE_ID_HUMI;
#endif

    // 调用启动设备接口：启动SHT30设备
    op_ret = tdl_sensor_dev_open(sg_sht30_handle, &sht30_cfg);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("tdl_sensor_dev_open, error code: %d.", op_ret);
        return op_ret;
    }

    TAL_PR_INFO("Open device successfully.");
    return OPRT_OK;
}
```

- SHT30 设备数据通知回调

```c
/**
 * @brief SHT30设备数据通知回调
 * @param[in] name: 设备名称
 * @param[in] ele_num: 元素个数
 * @param[in] ele_data: 元素数据
 * @return 无
 */
STATIC VOID_T __sht30_inform_cb(CHAR_T* name, UCHAR_T ele_num, SR_ELE_BUFF_T *ele_data)
{
#if 1
    // 通用写法
    UCHAR_T i, j;
    TAL_PR_DEBUG_RAW("\r\n");
    TAL_PR_INFO("---------- SHT30: inform callback ----------");
    TAL_PR_INFO("sensor name: %s", name);
    TAL_PR_INFO("ele_num: %d", ele_num);

    for (i = 0; i < ele_num; i++) {
        if (ele_data[i].id == SR_TH_ELE_ID_TEMP) {
            TAL_PR_INFO("Temp:");
        } else {
            TAL_PR_INFO("Humi:");
        }
        for (j = 0; j < ele_data[i].val_num; j++) {
            if (SR_VAL_TP_FLOAT == ele_data[i].val_tp) {
                TAL_PR_DEBUG_RAW("%.1f", ele_data[i].val[j].sr_float);
            } else {
                TAL_PR_DEBUG_RAW("%d", ele_data[i].val[j].sr_int);
            }
        }
        TAL_PR_DEBUG_RAW("\r\n");
    }
#else
    // 简单写法 (条件：fifo_size = 1, ele_sub = NULL)
    TAL_PR_INFO("Temp: %.1f", ele_data[0].val[0].sr_float);
    TAL_PR_INFO("Humi: %.1f", ele_data[1].val[0].sr_float);
#endif
}
```

- 配置 SHT30 设备

```c
/**
 * @brief 配置SHT30设备 (以设置采样精度为例)
 * @param 无
 * @return 操作结果
 */
STATIC OPERATE_RET __sht30_config(VOID_T)
{
    OPERATE_RET op_ret;

    // 调用配置设备接口：设置SHT30的采样精度 (配置命令由tdd层决定)
    SR_TH_PREC_E sht30_prec = SR_TH_PREC_MEDIUM;
    op_ret = tdl_sensor_dev_config(sg_sht30_handle, SR_TH_CMD_SET_PREC, &sht30_prec);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("SR_TH_CMD_SET_PREC, error code: %d.", op_ret);
        return op_ret;
    }

    return OPRT_OK;
}
```

- 读取实时数据

```c
/**
 * @brief 读取SHT30设备
 * @param 无
 * @return 操作结果
 */
STATIC OPERATE_RET __sht30_read(VOID_T)
{
    // 设置的元素值和传感驱动注册的元素值一致时可省略id的配置
    SR_ELE_DATA_T ele_data[2];
    ele_data[0].id = SR_TH_ELE_ID_TEMP;
    ele_data[1].id = SR_TH_ELE_ID_HUMI;

    // 调用读取设备接口：读取实时数据
    op_ret = tdl_sensor_dev_read(sg_sht30_handle, 2, ele_data, NULL);
    if (OPRT_OK == op_ret) {
        for (UCHAR_T i = 0; i < 2; i++) {
            TAL_PR_INFO("ele id: %d, ele val: %.1f", ele_data[i].id, ele_data[i].val.sr_float);
        }
    }
}
```

- 关闭 SHT30 设备

```c
/**
 * @brief 停止SHT30设备
 * @param 无
 * @return 操作结果
 */
STATIC OPERATE_RET __sht30_close(VOID_T)
{
    // 调用停止设备接口：停止SHT30设备
    OPERATE_RET op_ret = tdl_sensor_dev_close(sg_sht30_handle);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("tdl_sensor_dev_close, error code: %d.", op_ret);
        return op_ret;
    }

    TAL_PR_INFO("Close device successfully.");
    return OPRT_OK;
}
```

## 常见问题

暂无。

