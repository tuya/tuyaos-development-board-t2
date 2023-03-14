/**
 * @file tdd_ele_energy_hlw8032.h
 * @author www.tuya.com
 * @brief tdd_ele_energy_hlw8032 module is used to
 * @version 1.0.0
 * @date 2022-05-12
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#ifndef __TDD_ELE_ENERGY_HLW8032_H__
#define __TDD_ELE_ENERGY_HLW8032_H__

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
    /*串口选择*/
    UCHAR_T uart_no;
    UCHAR_T timer_no; // 硬件定时num
} TDD_HLW8032_DRV_RESOURCE_T;

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief hlw8032驱动初始化
 *
 * @param[in] drv_cfg 芯片配置参数
 * @param[in] dev_info 平台资源信息
 * @return OPRT_OK 代表成功。如果是其他错误码，请参考 tuya_error_code.h
 */
OPERATE_RET tdd_ele_energy_hlw8032_init(IN CONST TDL_ELE_ENERGY_CFG_T *drv_cfg,
                                        IN CONST TDD_HLW8032_DRV_RESOURCE_T *dev_info);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__TDD_ELE_ENERGY_HLW8032_H__*/