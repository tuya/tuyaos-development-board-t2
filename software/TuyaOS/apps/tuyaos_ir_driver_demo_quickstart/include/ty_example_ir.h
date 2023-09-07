/**
 * @file ty_example_ir.h
 * @author www.tuya.com
 * @brief ty_example_ir module is used to 
 * @version 0.1
 * @date 2023-05-31
 *
 * @copyright Copyright (c) tuya.inc 2023
 *
 */

#ifndef __TY_EXAMPLE_IR_H__
#define __TY_EXAMPLE_IR_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************macro define************************
***********************************************************/
#define IR_DEV_NAME     "ir_remote"

/***********************************************************
***********************typedef define***********************
***********************************************************/


/***********************************************************
********************function declaration********************
***********************************************************/

OPERATE_RET ir_hardware_register(VOID_T);

VOID_T ty_example_ir(VOID_T);

#ifdef __cplusplus
}
#endif

#endif /* __TY_EXAMPLE_IR_H__ */
