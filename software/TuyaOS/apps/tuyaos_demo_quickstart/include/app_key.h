/**
 * @file app_key.h
 * @author www.tuya.com
 * @brief app_key module is used to 
 * @version 0.1
 * @date 2022-10-28
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#ifndef __APP_KEY_H__
#define __APP_KEY_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************macro define************************
***********************************************************/


/***********************************************************
***********************typedef define***********************
***********************************************************/


/***********************************************************
********************function declaration********************
***********************************************************/

/**
 * @brief key gpio init, creat key thread
 *
 * @param[in] pin_id: key pin id
 * 
 * @return none
 */
VOID_T app_key_init(TUYA_GPIO_NUM_E pin_id);

#ifdef __cplusplus
}
#endif

#endif /* __APP_KEY_H__ */
