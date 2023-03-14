/**
 * @file tdd_ele_energy_bl0937_hlw8012.h
 * @author www.tuya.com
 * @brief tdd_ele_energy_bl0937_hlw8012 module is used to
 * @version 1.0.0
 * @date 2022-05-12
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#ifndef __TDD_ELE_ENERGY_BL0937_HLW8012_H__
#define __TDD_ELE_ENERGY_BL0937_HLW8012_H__

#include "tuya_cloud_types.h"
#include "tdl_ele_energy_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef struct {
    /*硬件定时器号*/
    UCHAR_T timer_no;
    /*功率检测引脚*/
    UCHAR_T epin;
    /*电压、电流检测管脚*/
    UCHAR_T ivpin;
    /*sel脚*/
    TDL_ELE_ENERGY_IO_CFG_T ivcpin;
} TDD_BL0937_HLW8012_DRV_RESOURCE_T;

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief bl0937初始化
 *
 * @param[in] drv_cfg 芯片配置参数
 * @param[in] platform_resource 驱动芯片所需的外设资源
 * @return OPRT_OK 代表成功。如果是其他错误码，请参考 tuya_error_code.h
 */
OPERATE_RET tdd_ele_energy_bl0937_init(IN CONST TDL_ELE_ENERGY_CFG_T *drv_cfg,
                                       IN CONST TDD_BL0937_HLW8012_DRV_RESOURCE_T *platform_resource);

/**
 * @brief hlw8012初始化
 *
 * @param[in] drv_cfg 芯片配置参数
 * @param[in] platform_resource 驱动芯片所需的外设资源
 * @return OPRT_OK 代表成功。如果是其他错误码，请参考 tuya_error_code.h
 */
OPERATE_RET tdd_ele_energy_hlw8012_init(IN CONST TDL_ELE_ENERGY_CFG_T *drv_cfg,
                                        IN CONST TDD_BL0937_HLW8012_DRV_RESOURCE_T *platform_resource);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__TDD_ELE_ENERGY_BL0937_HLW8012_H__*/
