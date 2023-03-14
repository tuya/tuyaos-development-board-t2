/**
 * @file tuya_device.c
 * @author www.tuya.com
 * @brief tuya_device module is used to 
 * @version 0.1
 * @date 2022-09-20
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include "tuya_iot_config.h"
#include "tuya_cli_register.h"
#include "tuya_cloud_types.h"
#include "tuya_iot_com_api.h"
#include "tal_thread.h"
#include "tal_log.h"

#if defined(ENABLE_LWIP) && (ENABLE_LWIP == 1)
#include "lwip_init.h"
#endif

/***********************************************************
************************macro define************************
***********************************************************/
#define PID "tvjs8eysp0imads8"

/* Please change this to your UUID and AUTHKEY.
    For more information, please see 
    "https://developer.tuya.com/cn/docs/iot/lisence-management?id=Kb4qlem97idl0" .
*/
// #define UUID "tuyaXXXXXXXXXXXX5060"
// #define AUTHKEY "YOkWXXXXXXXXXXXXXXXXXXXXXXXXsL21"

/***********************************************************
***********************typedef define***********************
***********************************************************/


/***********************************************************
********************function declaration********************
***********************************************************/

extern VOID example_soc_device_pid_set(CHAR_T *pid);
extern VOID example_mf_init(VOID);

/***********************************************************
***********************variable define**********************
***********************************************************/

STATIC THREAD_HANDLE ty_app_thread = NULL;

CHAR_T product_id[] = PID;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief Initialization device
 *
 * @param[in] none: 
 *
 * @return none
 */
STATIC VOID_T user_main(VOID_T)
{
    OPERATE_RET rt = OPRT_OK;

    /* LwIP Initialization */
#if defined(ENABLE_LWIP) && (ENABLE_LWIP == 1)
    TUYA_LwIP_Init();
#endif

    /* TuyaOS System Service Initialization */
    TY_INIT_PARAMS_S init_param = {0};
    init_param.init_db = TRUE;
    strcpy(init_param.sys_env, TARGET_PLATFORM);
    TUYA_CALL_ERR_LOG(tuya_iot_init_params(NULL, &init_param));

#ifdef UUID
    /* Write authorization information to flash */
    WF_GW_PROD_INFO_S prod_info = {UUID, AUTHKEY};
    ws_db_init_mf();
    TUYA_CALL_ERR_LOG(tuya_iot_set_wf_gw_prod_info(&prod_info));
#else
    /* Authorization using the host computer */
    example_mf_init();
#endif

    /* Examples function register */
    examples_cli_register();

    /* CLI service initialized and started */
    TUYA_CALL_ERR_LOG(tuya_cli_init_with_uart(UART_NUM_0));

    return;
}

/**
 * @brief tuyaos Initialization thread
 *
 * @param[in] arg: Parameters when creating a task
 *
 * @return none
 */
STATIC VOID_T tuya_app_thread(VOID_T *arg)
{
    user_main();

    tal_thread_delete(ty_app_thread);
    ty_app_thread = NULL;
}

/**
 * @brief tuyaos entry function, it will call by main
 *
 * @param[in] none: 
 *
 * @return none
 */
VOID_T tuya_app_main(VOID_T)
{
    THREAD_CFG_T thrd_param = {4096, 4, "tuya_app_main"};
    tal_thread_create_and_start(&ty_app_thread, NULL, NULL, tuya_app_thread, NULL, &thrd_param);
}
