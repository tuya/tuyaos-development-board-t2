/**
 * @file tdl_sensor_type.h
 * @brief tdl_sensor_type module is used to define sensor hub struct
 * @version 0.1
 * @date 2022-08-02
 */

#ifndef __TDL_SENSOR_TYPE_H__
#define __TDL_SENSOR_TYPE_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************macro define************************
***********************************************************/
/**
 * @brief config
 */
#define SR_CONFIG_NO_FPU        0

/**
 * @brief size
 */
#define SR_DEV_NAME_MAX_LEN     16u
#define SR_RAW_DATA_MAX_LEN     32u

/***********************************************************
***********************typedef define***********************
***********************************************************/
/**
 * @brief value type define
 */
typedef BYTE_T SR_VAL_TP_E;
#define SR_VAL_TP_INT           0x00
#define SR_VAL_TP_FLOAT         0x01

/**
 * @brief value union
 */
typedef union {
    INT_T       sr_int;
    FLOAT_T     sr_float;
} SR_VAL_U;

/**
 * @brief data struct (element type)
 */
typedef struct {
    UCHAR_T     id;
    SR_VAL_U    val;
} SR_ELE_DATA_T;

/**
 * @brief data struct (raw type)
 */
typedef struct {
    UINT_T      len;
    UCHAR_T     data[SR_RAW_DATA_MAX_LEN];
} SR_RAW_DATA_T;

#ifdef __cplusplus
}
#endif

#endif /* __TDL_SENSOR_TYPE_H__ */
