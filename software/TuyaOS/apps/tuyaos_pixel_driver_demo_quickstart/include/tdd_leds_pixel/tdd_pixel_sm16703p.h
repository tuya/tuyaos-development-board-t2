/**
* @file tdd_pixel_sm16703p.h
* @author www.tuya.com
* @brief tdd_pixel_sm16703p module is used to driving sm16703p chip
* @version 0.1
* @date 2022-03-08
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#ifndef __TDD_PIXEL_SM16703P_H__
#define __TDD_PIXEL_SM16703P_H__

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
* @function:tdd_sm16703p_driver_register
* @brief: 注册设备
* @param[in]: *driver_name -> 设备名
* @return: success -> OPRT_OK
*/
OPERATE_RET tdd_sm16703p_driver_register(IN CHAR_T *driver_name,IN UCHAR_T order_mode);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__TDD_PIXEL_SM16703P_H__*/


