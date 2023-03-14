/**
* @file examples_driver_pwm.c
* @author www.tuya.com
* @brief 一个简单的tkl pwm接口使用演示程序，可以通过命令行执行
* @version 0.1
* @date 2022-05-20
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include "tuya_cloud_types.h"

#include "tal_log.h"
#include "tal_thread.h"
#include "tal_system.h"
#include "tkl_pwm.h"

/***********************************************************
*************************micro define***********************
***********************************************************/
#define PWM_DUTY                    5000 // 50% duty
#define PWM_FREQUENCY               10000
#define TASK_PWM_PRIORITY           THREAD_PRIO_2
#define TASK_PWM_SIZE               1024

/***********************************************************
***********************typedef define***********************
***********************************************************/
#define PWM_ID                      PWM_NUM_0

/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC THREAD_HANDLE                sg_pwm_handle;

/***********************************************************
***********************function define**********************
***********************************************************/

/**
* @brief pwm task
*
* @param[in] param:Task parameters
* @return none
*/
STATIC VOID __example_pwm_task(VOID* param)
{
    OPERATE_RET rt = OPRT_OK;
    UINT32_T frequency = PWM_FREQUENCY;
    UINT32_T count = 0;

    /*pwm init*/
    TUYA_PWM_BASE_CFG_T pwm_cfg = {
        .duty = PWM_DUTY, /* 1-10000 */
        .frequency = PWM_FREQUENCY,
        .polarity  = TUYA_PWM_NEGATIVE,
    };
    TUYA_CALL_ERR_GOTO(tkl_pwm_init(PWM_ID, &pwm_cfg), __EXIT);

    /*start PWM3*/
    TUYA_CALL_ERR_GOTO(tkl_pwm_start(PWM_ID), __EXIT);
    TAL_PR_NOTICE("PWM%d start", PWM_ID);

    while (1) {
        /*Frequency, duty cycle settings*/
        TUYA_CALL_ERR_LOG(tkl_pwm_frequency_set(PWM_ID, frequency));
        TUYA_CALL_ERR_LOG(tkl_pwm_start(PWM_ID));
        TAL_PR_NOTICE("PWM%d , frequency: %d", PWM_ID,  frequency);

        /*close pwm*/
        if(count >= 3) {
            break;
        }
        count++;
        frequency = frequency+10000;

        tal_system_sleep(2000);
    }

    TUYA_CALL_ERR_LOG(tkl_pwm_stop(PWM_ID));

__EXIT:
    TAL_PR_NOTICE("PWM task is finished, will delete");
    tal_thread_delete(sg_pwm_handle);
    return;
}

/**
* @brief example_pwm_start_and_stop
*
* @return none
*/
VOID example_pwm(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;

    THREAD_CFG_T pwm_param = {
        .priority = TASK_PWM_PRIORITY,
        .stackDepth = TASK_PWM_SIZE,
        .thrdname = "pwm_task"
    };
    TUYA_CALL_ERR_LOG(tal_thread_create_and_start(&sg_pwm_handle, NULL, NULL, __example_pwm_task, NULL, &pwm_param));

    return;
}
