/**
 * @file tdl_ir_dev_manage.h
 * @author www.tuya.com
 * @brief tdl_ir_dev_manage module is used to 
 * @version 0.1
 * @date 2022-06-06
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#ifndef __TDL_IR_DEV_MANAGE_H__
#define __TDL_IR_DEV_MANAGE_H__

#include "tuya_cloud_types.h"
#include "tdl_ir_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************macro define************************
***********************************************************/

#define IR_DEV_NAME_MAX_LEN (16u)

/* ir protocol */
typedef UINT8_T IR_PROT_E;
#define IR_PROT_TIMECODE    0
#define IR_PROT_NEC         1
#define IR_PROT_MAX         2

typedef unsigned char IR_SEND_STATUS;
#define IR_STA_SEND_IDLE        0
#define IR_STA_SEND_BUILD       1
#define IR_STA_SENDING          2
#define IR_STA_SEND_FINISH      3
#define IR_STA_SEND_MAX         4

typedef unsigned char IR_RECV_STATUS;
#define IR_STA_RECV_IDLE        0
#define IR_STA_RECVING          1
#define IR_STA_RECV_FINISH      2
#define IR_STA_RECV_PARSER      3
#define IR_STA_RECV_OVERFLOW    4
#define IR_STA_RECV_MAX         5

/* tdl ir config command */
typedef unsigned char IR_CMD_E;
#define IR_CMD_GET_SEND_STATUS  1
#define IR_CMD_GET_RECV_STATUS  2
#define IR_CMD_SEND_HW_RESET    3
#define IR_CMD_RECV_HW_INIT     4
#define IR_CMD_RECV_HW_DEINIT   5
#define IR_CMD_RECV_TASK_START  6
#define IR_CMD_RECV_TASK_STOP   7
#define IR_CMD_RECV_QUEUE_CLEAN 8
#define IR_CMD_RECV_CB_REGISTER 9

/***********************************************************
***********************typedef define***********************
***********************************************************/

typedef void *IR_HANDLE_T;

/* nec protocol config struct */
typedef struct {
    UINT8_T is_nec_msb; // 1: msb, 0: lsb

    // percent value range: 0-100
    UINT8_T lead_err;
    UINT8_T logics_err; // logic code high level error percent
    UINT8_T logic0_err;
    UINT8_T logic1_err;
    UINT8_T repeat_err;
} IR_NEC_CFG_T;

/* ir protocol config union */
typedef union {
    IR_NEC_CFG_T nec_cfg;
} IR_PROT_CFG_U;

/* ir nec protocol data struct */
#pragma pack(1)
typedef struct {
    UINT16_T addr;
    UINT16_T cmd;
    UINT16_T repeat_cnt;
} IR_DATA_NEC_T;
#pragma pack()

/* ir timecode data struct */
typedef struct {
    UINT16_T len;
    UINT32_T *data;
} IR_DATA_TIMECODE_T;

/* ir data union */
typedef union {
    IR_DATA_NEC_T nec_data;
    IR_DATA_TIMECODE_T timecode;
} IR_DATA_U;

/* ir device config struct */
typedef struct {
    IR_MODE_E ir_mode;

    UINT8_T recv_queue_num;
    UINT16_T recv_buf_size; // recv data size
    UINT16_T recv_timeout; //unit :ms

    IR_PROT_E prot_opt;
    IR_PROT_CFG_U prot_cfg;
} IR_DEV_CFG_T;

typedef void (*IR_APP_RECV_CB)(UINT8_T is_frame_finish, IR_DATA_U *recv_data);

/***********************************************************
********************function declaration********************
***********************************************************/

/**
 * @brief find ir device
 *
 * @param[in] dev_name: device name
 * @param[out] handle: ir device handle
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_ir_dev_find(CHAR_T *dev_name, IR_HANDLE_T *handle);

/**
 * @brief open ir device
 *
 * @param[in] handle: ir device handle
 * @param[in] config: ir device config
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_ir_dev_open(IR_HANDLE_T handle, IR_DEV_CFG_T *config);

/**
 * @brief close ir device
 *
 * @param[in] handle: ir device handle
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_ir_dev_close(IR_HANDLE_T handle);

/**
 * @brief ir data send
 *
 * @param[in] handle: ir device handle
 * @param[in] ir_data: send data
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_ir_dev_send(IR_HANDLE_T handle, UINT32_T freq, IR_DATA_U ir_data, UINT8_T send_cnt);

/**
 * @brief ir data recv
 *
 * @param[in] handle: ir device handle
 * @param[in] ir_data: recv data
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_ir_dev_recv(IR_HANDLE_T handle, IR_DATA_U **ir_data, UINT32_T timeout_ms);

/**
 * @brief release recv data
 *
 * @param[in] handle: ir device handle
 * @param[in] ir_data: recv data
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_ir_dev_recv_release(IR_HANDLE_T handle, IR_DATA_U *ir_data);

/**
 * @brief config ir device
 *
 * @param[in] handle: ir device handle
 * @param[in] cmd: ir device command
 * @param[inout] params: config params or output data
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_ir_config(IR_HANDLE_T handle, IR_CMD_E cmd, void *params);


#ifdef __cplusplus
}
#endif

#endif /* __TDL_IR_DEV_MANAGE_H__ */
