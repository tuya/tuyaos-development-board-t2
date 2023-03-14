/**
 * @file app_main.c
 * @author www.tuya.com
 * @brief app_main module is used to 
 * @version 0.1
 * @date 2022-11-03
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include "tuya_cloud_com_defs.h"
#include "tal_thread.h"
#include "tal_log.h"
#include "tal_system.h"
#if defined(ENABLE_LWIP) && (ENABLE_LWIP == 1)
#include "lwip_init.h"
#endif
#include "tkl_gpio.h"
#include "tkl_pwm.h"

#include "tdl_ele_energy_ops.h"
#include "tdd_ele_energy_bl0937_hlw8012.h"

/***********************************************************
************************macro define************************
***********************************************************/


/***********************************************************
***********************typedef define***********************
***********************************************************/


/***********************************************************
********************function declaration********************
***********************************************************/


/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC THREAD_HANDLE ty_app_thread = NULL;

/***********************************************************
***********************function define**********************
***********************************************************/

/**
 * @brief task thread
 *
 * @param[in] arg: Parameters when creating a task
 *
 * @return none
 */
STATIC VOID_T __ele_energy_demo(VOID_T)
{
    OPERATE_RET rt = OPRT_OK;

    TDL_ELE_ENERGY_CFG_T ele_energy_cfg = {0};

    UINT_T pwr = 0, volt = 0, curr = 0;
    UINT_T energy = 0;

    ele_energy_cfg.mode = TDL_ELE_NORMAL_MODE;
    ele_energy_cfg.params.volt_type = TDL_ELE_VOLT_220V;

    /* 未校准过，采用芯片默认的校准参数，用户也可以自行设置 */
    ele_energy_cfg.fac_test.test_result = TDL_ELE_TEST_FAIL; // 不使用校准，直接默认校准失败，采用芯片内部默认的校准参数
    TAL_PR_DEBUG("ele_energy_cfg.fac_test, ret:%d, v_coe:%d, i_coe%d, p_coe%d, e_coe%d",
                ele_energy_cfg.fac_test.test_result, ele_energy_cfg.fac_test.coe_data.v_coe,
                ele_energy_cfg.fac_test.coe_data.i_coe, ele_energy_cfg.fac_test.coe_data.p_coe,
                ele_energy_cfg.fac_test.coe_data.e_coe);
    
    // BL0937
    ele_energy_cfg.params.resval = 2;      // 采样电阻2R，根据实际硬件电路设计来
    ele_energy_cfg.params.err_percent = 0; // 采用芯片内部默认产测误差
    TDD_BL0937_HLW8012_DRV_RESOURCE_T bl0937_resource;
    bl0937_resource.timer_no = TIMER_NUM_0;
    bl0937_resource.epin = GPIO_NUM_24; // pwm4, ele pin, cf
    bl0937_resource.ivpin = GPIO_NUM_26; // pwm5, vi pin, cf1
    bl0937_resource.ivcpin.pin = GPIO_NUM_14; // select pin
    bl0937_resource.ivcpin.type = TDL_ELE_IO_DRV_HIGH;
    TUYA_CALL_ERR_LOG(tdd_ele_energy_bl0937_init(&ele_energy_cfg, &bl0937_resource));

    for (;;) {
        tdl_ele_energy_read_pvi_value(&pwr, &volt, &curr);
        tdl_ele_energy_read_enery_value(&energy);
        TAL_PR_DEBUG("curr pvie data, P:%d, V:%d, I:%d, E:%d", pwr, volt, curr, energy);
        tal_system_sleep(3000);
    }
}

STATIC VOID_T tuya_app_thread(VOID_T *arg)
{
    OPERATE_RET rt = OPRT_OK;

    // Initialization LWIP, must be before tuya_iot_init_params()
#if defined(ENABLE_LWIP) && (ENABLE_LWIP == 1)
    TUYA_LwIP_Init();
#endif

    // Initialization, because DB initialization takes a long time, 
    // which affects the startup efficiency of some devices, 
    // so special processing is performed during initialization to delay initialization of DB    
    TY_INIT_PARAMS_S init_param = {0};
    init_param.init_db = TRUE;
    strcpy(init_param.sys_env, TARGET_PLATFORM);
    TUYA_CALL_ERR_LOG(tuya_iot_init_params(NULL, &init_param));

    /* ele energy demo */
    __ele_energy_demo();

    tal_thread_delete(ty_app_thread);
    ty_app_thread = NULL;

    return;
}

/**
 * @brief user entry function
 *
 * @param[in] none: 
 *
 * @return none
 */
VOID_T tuya_app_main(VOID_T)
{
    OPERATE_RET rt = OPRT_OK;
    THREAD_CFG_T thrd_param = {4096, 4, "tuya_app_main"};

    TUYA_CALL_ERR_LOG(tal_thread_create_and_start(&ty_app_thread, NULL, NULL, tuya_app_thread, NULL, &thrd_param));

    return;
}
