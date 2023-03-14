# 修改记录

| 版本 | 时间       | 修改内容 |
| ---- | ---------- | -------- |
| v2.0 | 2022.04.15 | 初版     |

# 名词解释



# 基本功能

1、提供查找设备（灯带）、启动设备和停止设备的功能。通过设备名字查找已注册设备，开启设备或停止设备

2、提供像素数据刷新显示功能，在灯带上显示相应的颜色

3、提供单一颜色显示、多种颜色显示、在背景色上设置前景色功能等

4、提供像素点颜色的左右平移、镜像平移功能等

# 组件依赖

| sdk组件依赖          | 说明         |
| -------------------- | ------------ |
| tuya_hal_mutex.h     | 锁           |
| tuya_hal_semaphore.h | 信号量       |
| tuya_hal_thread.h    | 线程         |
| mem_pool.h           | 内存池       |
| uni_log.h            | 日志管理     |
| tuya_hal_system.h    | 系统相关接口 |

# 资源依赖

以ws2812设备为例，驱动灯珠数为100颗

| 资源                           | 大小   | 说明                                                |
| ------------------------------ | ------ | --------------------------------------------------- |
| 完成设备注册并打开设备内存消耗 | 约7.6k | 每增加（减少）10颗灯珠，RAM消耗增加（减少）300bytes |

# 接口列表说明

- 像素点驱动管理接口，详细使用说明参考`tdl_pixel_dev_manage.h`

| 接口                                                         | 说明                               |
| ------------------------------------------------------------ | ---------------------------------- |
| int tdl_pixel_dev_find(char *name, OUT PIXEL_HANDLE_T *handle); | 查找幻彩灯带设备                   |
| int tdl_pixel_dev_open(PIXEL_HANDLE_T handle, PIXEL_DEV_CONFIG_T* config); | 启动设备                           |
| int tdl_pixel_dev_refresh(PIXEL_HANDLE_T handle);            | 刷新所有像素显存的数据到驱动端显示 |
| int tdl_pixel_dev_config(PIXEL_HANDLE_T handle, PIXEL_DEV_CFG_CMD_E cmd, void *arg); | 配置设备参数                       |
| int tdl_pixel_dev_close(PIXEL_HANDLE_T *handle);             | 停止设备                           |

- 像素点驱动颜色操作接口，详细使用说明参考`tdl_pixel_color_manage.h`

| 接口                                                         | 说明                     |
| ------------------------------------------------------------ | ------------------------ |
| int tdl_pixel_set_single_color(PIXEL_HANDLE_T handle, UINT_T index_start, UINT_T pixel_num, PIXEL_COLOR_T *color); | 设置像素段颜色（单一）   |
| int tdl_pixel_set_multi_color(PIXEL_HANDLE_T handle, UINT_T index_start, UINT_T pixel_num, PIXEL_COLOR_T *color_arr); | 设置像素段颜色（多种）   |
| int tdl_pixel_set_single_color_with_backcolor(PIXEL_HANDLE_T handle, UINT_T index_start, UINT_T pixel_num, PIXEL_COLOR_T *backcolor, PIXEL_COLOR_T *color); | 在背景色上设置像素段颜色 |
| int tdl_pixel_cycle_shift_color(PIXEL_HANDLE_T handle, PIXEL_SHIFT_DIR_T dir, UINT_T index_start, UINT_T index_end, UINT_T move_step); | 循环平移像素颜色         |
| int tdl_pixel_mirror_cycle_shift_color(PIXEL_HANDLE_T handle, PIXEL_M_SHIFT_DIR_T dir, UINT_T index_start, UINT_T index_end, UINT_T move_step); | 镜像循环移动像素颜色     |
| int tdl_pixel_get_color(PIXEL_HANDLE_T handle, UINT_T index,  PIXEL_COLOR_T *color); | 获得像素颜色             |
| int tdl_pixel_copy_color(PIXEL_HANDLE_T handle, UINT_T dst_idx, UINT_T src_idx, UINT_T len); | 复制像素颜色 |
|int tdl_pixel_set_single_color_all(PIXEL_HANDLE_T handle,  PIXEL_COLOR_T *color); | 设置所有像素为同一颜色 |



