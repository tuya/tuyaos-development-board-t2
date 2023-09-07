/**
 * @file app_led.h
 * @author www.tuya.com
 * @brief app_led module is used to 
 * @version 0.1
 * @date 2022-10-28
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#ifndef __APP_LED_H__
#define __APP_LED_H__

#include "tuya_cloud_types.h"
#include "gw_intf.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************macro define************************
***********************************************************/
typedef UINT8_T LED_STATUS_E;
#ifndef LED_OFF
#define LED_OFF   0
#endif
#ifndef LED_ON
#define LED_ON    1
#endif

/***********************************************************
***********************typedef define***********************
***********************************************************/


/***********************************************************
********************function declaration********************
***********************************************************/

/**
 * @brief led init
 *
 * @param[in] pin_id: led pin id
 * 
 * @return none
 */
VOID_T app_led_init(TUYA_GPIO_NUM_E pin_id);

/**
 * @brief set led
 *
 * @param[in] led_status: LED_ON: led turn on, LED_OFF: led turn off
 * 
 * @return none
 */
VOID_T set_led_status(LED_STATUS_E led_status);

/**
 * @brief get led status, on or off
 *
 * @param[in] none
 * 
 * @return current led status
 */
LED_STATUS_E get_led_status(VOID_T);

#ifdef __cplusplus
}
#endif

#endif /* __APP_LED_H__ */
