/**
 * @file tdd_ir_driver.h
 * @author www.tuya.com
 * @brief tdd_ir_driver module is used to 
 * @version 0.1
 * @date 2022-06-09
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#ifndef __TDD_IR_DRIVER_H__
#define __TDD_IR_DRIVER_H__

#include "tdl_ir_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************macro define************************
***********************************************************/


/***********************************************************
***********************typedef define***********************
***********************************************************/

typedef unsigned char IR_DRIVER_TYPE_E;
#define IR_DRV_SINGLE_TIMER     0
#define IR_DRV_DUAL_TIMER       1
#define IR_DRV_CAPTURE          2
#define IR_DRV_TYPE_MAX         3

typedef struct {
    int send_pin; /* This pin should support 38kHz pwm output */
    int recv_pin;
    int send_timer;
    int recv_timer; /* Use only in IR_DRV_DUAL_TIMER */
    int send_duty;
}IR_DRV_CFG_T;

/***********************************************************
********************function declaration********************
***********************************************************/

/**
 * @brief register ir driver, maximum 16 bytes
 *
 * @param[in] dev_name: device name, 
 * @param[in] drv_cfg: driver config params
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
int tdd_ir_driver_register(const char *dev_name, IR_DRIVER_TYPE_E driver_type, IR_DRV_CFG_T drv_cfg);

#ifdef __cplusplus
}
#endif

#endif /* __TDD_IR_DRIVER_H__ */
