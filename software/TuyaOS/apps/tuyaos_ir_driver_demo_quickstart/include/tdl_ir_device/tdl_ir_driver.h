/**
 * @file tdl_ir_driver.h
 * @author www.tuya.com
 * @brief tdl_ir_driver module is used to 
 * @version 0.1
 * @date 2022-06-06
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#ifndef __TDL_IR_DRIVER_H__
#define __TDL_IR_DRIVER_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************macro define************************
***********************************************************/
#define IR_INPUT_INVALID        (-1)

/* ir mode */
typedef unsigned char           IR_MODE_E;
#define IR_MODE_RECV_ONLY       0
#define IR_MODE_SEND_ONLY       1
#define IR_MODE_SEND_RECV       2
#define IR_MODE_MAX             3

typedef unsigned char           IR_DRIVER_STATE_E;
#define IR_DRV_PRE_SEND_STATE       0
#define IR_DRV_SEND_FINISH_STATE    1
#define IR_DRV_PRE_RECV_STATE       2
#define IR_DRV_RECV_FINISH_STATE    3
#define IR_DRV_SEND_HW_RESET        4
#define IR_DRV_RECV_HW_INIT         5
#define IR_DRV_RECV_HW_DEINIT       6

/***********************************************************
***********************typedef define***********************
***********************************************************/

typedef void *IR_DRV_HANDLE_T;

typedef int (*IR_DRV_OUTPUT_FINISH_CB)(void *args);
typedef int (*IR_DRV_RECV_CB)(IR_DRV_HANDLE_T drv_hdl, unsigned int raw_data, void *args);

/* tdl processing callbacks */
typedef struct {
    IR_DRV_OUTPUT_FINISH_CB output_finish_cb;
    IR_DRV_RECV_CB          recv_cb;
}IR_TDL_TRANS_CB;

typedef struct {
    int (*open)(IR_DRV_HANDLE_T drv_hdl, unsigned char mode, IR_TDL_TRANS_CB ir_tdl_cb, void *args);
    int (*close)(IR_DRV_HANDLE_T drv_hdl, unsigned char mode);
    int (*output)(IR_DRV_HANDLE_T drv_hdl, unsigned int freq, unsigned char is_active, unsigned int time_us);

    int (*status_notif)(IR_DRV_HANDLE_T drv_hdl, IR_DRIVER_STATE_E state, void *args);
} TDD_IR_INTFS_T;

/***********************************************************
********************function declaration********************
***********************************************************/

/**
 * @brief register ir device
 *
 * @param[in] dev_name: device name
 * @param[in] cfg: device config params
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_ir_dev_register(CHAR_T *dev_name, IR_DRV_HANDLE_T drv_hdl, TDD_IR_INTFS_T *ir_intfs);

#ifdef __cplusplus
}
#endif

#endif /* __TDL_IR_DRIVER_H__ */
