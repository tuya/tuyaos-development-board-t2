/**
* @file tdd_pixel_ws2814.h
* @author www.tuya.com
* @brief tdd_pixel_ws2814 module is used to driving ws2814 chip
* @version 0.1
* @date 2022-03-08
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#ifndef __TDD_PIXEL_WS2814_H__
#define __TDD_PIXEL_WS2814_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
******************************macro define****************************
*********************************************************************/


/*********************************************************************
****************************typedef define****************************
*********************************************************************/


/*********************************************************************
****************************variable define***************************
*********************************************************************/


/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @brief tdd_ws2814_driver_register
*
* @param[in] driver_name
* @param[in] order_mode
* @return 
*/
OPERATE_RET tdd_ws2814_driver_register(IN CHAR_T *driver_name, IN UCHAR_T order_mode);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__TDD_PIXEL_WS2814_H__*/


