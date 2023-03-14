/**
 * @file app_main.c
 * @author www.tuya.com
 * @brief app_main module is used to
 * @version 0.1
 * @date 2022-04-14
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */
#include "tkl_thread.h"

/***********************************************************
*************************micro define***********************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC TKL_THREAD_HANDLE ty_app_thread = NULL;

/***********************************************************
***********************function define**********************
***********************************************************/

/**
 * @brief
 *
 * @param[in]
 * @return
 */
void app_main(void)
{
    tuya_leds_pixel_dev_demo();
    return;
}

static void tuya_app_thread(void *arg)
{
    app_main();

    tkl_thread_release(ty_app_thread);
    ty_app_thread = NULL;
}

void tuya_app_main(void)
{
    tkl_thread_create(&ty_app_thread, "tuya_app_main", 4096, 4, tuya_app_thread, NULL);
}