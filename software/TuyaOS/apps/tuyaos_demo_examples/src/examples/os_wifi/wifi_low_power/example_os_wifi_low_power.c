/**
* @file examples_os_wifi_low_power.c
* @author www.tuya.com
* @brief examples_os_wifi_low_power module is used to 
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
#include "tal_sleep.h"
#include "tal_wifi.h"

/***********************************************************
*************************micro define***********************
***********************************************************/


/***********************************************************
***********************typedef define***********************
***********************************************************/


/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC THREAD_HANDLE __wifi_lp_hdl1 = NULL;
STATIC THREAD_HANDLE __wifi_lp_hdl2 = NULL;

/***********************************************************
***********************function define**********************
***********************************************************/

STATIC VOID __wifi_low_power_task1(VOID_T *arg)
{
    for (;;) {
        tal_wifi_lp_disable();
        /* exit low-power do something */
        tal_system_sleep(3000);
        tal_wifi_lp_enable();

        tal_system_sleep(5000);
    }
    tal_thread_delete(__wifi_lp_hdl1);
}

STATIC VOID __wifi_low_power_task2(VOID_T *arg)
{
    for (;;) {
        tal_wifi_lp_disable();
        /* exit low-power do something */
        tal_system_sleep(3000);
        tal_wifi_lp_enable();

        tal_system_sleep(7000);
    }
    tal_thread_delete(__wifi_lp_hdl2);
}

VOID example_wifi_low_power(INT_T argc, CHAR_T *argv[])
{
    /*enable low power*/
    tal_cpu_set_lp_mode(TRUE);

    /* The higher the dtim, the lower the power consumption */
    tal_wifi_set_lps_dtim(2);
    /* enter tickless&WiFi low-power mode */
    tal_wifi_lp_enable();

    THREAD_CFG_T thrd_param = {1024, 4, "low_power_1"};
    tal_thread_create_and_start(&__wifi_lp_hdl1, NULL, NULL, __wifi_low_power_task1, NULL, &thrd_param);

    thrd_param.thrdname = "low_power_2";
    tal_thread_create_and_start(&__wifi_lp_hdl2, NULL, NULL, __wifi_low_power_task2, NULL, &thrd_param);

    return;
}


