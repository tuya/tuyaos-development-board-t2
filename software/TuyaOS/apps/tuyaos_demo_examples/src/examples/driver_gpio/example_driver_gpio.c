/**
* @file examples_driver_gpio.c
* @author www.tuya.com
* @brief 一个简单的tkl gpio接口使用演示程序，可以通过命令行执行
* @version 0.1
* @date 2022-05-20
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include "tuya_iot_config.h"

#include "tuya_cloud_types.h"
#include "tal_log.h"
#include "tal_thread.h"
#include "tal_system.h"
#include "tkl_gpio.h"
#include "tkl_output.h"

/***********************************************************
*************************micro define***********************
***********************************************************/
#define GPIO_OUT_PIN_CBU        GPIO_NUM_15
#define GPIO_IN_PIN_CBU         GPIO_NUM_17
#define GPIO_IRQ_PIN_CBU        GPIO_NUM_9

#define GPIO_OUT_PIN_WBRU       GPIO_NUM_2
#define GPIO_IN_PIN_WBRU        GPIO_NUM_3
#define GPIO_IRQ_PIN_WBRU       GPIO_NUM_4

#define TASK_GPIO_PRIORITY      THREAD_PRIO_2
#define TASK_GPIO_SIZE          1024

/***********************************************************
***********************typedef define***********************
***********************************************************/


/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC THREAD_HANDLE sg_gpio_handle;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
* @brief interrupt callback function
*
* @param[in] args:parameters
* @return none
*/
STATIC VOID_T __gpio_irq_callback(VOID_T *args)
{
    /* TAL_PR_ , PR_ ，这两种打印里面有锁，不要在中断里使用 */
    tkl_log_output("\r\n------------ GPIO IRQ Callbcak ------------\r\n");
}

/**
 * @brief 根据不同芯片平台选择不同引脚
 *
 * @param[in] : 
 *
 * @return none
 */
STATIC VOID __example_pin_get(TUYA_GPIO_NUM_E *out_pin, TUYA_GPIO_NUM_E *in_pin, TUYA_GPIO_NUM_E *irq_pin)
{
    if (0 == strcmp("rtl8720cf_ameba", TARGET_PLATFORM)) {
        TAL_PR_NOTICE("TARGET_PLATFORM is rtl8720cf_ameba");
        *out_pin = GPIO_OUT_PIN_WBRU;
        *in_pin = GPIO_IN_PIN_WBRU;
        *irq_pin = GPIO_IRQ_PIN_WBRU;
    } else if (0 == strcmp("bk7231n", TARGET_PLATFORM)) {
        TAL_PR_NOTICE("TARGET_PLATFORM is bk7231n");
        *out_pin = GPIO_OUT_PIN_CBU;
        *in_pin = GPIO_IN_PIN_CBU;
        *irq_pin = GPIO_IRQ_PIN_CBU;
    }

    return;
}

/**
* @brief gpio task
*
* @param[in] param:Task parameters
* @return none
*/
STATIC VOID __example_gpio_task(VOID* param)
{
    OPERATE_RET rt = OPRT_OK;
    UCHAR_T i = 0;
    TUYA_GPIO_LEVEL_E read_level = 0;

    TUYA_GPIO_NUM_E out_pin, in_pin, irq_pin;

    __example_pin_get(&out_pin, &in_pin, &irq_pin);

    /*GPIO output init*/
    TUYA_GPIO_BASE_CFG_T out_pin_cfg = {
        .mode = TUYA_GPIO_PUSH_PULL,
        .direct = TUYA_GPIO_OUTPUT,
        .level = TUYA_GPIO_LEVEL_LOW
    };
    TUYA_CALL_ERR_LOG(tkl_gpio_init(out_pin, &out_pin_cfg));

    /*GPIO input init*/
    TUYA_GPIO_BASE_CFG_T in_pin_cfg = {
        .mode = TUYA_GPIO_PUSH_PULL,
        .direct = TUYA_GPIO_INPUT,
    };
    TUYA_CALL_ERR_LOG(tkl_gpio_init(in_pin, &in_pin_cfg));

    /*GPIO irq init*/
    TUYA_GPIO_IRQ_T irq_cfg = {
        .cb = __gpio_irq_callback,
        .arg = NULL,
        .mode = TUYA_GPIO_IRQ_RISE,
    };
    TUYA_CALL_ERR_LOG(tkl_gpio_irq_init(irq_pin, &irq_cfg));

    /*irq enable*/
    TUYA_CALL_ERR_LOG(tkl_gpio_irq_enable(irq_pin));

    while (1) {
        /* GPIO output */
        if (i == 0) {
            tkl_gpio_write(out_pin, TUYA_GPIO_LEVEL_HIGH);
            TAL_PR_DEBUG("pin output high");
        } else {
            tkl_gpio_write(out_pin, TUYA_GPIO_LEVEL_LOW);
            TAL_PR_DEBUG("pin output low");
        }
        i = i^1;

        /* GPIO read */
        TUYA_CALL_ERR_LOG(tkl_gpio_read(in_pin, &read_level));
        if(read_level == 1) {
            TAL_PR_DEBUG("GPIO read high level");
        } else {
            TAL_PR_DEBUG("GPIO read low level");
        }

        tal_system_sleep(2000);
    }
}

/**
* @brief examples_adc_init_and_read
*
* @return none
*/
VOID example_gpio(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;

    STATIC THREAD_CFG_T thrd_param = {
        .priority = TASK_GPIO_PRIORITY,
        .stackDepth = TASK_GPIO_SIZE,
        .thrdname = "gpio"
    };
    TUYA_CALL_ERR_LOG(tal_thread_create_and_start(&sg_gpio_handle, NULL, NULL, __example_gpio_task, NULL, &thrd_param));

    return;
}
