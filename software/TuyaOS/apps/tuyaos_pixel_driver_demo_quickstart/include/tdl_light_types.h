/**
 * @file tdl_light_types.h
 * @author www.tuya.com
 * @brief tdl_light_types module is used to provide common
 *        types of lighting products
 * @version 0.1
 * @date 2022-08-22
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#ifndef __TDL_LIGHT_TYPES_H__
#define __TDL_LIGHT_TYPES_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************macro define************************
***********************************************************/
/**
 * @brief light device type - bit code
 */
#define LIGHT_R_BIT 0x01
#define LIGHT_G_BIT 0x02
#define LIGHT_B_BIT 0x04
#define LIGHT_C_BIT 0x08
#define LIGHT_W_BIT 0x10

/**
 * @brief light device name length
 */
#define LIGHT_NAME_MAX_LEN 16u

/**
 * @brief parameter checking
 */
#define TY_APP_PARAM_CHECK(condition)                                                                                  \
    do {                                                                                                               \
        if (!(condition)) {                                                                                            \
            PR_ERR("Pre check(" #condition "):in line %d", __LINE__);                                                  \
            return OPRT_COM_ERROR;                                                                                     \
        }                                                                                                              \
    } while (0)

/***********************************************************
***********************typedef define***********************
***********************************************************/
/**
 * @brief light device type
 */
typedef BYTE_T LIGHT_DEV_TP_E;
#define LIGHT_DEV_TP_C     (LIGHT_C_BIT)
#define LIGHT_DEV_TP_CW    (LIGHT_C_BIT | LIGHT_W_BIT)
#define LIGHT_DEV_TP_RGB   (LIGHT_R_BIT | LIGHT_G_BIT | LIGHT_B_BIT)
#define LIGHT_DEV_TP_RGBC  (LIGHT_R_BIT | LIGHT_G_BIT | LIGHT_B_BIT | LIGHT_C_BIT)
#define LIGHT_DEV_TP_RGBW  (LIGHT_R_BIT | LIGHT_G_BIT | LIGHT_B_BIT | LIGHT_W_BIT)
#define LIGHT_DEV_TP_RGBCW (LIGHT_R_BIT | LIGHT_G_BIT | LIGHT_B_BIT | LIGHT_C_BIT | LIGHT_W_BIT)

/**
 * @brief light mode
 */
typedef BYTE_T LIGHT_MODE_E;
#define LIGHT_MODE_WHITE 0x00
#define LIGHT_MODE_COLOR 0x01
#define LIGHT_MODE_SCENE 0x02
#define LIGHT_MODE_MUSIC 0x03
#define LIGHT_MODE_NUM   0x04

/**
 * @brief hsv data
 */
typedef struct {
    USHORT_T hue;
    USHORT_T sat;
    USHORT_T val;
} COLOR_HSV_T;

/**
 * @brief rgb data
 */
typedef struct {
    USHORT_T red;
    USHORT_T green;
    USHORT_T blue;
} COLOR_RGB_T;

/**
 * @brief bt data
 */
typedef struct {
    USHORT_T bright;
    USHORT_T temper;
} WHITE_BT_T;

/**
 * @brief cw data
 */
typedef struct {
    USHORT_T cold;
    USHORT_T warm;
} WHITE_CW_T;

/**
 * @brief hsvbt data
 */
typedef struct {
    USHORT_T hue;
    USHORT_T sat;
    USHORT_T val;
    USHORT_T bright;
    USHORT_T temper;
} LIGHT_HSVBT_T;

/**
 * @brief rgbcw data
 */
typedef struct {
    USHORT_T red;
    USHORT_T green;
    USHORT_T blue;
    USHORT_T cold;
    USHORT_T warm;
} LIGHT_RGBCW_T, PIXEL_COLOR_T;

/**
 * @brief get switch state callback function type
 */
typedef BOOL_T (*LIGHT_GET_SWITCH_CB)(VOID_T);

/***********************************************************
********************function declaration********************
***********************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __TDL_LIGHT_TYPES_H__ */
