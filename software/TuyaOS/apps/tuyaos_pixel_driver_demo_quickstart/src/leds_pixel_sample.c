/**
 * @file leds_pixel_sample.c
 * @author www.tuya.com
 * @brief leds_pixel_sample module is used to provid leds pixel device sample
 * @version 0.1
 * @date 2022-03-30
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */
#include "mem_pool.h"
#include "uni_log.h"
#include "tuya_hal_system.h"
#include "tdl_pixel_dev_manage.h"
#include "tdl_pixel_color_manage.h"
#include "tdd_pixel_ws2812.h"
#include "tuya_hal_system.h"

/***********************************************************
*************************micro define***********************
***********************************************************/
#define LEDS_PIXEL_NUM 200u
#define LEDS_PIXEL_DEV "my_device"
#define COLOR_ARR_NUM  3

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
PIXEL_HANDLE_T dev_handle = NULL;
PIXEL_COLOR_T g_rgb_arr[COLOR_ARR_NUM] = {
    {
        .red = 255,
        .green = 0,
        .blue = 0,
        .cold = 0,
        .warm = 0,
    },
    {
        .red = 0,
        .green = 255,
        .blue = 0,
        .cold = 0,
        .warm = 0,
    },
    {
        .red = 0,
        .green = 0,
        .blue = 255,
        .cold = 0,
        .warm = 0,
    },
};

PIXEL_COLOR_T g_gbr_arr[COLOR_ARR_NUM] = {
    {
        .red = 0,
        .green = 255,
        .blue = 0,
        .cold = 0,
        .warm = 0,
    },
    {
        .red = 0,
        .green = 0,
        .blue = 255,
        .cold = 0,
        .warm = 0,
    },
    {
        .red = 255,
        .green = 0,
        .blue = 0,
        .cold = 0,
        .warm = 0,
    },
};

/***********************************************************
***********************function define**********************
***********************************************************/
void tuya_leds_pixel_dev_demo(void)
{
    OPERATE_RET op_ret = OPRT_OK;
    PIXEL_DEV_CONFIG_T config = {0};
    UINT_T i = 0;
    STATIC UCHAR_T refresh_cnt = 0;
    UCHAR_T refresh_pixel_num = 0;

    // register my device
    op_ret = tdd_ws2812_driver_register(LEDS_PIXEL_DEV, 2);
    if (OPRT_OK != op_ret) {
        PR_ERR("tdd_ws2812_driver_register err:%d", op_ret);
        return;
    }

    PR_NOTICE("register my device succ!");

    // find device
    op_ret = tdl_pixel_dev_find(LEDS_PIXEL_DEV, &dev_handle);
    if (OPRT_OK != op_ret) {
        PR_ERR("cant find dev err:%d", op_ret);
        return;
    }

    PR_NOTICE("find my device succ!");

    // open device
    config.pixel_num = LEDS_PIXEL_NUM;
    op_ret = tdl_pixel_dev_open(dev_handle, &config);
    if (OPRT_OK != op_ret) {
        PR_ERR("open find dev err:%d", op_ret);
        return;
    }

    PR_NOTICE("open my device succ!");

    while (1) {
        refresh_cnt++;
        if ((refresh_cnt * 6) >= LEDS_PIXEL_NUM) {
            refresh_cnt = 0;
        }
        refresh_pixel_num = LEDS_PIXEL_NUM - refresh_cnt * 6;
        op_ret = tdl_pixel_set_single_color(dev_handle, 0, refresh_pixel_num, &g_rgb_arr[i]);
        if (OPRT_OK != op_ret) {
            PR_ERR("tdl_pixel_set_single_color err:%d", op_ret);
            return;
        }

        op_ret = tdl_pixel_dev_refresh(dev_handle);
        if (OPRT_OK != op_ret) {
            PR_ERR("tdl_pixel_dev_refresh err:%d", op_ret);
            return;
        }

        PR_NOTICE("refresh:%d", refresh_pixel_num);

        tuya_hal_system_sleep(1000);
        i = (i + 1) % COLOR_ARR_NUM;
    }

    return;
}