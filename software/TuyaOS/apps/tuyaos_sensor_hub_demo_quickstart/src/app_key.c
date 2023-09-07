/**
 * @file app_key.c
 * @author www.tuya.com
 * @brief app_key module is used to 
 * @version 0.1
 * @date 2022-10-28
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include "tkl_gpio.h"
#include "tal_log.h"
#include "tal_system.h"
#include "tal_thread.h"
#include "tuya_iot_wifi_api.h"

#include "app_key.h"
#include "app_led.h"

#include "app_temp_humi.h"

/***********************************************************
************************macro define************************
***********************************************************/
#define LONE_PRESS_TIME 3000 // long press time, uint: ms

/***********************************************************
***********************typedef define***********************
***********************************************************/


/***********************************************************
********************function declaration********************
***********************************************************/


/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC TUYA_GPIO_NUM_E sg_key_pin_id=0;

/***********************************************************
***********************function define**********************
***********************************************************/

/**
 * @brief key thread task, judge whether the key is pressed, long or short press
 *
 * @param[in] args: Parameters when the task is created
 * 
 * @return none
 */
STATIC VOID_T app_key_task(VOID_T *args)
{
    OPERATE_RET op_ret = OPRT_OK;
    TUYA_GPIO_LEVEL_E read_level = TUYA_GPIO_LEVEL_HIGH;
    UINT32_T time_start = 0, timer_end = 0;

    for (;;) {
        tkl_gpio_read(sg_key_pin_id, &read_level);
        if (TUYA_GPIO_LEVEL_LOW == read_level) {
            tal_system_sleep(3);
            tkl_gpio_read(sg_key_pin_id, &read_level);
            if (TUYA_GPIO_LEVEL_LOW != read_level) {
                break; // jitter
            }

            time_start = tal_system_get_millisecond();
            while (TUYA_GPIO_LEVEL_LOW == read_level) {
                tal_system_sleep(30);
                tkl_gpio_read(sg_key_pin_id, &read_level);
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

            if (timer_end - time_start > 50) {
                TAL_PR_DEBUG("normal press");
                /* normal press */
                if (get_led_status()) {
                    set_led_status(LED_OFF);
                }
                else {
                    set_led_status(LED_ON);
                }
                
                app_temp_humi_repo_all_dp();
            } else {
                TAL_PR_DEBUG("time too short");
            }
        }

        tal_system_sleep(100);
    }

    return;
}

/**
 * @brief key gpio init, creat key thread
 *
 * @param[in] pin_id: key pin id
 * 
 * @return none
 */
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
