/**
 * @file tdl_ele_energy_ops.c
 * @author www.tuya.com
 * @brief tdl_ele_energy_ops module is used to
 * @version 1.0.0
 * @date 2022-05-12
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

/*c lib*/
#include <string.h>
/*sdk*/
#include "uni_log.h"
#include "sys_timer.h"

#include "tdl_ele_energy_ops.h"

/***********************************************************
*************************micro define***********************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC TDL_ELE_ENERGY_PVIE_T sg_pvie_value; // 测量值
STATIC TDL_ELE_ENERGY_DRV_OPS_T sg_ops;

/***********************************************************
***********************function define**********************
***********************************************************/

/**
 * @brief 定时读采样数据
 *
 * @param[in] timerID 定时器id
 * @param[in] pTimerArg arg param
 * @return VOID_T
 */
STATIC VOID_T __ele_energy_timer_cb(IN UINT_T timerID, IN PVOID_T pTimerArg)
{
    if (NULL == sg_ops.drv_read_pvie_cb) {
        PR_ERR("get pvie value api is null");
        return;
    }

    STATIC TDL_ELE_ENERGY_PVIE_T energy_data; // 必须用静态,不然采样未结束时，会出现0值跳变
    sg_ops.drv_read_pvie_cb(&energy_data);

    sg_pvie_value.power_val = energy_data.power_val;
    sg_pvie_value.volt_val = energy_data.volt_val;
    sg_pvie_value.curr_val = energy_data.curr_val;
    sg_pvie_value.energy_val += energy_data.energy_val;

    energy_data.energy_val = 0;

    return;
}

/**
 * @brief 计量芯片驱动注册
 *        不支持重入
 * @param[in] chip_info 芯片信息
 * @return OPRT_OK 代表成功。如果是其他错误码，请参考 tuya_error_code.h
 */
OPERATE_RET tdl_ele_energy_driver_register(IN TDL_ELE_ENERGY_DRIVER_T *chip_info)
{
    if (NULL == chip_info) {
        return OPRT_INVALID_PARM;
    }

    STATIC BOOL_T ele_energy_is_reg = FALSE;
    OPERATE_RET ret = OPRT_OK;
    TIMER_ID ele_energy_timer;

    if (TRUE == ele_energy_is_reg) {
        PR_NOTICE("ele energy driver already registered, duplicate registration is not supported");
        return OPRT_OK;
    }

    PR_NOTICE("ele energy chip name:%s", chip_info->name);
    memset(&sg_ops, 0x00, SIZEOF(sg_ops));
    memcpy(&sg_ops, chip_info->ops, SIZEOF(sg_ops));

    if (NULL == sg_ops.drv_init_cb) {
        return OPRT_INVALID_PARM;
    }

    ret = sg_ops.drv_init_cb(chip_info->cfg, chip_info->platform_resource);
    if (OPRT_OK != ret) {
        return ret;
    }

    if (sg_ops.drv_ctrl_cb) {
        sg_ops.drv_ctrl_cb(TRUE);
    }

    memset(&sg_pvie_value, 0, SIZEOF(sg_pvie_value));
    ret = sys_add_timer(__ele_energy_timer_cb, NULL, &ele_energy_timer);
    if (OPRT_OK != ret) {
        PR_ERR("ele_energy timer init err:%d", ret);
        return ret;
    }
    sys_start_timer(ele_energy_timer, 300, TIMER_CYCLE);

    if (TDL_ELE_TEST_MODE == chip_info->cfg->mode) {
        PR_NOTICE("ele_energy module init success, test mode");
    } else {
        PR_NOTICE("ele_energy module init success, normal mode");
    }

    ele_energy_is_reg = TRUE;
    return OPRT_OK;
}

/**
 * @brief 计量芯片控制
 *
 * @param[in] state TRUE 开启计量 FALSE 关闭计量
 * @return OPRT_OK 代表成功。如果是其他错误码，请参考 tuya_error_code.h
 */
OPERATE_RET tdl_ele_energy_ctrl(IN BOOL_T state)
{
    if (NULL == sg_ops.drv_ctrl_cb) {
        PR_ERR("drv ctrl cb is null");
        return OPRT_INVALID_PARM;
    }

    sg_ops.drv_ctrl_cb(state);

    return OPRT_OK;
}

/**
 * @brief 读产测校准数据
 *
 * @param[out] fac_test_data 产测数据
 * @return OPRT_OK 代表成功。如果是其他错误码，请参考 tuya_error_code.h
 */
OPERATE_RET tdl_ele_energy_read_fac_test_data(OUT TDL_ELE_ENERGY_FAC_TEST_T *fac_test_data)
{
    if ((NULL == fac_test_data) || (NULL == sg_ops.drv_read_fac_test_data_cb)) {
        return OPRT_INVALID_PARM;
    }

    sg_ops.drv_read_fac_test_data_cb(fac_test_data);

    return OPRT_OK;
}

/**
 * @brief 读计量芯片当前产测状态
 *
 * @return TRUE 产测结束 FASLE 产测未开始或未结束
 */
BOOL_T tdl_ele_energy_read_fac_test_state(VOID_T)
{
    BOOL_T ret = FALSE;

    ret = (sg_ops.drv_read_fac_test_state_cb) ? (sg_ops.drv_read_fac_test_state_cb()) : (FALSE);

    return ret;
}

/**
 * @brief 获取电压，电流，功率采样数据(如果仅需要其中某一项参数，别的传参填NULL即可)
 *
 * @param[out] pwr 功率值
 * @param[out] volt_val 电压值
 * @param[out] curr_val 电流值
 * @return VOID_T
 */
VOID_T tdl_ele_energy_read_pvi_value(OUT UINT_T *pwr, OUT UINT_T *volt_val, OUT UINT_T *curr_val)
{
    if (pwr) {
        *pwr = sg_pvie_value.power_val;
    }

    if (volt_val) {
        *volt_val = sg_pvie_value.volt_val;
    }

    if (curr_val) {
        *curr_val = sg_pvie_value.curr_val;
    }

    return;
}

/**
 * @brief 获取单位时间内的累计电量值
 *
 * @param[out] enry 累计电量值
 * @return VOID_T
 */
VOID_T tdl_ele_energy_read_enery_value(OUT UINT_T *enry)
{
    if (NULL == enry) {
        return;
    }

    *enry = sg_pvie_value.energy_val;
    sg_pvie_value.energy_val = 0; // 读取完以后就清0

    return;
}

/**
 * @brief 范围判断
 *
 * @param[in] src_value 源值
 * @param[in] tag_value 目标值
 * @param[in] range 范围值
 * @return TRUE 范围值内 FASLE 范围值外
 */
BOOL_T tdl_ele_energy_rang_judge(IN UINT_T src_value, IN UINT_T tag_value, IN UINT_T range)
{
    if ((src_value * 100 >= tag_value * (100 - range)) && (src_value * 100 <= tag_value * (100 + range))) {
        return TRUE;
    }

    return FALSE;
}
