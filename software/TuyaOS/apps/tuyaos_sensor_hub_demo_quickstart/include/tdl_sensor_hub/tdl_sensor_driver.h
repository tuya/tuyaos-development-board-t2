/**
 * @file tdl_sensor_driver.h
 * @brief tdl_sensor_driver module is used to provid sensor driver access standard
 * @version 0.1
 * @date 2022-08-02
 */

#ifndef __TDL_SENSOR_DRIVER_H__
#define __TDL_SENSOR_DRIVER_H__

#include "tdl_sensor_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************macro define************************
***********************************************************/
/**
 * @brief custom resource buffer size
 */
#define SR_RSRC_INFO_LEN        8u

/***********************************************************
***********************typedef define***********************
***********************************************************/
/**
 * @brief element configuration
 */
typedef struct {
    UCHAR_T     id;                         // element ID
    SR_VAL_TP_E val_tp;                     // element value's type
} SR_ELE_CFG_T;

/**
 * @brief resource buffer
 */
typedef struct {
    UCHAR_T     type;                       // resource type
    UCHAR_T     port;                       // resource port
    VOID_T     *handle;                     // resource handle
    UCHAR_T     info[SR_RSRC_INFO_LEN];     // custom resource information
} SR_RSRC_T;

/**
 * @brief interfaces
 */
typedef struct {
    OPERATE_RET (*open)(SR_RSRC_T* dev);
    OPERATE_RET (*close)(SR_RSRC_T* dev);
    OPERATE_RET (*control)(SR_RSRC_T* dev, UCHAR_T cmd, VOID_T *param);
    OPERATE_RET (*read_ele)(SR_RSRC_T* dev, SR_ELE_DATA_T *ele_data, UCHAR_T ele_num);
    OPERATE_RET (*read_raw)(SR_RSRC_T* dev, SR_RAW_DATA_T *raw_data);
} SR_INTFS_T;

/***********************************************************
********************function declaration********************
***********************************************************/
/**
 * @brief register sensor
 *
 * @param[in] name: device name
 * @param[in] intfs: interface for operating the sensor
 * @param[in] ele_num: number of elements, 0 means the element type is raw
 * @param[in] ele_cfg: data type configuration for each element
 * @param[in] resource: resource that device depends on
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdl_sensor_register(IN CHAR_T *name, IN SR_INTFS_T *intfs, IN UCHAR_T ele_num,
                                IN SR_ELE_CFG_T *ele_cfg, IN SR_RSRC_T *resource);

#ifdef __cplusplus
}
#endif

#endif /* __TDL_SENSOR_DRIVER_H__ */
