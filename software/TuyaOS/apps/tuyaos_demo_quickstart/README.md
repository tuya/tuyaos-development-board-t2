author: Tuya
summary: quickstart demo
id: tuyaos_demo_quickstart
categories: quickstart,markdown
environments: Web
status: Published

# tuyaos_demo_quickstart

## 简介

`tuyaos_demo_quickstart`  示例的目的是在体现 TuyaOS 单品开发的基本能力下，提供一个尽可能简单的示例。方便开发者借助此示例熟悉 TuyaOS 开发流程，验证手上的模组是否能够正常联网。

## 环境准备

#### 硬件环境

在 `tuya_app_main.c` 中可以修改示例的配网重置引脚和配网指示灯引脚。

| 丝印名称 | 备注                            |
|:----:|:-----------------------------:|
| P17  | LED 引脚，高电平点亮                  |
| P7   | 按键引脚，低电平有效，短按切换指示灯状态，长按进入配网状态 |

> 注意：这里的 GPIO 设置需要开发者根据不同模组实际情况进行配置。

#### 软件环境

使用 `Tuya Wind IDE` 工具获取到此演示程序的开发者，不需要额外的环境准备。其他渠道获取此演示程序的开发者，可以参照[环境搭建](https://developer.tuya.com/cn/docs/iot-device-dev/tuyaos-wind-ide?id=Kbfy6kfuuqqu3)指引，来完成开发环境的搭建。

## 文件介绍

### 常用头文件介绍

| 头文件名称                   | 功能                                                                  |
|:-----------------------:|:-------------------------------------------------------------------:|
| `tal_log.h`             | TuyaOS 日志打印。                                                        |
| `tuya_iot_wifi_api.h`   | 提供与 Wi-Fi 相关的接口。常用 API 有初始化 TuyaOS 框架进行设置 Wi-Fi的工作模式，重置设备再次进入配网状态等。 |
| `tal_system.h`          | 系统接口封装。常用 API 有得到系统重启原因，得到系统运行的 ticket 等。                           |
| `tuya_error_code.h`     | 涂鸦对一些错误类型的定义                                                        |
| `tuya_cloud_com_defs.h` | 一些与云端相关的类型定义。                                                       |
| `tuya_cloud_types.h`    | 对参数类型的封装。对变量、函数进行类型定义或修饰时应调用这里的函数。                                  |
| `tkl_gpio.h`            | 对 GPIO 的 API 进行了封装，需在 TuyaOS 初始化完成后调用。                              |

### device文件介绍

此演示程序在 `tuya_app_main.c` 文件里实现了 `Wi-Fi SoC` 基本功能：

| 函数名称                            | 函数功能                                                                     |
|:------------------------------- |:------------------------------------------------------------------------ |
| `tuya_app_main`                 | 此函数作为统一 app 入口，该函数为开发者唯一需要实现的函数。此函数会调用  `user_main` 函数，开发者可在此函数中做相关设备处理。 |
| `__soc_device_init`             | 设备初始化接口，初始化设备工作模式、配网模式，产品 PID 和版本信息。应用必须对其进行实现，如果不需要，则实现空函数。             |
| `__soc_dev_status_changed_cb`   | 设备运行状态变化回调。                                                              |
| `__soc_dev_rev_upgrade_info_cb` | 设备升级启动回调，此接口是升级入口。                                                       |
| `__soc_dev_restart_req_cb`      | 设备重启请求，设备有重启需求之前会调用此接口让应用程序做好重启准备。                                       |
| `__soc_dev_obj_dp_cmd_cb`       | 设备 DP 命令处理，包含一个或者多个DP，需要对DP命令进行处理。                                       |
| `__soc_dev_raw_dp_cmd_cb`       | 设备 RAW DP 命令处理，RAW DP 是一种特殊 DP，传递自定义的原始数据，可以是字符串、二进制、数值等各种类型数据。          |
| `__soc_dev_dp_query_cb`         | 设备 DP 查询，此回调是响应云端、APP 查询命令，虚返回对应 DP 的值，当查询的 DP 数量为0的时候，需要返回全部 DP。        |
| `__soc_dev_net_status_cb`       | 设备网络连接状态变化回调，通知应用设备网络连接状态发生变化。                                           |

#### `__soc_device_init`

开发者需要特别注意 `__soc_device_init` 函数中的这段代码。由于这只是一个 demo，所以开发者需要手动修改 UUID 和 AUTHKEY。在产品批量生成的过程会使用更加方便授权工具，开发者无需关心这个问题。

```c
#define PID "uiiyltkjmkhwumga"
#define UUID "tuyaxxxxxxxxxxxxa4c6"
#define AUTHKEY "GPwIxxxxxxxxxxxxxxxxxxxxxxxx1maj"
/*...*/
STATIC VOID_T __soc_device_init(VOID_T)
{
   /*...*/
    WF_GW_PROD_INFO_S prod_info = {UUID, AUTHKEY};
    op_ret = tuya_iot_set_wf_gw_prod_info(&prod_info);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_set_gw_prod_info err:%d", op_ret);
        return -2;
    }
    /*...*/
}
```

#### `tuya_iot_init_params`

```
/**
 * @brief tuya_iot_init_params 
 * @desc init tuyaos framework
 *
 * @param[in] fs_storge_path: filesystem read write storge path
 *            (if os have no fs,then fs_storge_path is invalid)
 * @param[in] p_param: custom init params
 *
 * @return OPRT_OK: success  Other: fail
 */
OPERATE_RET tuya_iot_init_params(IN CONST CHAR_T *fs_storge_path, IN CONST TY_INIT_PARAMS_S *p_param);
```

此接口是 `TuyaOS` 的初始化接口，接口内部会对 `TuyaOS` 的内部基础功能进行资源分配、初始化。一般来说，这个接口会第一个调用。开发者如果有一些需要尽早启动，并且和 `TuyaOS` 无关的逻辑，可以在此接口调用之前执行。

* `CHAR_T *fs_storge_path`：数据存储路径，仅在`Linux` 系统下使用，如果不提供，则默认在程序当前路径下创建数据库目录 `tuya_db_files`。

* `TY_INIT_PARAMS_S *p_param`：初始化配置，`Linux` 系统可以设置为 `NULL`。
  
  ```c
  typedef struct {
    BOOL_T init_db;// 是否初始化KV，此处为了提高启动速度，一些要求快速启动的设备可以设置成FALSE
    CHAR_T sys_env[SYS_ENV_LEN]; // 系统环境，可以不提供，默认为全0
    CHAR_T log_seq_path[LOG_SEQ_PATH_LEN]; // 日志序路径，可以不提供，默认为全0
  }TY_INIT_PARAMS_S;
  ```

#### `tuya_iot_set_wf_gw_prod_info`

```C
/***********************************************************
*  Function: tuya_iot_set_wf_gw_prod_info
*  Input: wf_prod_info
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_set_wf_gw_prod_info(IN CONST WF_GW_PROD_INFO_S *wf_prod_info)
```

此接口是 `TuyaOS` 授权信息写入接口，写入唯一的 `UUID` 和 `AuthKey` 信息，用于和涂鸦云平台交互、认证使用。此接口仅用于演示程序使用，实际量产的代码会通过更加高效的功能模块和专门的工具来进行，不需要客户调用接口来处理。

* `WF_GW_PROD_INFO_S *wf_prod_info`：产测信息，包含了 `UUID` 和 `AuthKey`。

```c
typedef struct {
    CHAR_T *uuid;       // UUID，必须提供，可以从涂鸦云平台申请
    CHAR_T *auth_key;   // AuthKey，必须提供，可以从涂鸦云平台申请
    CHAR_T *ap_ssid;    // 默认热点名称，可以不提供，默认为Smartlife_xxxx
    CHAR_T *ap_passwd;  // 默认热点密码，可以不提供，默认为空密码
}WF_GW_PROD_INFO_S;
```

#### `tuya_iot_wf_soc_dev_init_param`

```c
/**
 * @brief tuya_iot_wf_soc_dev_init_param 
 *
 * @param[in] cfg
 * @param[in] start_mode
 * @param[in] cbs: tuya wifi sdk user callbacks,note cbs->dev_ug_cb is useless
 * @param[in] firmware_key
 * @param[in] product_key: product key/proudct id,get from tuya open platform
 * @param[in] wf_sw_ver: wifi module software version format:xx.xx.xx (0<=x<=9)
 *
 * @return OPERATE_RET
 */
 OPERATE_RET tuya_iot_wf_soc_dev_init_param(IN CONST GW_WF_CFG_MTHD_SEL cfg, IN CONST GW_WF_START_MODE start_mode,
                                     IN CONST TY_IOT_CBS_S *cbs,IN CONST CHAR_T *firmware_key,
                                     IN CONST CHAR_T *product_key,IN CONST CHAR_T *wf_sw_ver);
```

此接口是 `TuyaOS` 应用产品初始化接口，通过开发者传递过来的 `PID`、固件key、版本信息，到涂鸦云激活，并向云端获取对应 `PID` 的模型，即 `schema` 信息对设备进行初始化。此函数接口参数较多，下面逐一解释每个参数的含义。

* `GW_WF_CFG_MTHD_SEL cfg`：`Wi-Fi` 设备工作模式。

```c
typedef BYTE_T GW_WF_CFG_MTHD_SEL;  // wifi config method select
#define GWCM_OLD                0   // 不支持低功耗模式
#define GWCM_LOW_POWER          1   // with low power mode
#define GWCM_SPCL_MODE          2   // special with low power mode
#define GWCM_OLD_PROD           3   // GWCM_OLD mode with product
#define GWCM_LOW_POWER_AUTOCFG  4   // with low power mode && auto cfg
#define GWCM_SPCL_AUTOCFG       5   // special with low power mode && auto cfg
```

* `GW_WF_START_MODE start_mode`：`Wi-Fi` 设备配网模式。

```c
typedef BYTE_T GW_WF_START_MODE;
#define WF_START_AP_ONLY        0   // 仅支持AP配网
#define WF_START_SMART_ONLY     1   // 仅支持EZ配网
#define WF_START_AP_FIRST       2   // 支持AP和EZ配网，初始默认AP，通过重置在AP和EZ配网直接切换
#define WF_START_SMART_FIRST    3   // 支持AP和EZ配网，初始默认EZ，通过重置在AP和EZ配网直接切换
#define WF_START_SMART_AP_CONCURRENT    4   //  支持AP和EZ配网，AP、EZ同时存在
```

* `TY_IOT_CBS_S *cbs`

其中回调函数集合的用途如下表所示，如果应用不需要处理，设置 `NULL` 即可。

```c
typedef struct {
    GW_STATUS_CHANGED_CB gw_status_cb;     //通知应用设备状态发生了变化
    GW_UG_INFORM_CB gw_ug_cb;            //通知应用设备开始升级
    GW_RESET_IFM_CB gw_reset_cb;        //通知应用设备正准备重置
    DEV_OBJ_DP_CMD_CB dev_obj_dp_cb;    //通知应用收到了对象型控制命令
    DEV_RAW_DP_CMD_CB dev_raw_dp_cb;    //通知应用收到了`RAW`型控制命令
    DEV_DP_QUERY_CB dev_dp_query_cb;    //通知应用收到了查询命令
    DEV_UG_INFORM_CB dev_ug_cb;            //通知应用开始子设备升级（网关需要）
    DEV_RESET_IFM_CB dev_reset_cb;        //通知应用开始子设备重置（网关需要）
    ACTIVE_SHORTURL_CB active_shorturl;    //通知应用获得了二维码，方便展示二维码（带屏幕设备）。
    GW_UG_INFORM_CB pre_gw_ug_cb;        //通知应用设备即将开始升级
    DEV_UG_INFORM_CB pre_dev_ug_cb;        //通知应用子设备即将开始升级（网关需要）
}TY_IOT_CBS_S;
```

#### `tuya_iot_reg_get_wf_nw_stat_cb`

```c
/**
 * @brief tuya_iot_reg_get_wf_nw_stat_cb_params 
 *
 * @param wf_nw_stat_cb network status change callback
 * @param min_interval_s network status monitor time interval
 *
 * @return 
 */
OPERATE_RET tuya_iot_reg_get_wf_nw_stat_cb_params(IN CONST GET_WF_NW_STAT_CB wf_nw_stat_cb, IN CONST INT_T min_interval_s);
```

此接口注册了一个网络状态监控的回调函数，每1秒钟查询一次设备的网络状态，如果网络状态发生了变化，`TuyaOS` 会调用注册的回调接口，将变化通知给应用程序。

`GET_WF_NW_STAT_CB wf_nw_stat_cb`：网络状态变化通知回调，按照`min_interval_s`周期，检测网络状态，如果发生变化则会调用改回调接口。

### dp_process文件介绍

对DP相关功能的处理都在该文件内实现。

#### `update_all_dp`

DP数据通过 `OPERATE_RET dev_report_dp_json_async(IN CONST CHAR_T *dev_id,IN CONST TY_OBJ_DP_S *dp_data,IN CONST UINT_T cnt);` 函数上报到云端。

DP 上报函数示例:

```c
VOID_T update_all_dp(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    INT_T dp_cnt = 1; /* update DP number */

    /* no connect router, return */
    GW_WIFI_NW_STAT_E wifi_state = STAT_LOW_POWER;
    get_wf_gw_nw_status(&wifi_state);
    if (wifi_state <= STAT_AP_STA_DISC || wifi_state == STAT_STA_DISC) {
        return;
    }

    TY_OBJ_DP_S *dp_arr = (TY_OBJ_DP_S *)tal_malloc(dp_cnt*SIZEOF(TY_OBJ_DP_S));
    if(NULL == dp_arr) {
        TAL_PR_ERR("malloc failed");
        return;
    }

    dp_arr[0].dpid = DPID_SWITCH; /* DP ID */
    dp_arr[0].type = PROP_BOOL; /* DP type */
    dp_arr[0].time_stamp = 0;
    dp_arr[0].value.dp_bool = get_led_status(); /* DP data */

    /* report DP */
    op_ret = dev_report_dp_json_async(NULL ,dp_arr, dp_cnt);

    /* free requested memory space */
    tal_free(dp_arr);
    dp_arr = NULL;
    if(OPRT_OK != op_ret) {
        TAL_PR_ERR("dev_report_dp_json_async relay_config data error,err_num",op_ret);
    }

    return;
}
```

#### `deal_dp_proc`

云端下发DP数据后，会通过 `STATIC VOID_T dev_obj_dp_cb(IN CONST TY_RECV_OBJ_DP_S *dp)` 函数进行回调处理，该函数将数据传入到 `VOID_T deal_dp_proc(IN CONST TY_OBJ_DP_S *root)` 函数里面执行下发的 DP 命令。
DP 处理代码示例：

```c
VOID_T deal_dp_proc(IN CONST TY_OBJ_DP_S *root)
{
    UCHAR_T dpid;
    dpid = root->dpid;
    TAL_PR_DEBUG("dpid:%d", dpid);

    switch(dpid) {
        case DPID_SWITCH:
            if (root->value.dp_bool == TRUE) {
                set_led_status(LED_ON);
                TAL_PR_NOTICE("led on");
            } else {
                set_led_status(LED_OFF);
                TAL_PR_NOTICE("led off");
            }

            /* update device current status to cloud */
            update_all_dp();
            break;

        default :
            break;
    }
    return;
}
```

### app_led 文件介绍

对灯的控制的功能都在该文件内进行实现。

#### `app_led_init`

初始化 LED 灯的使用引脚。

```c
VOID_T app_led_init(TUYA_GPIO_NUM_E pin_id)
{
    OPERATE_RET rt = OPRT_OK;

    TUYA_GPIO_BASE_CFG_T led_cfg = {
        .mode = TUYA_GPIO_PUSH_PULL,
        .direct = TUYA_GPIO_OUTPUT,
        .level = TUYA_GPIO_LEVEL_HIGH
    };
    TUYA_CALL_ERR_LOG(tkl_gpio_init(pin_id, &led_cfg));
    sg_led_pin = pin_id;
    set_led_status(LED_OFF);

    THREAD_CFG_T blink_thread_cfg = {
        .thrdname = "led_task",
        .priority = THREAD_PRIO_6,
        .stackDepth = 1024
    };
    TUYA_CALL_ERR_LOG(tal_thread_create_and_start(&blink_task_handle, NULL, NULL, wifi_status_display_task, NULL, &blink_thread_cfg));
    return;
}
```

#### `set_led_status`

通过调用该接口对 LED 进行开关控制，需要注意的是要根据 LED 是低电平点亮还是高电平点亮

```c
VOID_T set_led_status(LED_STATUS_E led_status)
{
    if (LED_ON == led_status) {
        tkl_gpio_write(sg_led_pin, TUYA_GPIO_LEVEL_HIGH);
    }
    else {
        tkl_gpio_write(sg_led_pin, TUYA_GPIO_LEVEL_LOW);
    }

    cur_led_status = led_status;
    return;
}
```

#### `get_led_status`

通过该函数可以获取灯当前的开关状态。

```c
LED_STATUS_E get_led_status(void)
{
    return cur_led_status;
}
```

#### `wifi_status_display_task`

当设备处于配网模式时，会通过闪烁 LED 灯来指示：

```c
STATIC VOID_T wifi_status_display_task(VOID_T *args)
{
    UINT32_T blink_interval = (UINT32_T)(args);
    GW_WIFI_NW_STAT_E network_status;

    while (1)
    {
        get_wf_gw_nw_status(&network_status);
        if ((network_status >= STAT_UNPROVISION) && (network_status <= STAT_STA_CONN)) {
            tkl_gpio_write(LED_PIN, TUYA_GPIO_LEVEL_LOW);
            tal_system_sleep(blink_interval);
            tkl_gpio_write(LED_PIN, TUYA_GPIO_LEVEL_HIGH);
            tal_system_sleep(blink_interval);
        }
        else {
            tal_system_sleep(3000);
        }
    }
}
```

```c
VOID_T app_led_thread_init(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;
    UINT32_T blink_interval = 500;

    THREAD_CFG_T blink_thread_cfg = {
        .thrdname = "led_task",
        .priority = THREAD_PRIO_6,
        .stackDepth = 1024};

    tal_thread_create_and_start(&blink_task_handle,
                                NULL,
                                NULL,
                                wifi_status_display_task,
                                blink_interval,
                                &blink_thread_cfg);
}
```

### app_key 文件介绍

该文件主要实现配网按键功能。

#### `app_key_init`

初始化按键引脚，创建一个线程去检测按键是否按下。

```c
VOID_T app_key_init(TUYA_GPIO_NUM_E pin_id)
{
    OPERATE_RET rt = OPRT_OK;

    /* init key gpio */
    TUYA_GPIO_BASE_CFG_T key_cfg = {
        .mode = TUYA_GPIO_PULLUP,
        .direct = TUYA_GPIO_INPUT,
        .level = TUYA_GPIO_LEVEL_HIGH
    };
    TUYA_CALL_ERR_LOG(tkl_gpio_init(pin_id, &key_cfg));
    sg_key_pin_id = pin_id;

    /* start key thread */
    THREAD_HANDLE key_task_handle;
    THREAD_CFG_T thread_cfg = {
        .thrdname = "key_task",
        .priority = THREAD_PRIO_6,
        .stackDepth = 2048
    };
    TUYA_CALL_ERR_LOG(tal_thread_create_and_start(&key_task_handle, NULL, NULL, app_key_task, NULL, &thread_cfg));

    return;
}
```

#### `app_key_task`

按键任务函数每 100ms 会去轮询检测按键是否按下，并通过调用 `tal_system_get_millisecond()` 得到系统节拍时间判断按键是短按还是长按。短按按键切换灯光状态，并进行上报。长按按键调用 `tuya_iot_wf_gw_unactive()` 函数移除设备。

```c
STATIC VOID_T app_key_task(VOID_T *args)
{
    OPERATE_RET op_ret = OPRT_OK;
    TUYA_GPIO_LEVEL_E read_level = TUYA_GPIO_LEVEL_HIGH;
    UINT32_T time_start = 0, timer_end = 0;
    UINT8_T release_flag = 1;

    for (;;) {
        tkl_gpio_read(sg_key_pin_id, &read_level);
        if (TUYA_GPIO_LEVEL_LOW == read_level && release_flag) {
            release_flag = 0;
            tal_system_sleep(3);
            tkl_gpio_read(sg_key_pin_id, &read_level);

            time_start = tal_system_get_millisecond();
            while (TUYA_GPIO_LEVEL_LOW == read_level) {
                tkl_gpio_read(sg_key_pin_id, &read_level);
                tal_system_sleep(30);

                timer_end = tal_system_get_millisecond();
                if (timer_end - time_start >= LONE_PRESS_TIME) {
                    TAL_PR_DEBUG("long press, remove device");
                    /* long press, remove device */
                    op_ret = tuya_iot_wf_gw_unactive();
                    if (op_ret != OPRT_OK) {
                        TAL_PR_ERR("long press tuya_iot_wf_gw_unactive error, %d", op_ret);
                    }
                    break;
                }
            }

            /* wait key release */
            while(TUYA_GPIO_LEVEL_LOW != read_level) {
                tal_system_sleep(10);
                tkl_gpio_read(sg_key_pin_id, &read_level);
            }
            release_flag = 1;

            if (timer_end - time_start > 50) {
                /* normal press */
                if (get_led_status()) {
                    set_led_status(LED_OFF);
                }
                else {
                    set_led_status(LED_ON);
                }
                update_all_dp();
            }
        }
        tal_system_sleep(100);
    }

    return;
}
```

## 编译代码

#### 编译

右键点击 app 目录下的 demo 文件，点击 `Build Project`

![build](https://images.tuyacn.com/fe-static/docs/img/7bb58048-112a-4310-bfc2-a2d0f0ee7a9d.png)
随后需要输入生成的版本，这里以 `1.0.0` 为例。
![build_version](https://images.tuyacn.com/fe-static/docs/img/287ffb9a-e6bb-4a98-b872-e60e60052d28.png)
输入版本号之后就等待编译结果，显示 `COMPILE SUCCESS` 就表明编译成功。

## 生成产物

在编译完成后，会将生成的固件按照不同版本放在所编译的工程目录下 output 文件夹内。

例：如果我们编译的是 apps 文件夹中的 `tuyaos_demo_quickstart` 工程，编译版本为1.0.0，那么生成的固件将会放在 `apps/tuyaos_demo_quickstart/output/1.0.0`中。

![output](https://images.tuyacn.com/fe-static/docs/img/835edbfe-63b1-4d81-860f-bb97d91a8b2b.png)

编译后的产物如上图所示，这里主要关注的 `tuyaos_demo_quickstart_QIO_1.0.0.bin` 、`tuyaos_demo_quickstart_UA_1.0.0.bin` 和 `tuyaos_demo_quickstart_UG_1.0.0.bin` 这三个文件。

| 文件名                                    | 功能                     |
| -------------------------------------- | ---------------------- |
| `tuyaos_demo_quickstart_QIO_1.0.0.bin` | 生产区固件：boot+用户区固件+检测固件。 |
| `tuyaos_demo_quickstart_UA_1.0.0.bin`  | 用户区固件                  |
| `tuyaos_demo_quickstart_UG_1.0.0.bin`  | 升级固件                   |

## 烧写流程

请开发者查看PC/tools目录，原厂烧录工具及说明文档都在此目录下。

## FAQ

在使用过程中，如果遇到问题，可以访问涂鸦开发者平台[Wi-Fi设备接入-故障排除](https://developer.tuya.com/cn/docs/iot-device-dev/faq?id=Kaiuyfi4402ee)，按照自己遇到的问题分类，寻找合适的解决方法。
