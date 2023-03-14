/**
* @file tdd_pixel_sk6812.h
* @author www.tuya.com
* @brief tdd_pixel_sk6812 module is used to driving sk6812 chip
* @version 0.1
* @date 2022-03-08
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#ifndef __TDD_PIXEL_SK6812_H__
#define __TDD_PIXEL_SK6812_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"

/*********************************************************************
******************************macro define****************************
*********************************************************************/

/*********************************************************************
****************************typedef define****************************
*********************************************************************/

/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @function: tdd_sk6812_driver_register
* @brief: 注册设备
* @param[in] driver_name
* @param[in] order_mode
* @return: success -> OPRT_OK
*/
OPERATE_RET tdd_sk6812_driver_register(IN CHAR_T *driver_name, IN UCHAR_T order_mode);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__TDD_PIXEL_SK6812_H__*/


