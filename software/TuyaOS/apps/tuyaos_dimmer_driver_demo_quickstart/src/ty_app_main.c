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
#include "tal_log.h"
#include "tal_system.h"
#include "tal_thread.h"
#include "tuya_cloud_com_defs.h"

#include "ty_example_light.h"
/***********************************************************
*************************micro define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC THREAD_HANDLE ty_app_thread = NULL;


/***********************************************************
***********************function define**********************
***********************************************************/
VOID_T example_main(VOID_T)
{
    OPERATE_RET rt =  OPRT_OK;
    TY_INIT_PARAMS_S init_param = {0};

    //涂鸦系统服务初始化
    init_param.init_db = FALSE;
    strcpy(init_param.sys_env, TARGET_PLATFORM);
    TUYA_CALL_ERR_LOG(tuya_iot_init_params(NULL, &init_param));

    TAL_PR_NOTICE("sdk_info:%s", tuya_iot_get_sdk_info()); /* print SDK information */
    TAL_PR_NOTICE("name:%s:%s", APP_BIN_NAME, USER_SW_VER);                                    /* print the firmware name and version */
    TAL_PR_NOTICE("firmware compiled at %s %s", __DATE__, __TIME__);              /* print firmware compilation time */
    TAL_PR_NOTICE("system reset reason:[%d]", tal_system_get_reset_reason(NULL)); /* print system reboot causes */

    //example entrance
    ty_example_light();

    return;
}

static void tuya_app_thread(void *arg)
{
    example_main();

    tal_thread_delete(ty_app_thread);
    ty_app_thread = NULL;

    return;
}

void tuya_app_main(void)
{
    THREAD_CFG_T thrd_cfg = {
        .priority   = THREAD_PRIO_1,
        .stackDepth = 4096,
        .thrdname   = "tuya_app_main"
    };

    tal_thread_create_and_start(&ty_app_thread, NULL, NULL, tuya_app_thread, NULL, &thrd_cfg);

    return;
}