# 使用说明

```c
#define SOUND_DEVICE_NAME		"my_sound"
#define PIXEL_DEVICE_NAME       "my_leds_pixel" 

PIXEL_HANDLE_T       ws2812_handle;                     // 设备句柄

PIXEL_COLOR_T   g_gbr_arr[COLOR_ARR_NUM] = {
    {
        .red   = 0,
        .green = 255,
        .blue  = 0,
        .cold  = 0,
        .warm  = 0,
    },
    {
        .red   = 0,
        .green = 0,
        .blue  = 255,
        .cold  = 0,
        .warm  = 0,
    },
    {
        .red   = 255,
        .green = 0,
        .blue  = 0,
        .cold  = 0,
        .warm  = 0,
    },
}; 

void tuya_leds_pixel_ctrl_demo(void)
{
    OPERATE_RET op_ret = OPRT_OK;
    PIXEL_DEV_CONFIG_T config = {0};

    //register my device
    op_ret = tdd_ws2812_driver_register(LEDS_PIXEL_DEV, 2);
    if(OPRT_OK != op_ret) {
        PR_ERR("tdd_ws2812_driver_register err:%d", op_ret);
        return;
    }

    PR_NOTICE("register my device succ!");

    //find device
    op_ret = tdl_pixel_dev_find(LEDS_PIXEL_DEV, &ws2812_handle);
    if(OPRT_OK != op_ret) {
        PR_ERR("cant find dev err:%d", op_ret);
        return;
    }
  
    PR_NOTICE("find my device succ!");

    //open device
    config.pixel_num = 100;
    op_ret = tdl_pixel_dev_open(ws2812_handle, &config);
    if(OPRT_OK != op_ret) {
        PR_ERR("open find dev err:%d", op_ret);
        return;
    }

    /* 设置24颗灯珠的颜色均为红色 */
    op_ret = tdl_pixel_set_single_color(ws2812_handle, 0, 100, &g_gbr_arr[0]);
    tdl_pixel_dev_refresh(ws2812_handle);     // 刷新所有像素显存的数据到驱动端显示   
    
    /* 设置多种像素颜色 */
    // for (int i = 0; i < 34; i++) {
    //     op_ret = tdl_pixel_set_multi_color(ws2812_handle, i*3, 3, &g_gbr_arr);
    // }
    // tdl_pixel_dev_refresh(ws2812_handle);
    
    /* 在背景色上设置前景色 */
    // op_ret = tdl_pixel_set_single_color_with_backcolor(ws2812_handle, 8, 8, &g_gbr_arr[0], &g_gbr_arr[1]);
    // tdl_pixel_dev_refresh(ws2812_handle);
    
    /* 循环平移像素颜色 */
    // op_ret = tdl_pixel_cycle_shift_color(ws2812_handle, PIXEL_SHIFT_RIGHT, 0, 23, 1);
    // tdl_pixel_dev_refresh(ws2812_handle);
    
    /* 镜像循环移动像素颜色 */
    // op_ret = tdl_pixel_mirror_cycle_shift_color(ws2812_handle, PIXEL_SHIFT_CLOSE, 0, 23, 1);
    // tdl_pixel_dev_refresh(ws2812_handle); 

    return op_ret;
}
```

# 常见问题

A：

Q：

# 相关链接

- [Tuya IoT 文档中心](https://developer.tuya.com/cn/docs/iot)

- [Wi-Fi 模组 SDK 二次开发教程](https://developer.tuya.com/cn/docs/iot/Module-SDK-development_tutorial?id=Kauqptzv5yo8a)