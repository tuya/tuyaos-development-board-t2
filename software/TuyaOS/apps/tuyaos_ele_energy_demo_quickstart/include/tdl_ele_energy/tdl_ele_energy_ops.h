/**
 * @file tdl_ele_energy_ops.h
 * @author www.tuya.com
 * @brief tdl_ele_energy_ops module is used to
 * @version 1.0.0
 * @date 2022-05-12
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#ifndef __TDL_ELE_ENERGY_OPS_H__
#define __TDL_ELE_ENERGY_OPS_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
/* 220V 产测参考电压、电流、功率 */
#define TDL_ELE_V_REF_220 2200
#define TDL_ELE_I_REF_220 392
#define TDL_ELE_P_REF_220 864

/* 120V 产测参考电压、电流、功率 */
#define TDL_ELE_V_REF_120 1200
#define TDL_ELE_I_REF_120 833
#define TDL_ELE_P_REF_120 1000

/***********************************************************
***********************typedef define***********************
***********************************************************/

typedef enum {
    TDL_ELE_VOLT_220V = 0,
    TDL_ELE_VOLT_120V = 1,
} TDL_ELE_ENERGY_VOLT_TYPE_E;

typedef enum {
    TDL_ELE_NORMAL_MODE = 0,
    TDL_ELE_TEST_MODE,
} TDL_ELE_ENERGY_MODE_E;

typedef enum {
    TDL_ELE_IO_DRV_LOW,  // 低电平有效
    TDL_ELE_IO_DRV_HIGH, // 高电平有效
    TDL_ELE_IO_DRV_ERR,  // 该IO不存在
} TDL_ELE_ENERGY_IO_DRV_TYPE_E;

typedef enum {
    TDL_ELE_TEST_PASS = 1,
    TDL_ELE_TEST_FAIL = 2,
} TDL_ELE_ENERGY_TEST_RESULT_E;

typedef struct {
    TDL_ELE_ENERGY_IO_DRV_TYPE_E type; // 有效电平类型
    UCHAR_T pin;                       // 引脚号
} TDL_ELE_ENERGY_IO_CFG_T;

typedef struct {
    UCHAR_T err_percent; // 产测通过范围，0：完全一致； 10: 采样和参考值允许10%误差; 最大100%
    UCHAR_T resval;      // 采样电阻规格 bl0937_hlw8012可选择:1\2 毫欧;bl0942和hlw8032默认1mR
    TDL_ELE_ENERGY_VOLT_TYPE_E volt_type;
} TDL_ELE_ENERGY_CFG_PARAMS_T;

/*校准参数v,i,p,e*/
typedef struct {
    UINT_T v_coe;
    UINT_T i_coe;
    UINT_T p_coe;
    UINT_T e_coe;
} TDL_ELE_ENERGY_COE_T;

typedef struct {
    UINT_T power_val;
    UINT_T volt_val;
    UINT_T curr_val;
    UINT_T energy_val;
} TDL_ELE_ENERGY_PVIE_T;

typedef struct {
    TDL_ELE_ENERGY_TEST_RESULT_E test_result;
    TDL_ELE_ENERGY_COE_T coe_data; // 默认的校准数据
} TDL_ELE_ENERGY_FAC_TEST_T;

typedef struct {
    TDL_ELE_ENERGY_FAC_TEST_T fac_test;
    TDL_ELE_ENERGY_CFG_PARAMS_T params;
    TDL_ELE_ENERGY_MODE_E mode;
} TDL_ELE_ENERGY_CFG_T;

typedef struct {
    OPERATE_RET (*drv_init_cb)(IN CONST TDL_ELE_ENERGY_CFG_T *drv_cfg, IN CONST VOID_T *platform_resource);
    VOID_T (*drv_ctrl_cb)(BOOL_T state);
    VOID_T (*drv_read_pvie_cb)(OUT TDL_ELE_ENERGY_PVIE_T *pvie_val);
    VOID_T (*drv_read_fac_test_data_cb)(OUT TDL_ELE_ENERGY_FAC_TEST_T *fac_test_data);
    BOOL_T (*drv_read_fac_test_state_cb)(VOID_T);
} TDL_ELE_ENERGY_DRV_OPS_T;

typedef struct {
    CONST CHAR_T *name;                  // 驱动名
    CONST TDL_ELE_ENERGY_DRV_OPS_T *ops; // 驱动操作
    CONST TDL_ELE_ENERGY_CFG_T *cfg;     // 驱动参数配置
    CONST VOID_T *platform_resource;     // 平台资源配置
} TDL_ELE_ENERGY_DRIVER_T;

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief 计量芯片驱动注册
 *        不支持重入
 * @param[in] chip_info 芯片信息
 * @return OPRT_OK 代表成功。如果是其他错误码，请参考 tuya_error_code.h
 */
OPERATE_RET tdl_ele_energy_driver_register(IN TDL_ELE_ENERGY_DRIVER_T *chip_info);

/**
 * @brief 计量芯片控制
 *
 * @param[in] state TRUE 开启计量 FALSE 关闭计量
 * @return OPRT_OK 代表成功。如果是其他错误码，请参考 tuya_error_code.h
 */
OPERATE_RET tdl_ele_energy_ctrl(IN BOOL_T state);

/**
 * @brief 读产测校准数据
 *
 * @param[out] fac_test_data 产测数据
 * @return OPRT_OK 代表成功。如果是其他错误码，请参考 tuya_error_code.h
 */
OPERATE_RET tdl_ele_energy_read_fac_test_data(OUT TDL_ELE_ENERGY_FAC_TEST_T *fac_test_data);

/**
 * @brief 读计量芯片当前产测状态
 *
 * @return TRUE 产测结束 FASLE 产测未开始或未结束
 */
BOOL_T tdl_ele_energy_read_fac_test_state(VOID_T);

/**
 * @brief 获取电压，电流，功率采样数据(如果仅需要其中某一项参数，别的传参填NULL即可)
 *
 * @param[out] pwr 功率值
 * @param[out] volt_val 电压值
 * @param[out] curr_val 电流值
 * @return VOID_T
 */
VOID_T tdl_ele_energy_read_pvi_value(OUT UINT_T *pwr, OUT UINT_T *volt_val, OUT UINT_T *curr_val);

/**
 * @brief 获取单位时间内的累计电量值
 *
 * @param[out] enry 累计电量值
 * @return VOID_T
 */
VOID_T tdl_ele_energy_read_enery_value(OUT UINT_T *enry);

/**
 * @brief 范围判断
 *
 * @param[in] src_value 源值
 * @param[in] tag_value 目标值
 * @param[in] range 范围值
 * @return TRUE 范围值内 FASLE 范围值外
 */
BOOL_T tdl_ele_energy_rang_judge(IN UINT_T src_value, IN UINT_T tag_value, IN UINT_T range);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__TDL_ELE_ENERGY_OPS_H__*/
