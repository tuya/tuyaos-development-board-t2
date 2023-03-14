/**
 * @file tdd_dimmer_types.h
 * @author www.tuya.com
 * @brief tdd_dimmer_types module is used to provide structure and define for dimmer drvier
 * @version 0.1
 * @date 2022-08-26
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#ifndef __TDD_DIMMER_TYPES_H__
#define __TDD_DIMMER_TYPES_H__

#include "tdu_light_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************macro define************************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef BYTE_T DIMMER_CH_ID_E;
#define DIMMER_CH_ID_1                          0x00
#define DIMMER_CH_ID_2                          0x01
#define DIMMER_CH_ID_3                          0x02
#define DIMMER_CH_ID_4                          0x03
#define DIMMER_CH_ID_5                          0x04
#define DIMMER_CH_ID_INVALID                    0xFE

#define DIMMER_RGB_CH_ID_MIN                    DIMMER_CH_ID_1
#define DIMMER_RGB_CH_ID_MAX                    DIMMER_CH_ID_3
#define DIMMER_CW_CH_ID_MIN                     DIMMER_CH_ID_4
#define DIMMER_CW_CH_ID_MAX                     DIMMER_CH_ID_5

#pragma pack(1)

typedef union {
    struct{
        DIMMER_CH_ID_E r;
        DIMMER_CH_ID_E g;
        DIMMER_CH_ID_E b;
        DIMMER_CH_ID_E c;
        DIMMER_CH_ID_E w;
    };

    DIMMER_CH_ID_E array[LIGHT_COLOR_CHANNEL_MAX];
}DIMMER_CH_CFG_U;

#pragma pack()

typedef union{
    struct {
        UCHAR_T r;
        UCHAR_T g;
        UCHAR_T b;
        UCHAR_T c;
        UCHAR_T w;
    };
    UCHAR_T array[LIGHT_COLOR_CHANNEL_MAX];
}CURRENT_RGBCW_CFG_U;

typedef struct{
    UCHAR_T color;
    UCHAR_T white;
}CURRENT_COLOR_CFG_T;


#ifdef __cplusplus
}
#endif

#endif /* __TDD_DIMMER_TYPES_H__ */
