/**
* @file uni_msg_queue.h
* @author nzy@tuya.com
* @brief Common process - Initialization
* @version 0.1
* @date 2020-11-09
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/
#ifndef _UNI_MSG_QUEUE_H
#define _UNI_MSG_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"
#include "sys_timer.h"

typedef PVOID_T MSG_QUE_HANDLE; // message queue handle
typedef USHORT_T MSG_ID;          // message id
typedef PVOID_T P_MSG_DATA;       // message data
typedef UINT_T MSG_DATA_LEN;      // message data lenth

#define UNVALUED_MSGID 0xffff  // invalid message id

/**
 * @brief Definition of message type
 */
typedef UINT_T MSG_TYPE; 
#define INSTANCY_MESSAGE 0  // instant message type
#define NORMAL_MESSAGE 1    // normal message type

/**
 * @brief Init param of message
 */
typedef struct
{
    MSG_ID msgID;            // message id
    P_MSG_DATA pMsgData;     // message data
    MSG_DATA_LEN msgDataLen; // message data len
}MESSAGE,*P_MESSAGE;

// message processing callback
typedef VOID(*MSG_CALLBACK)(MESSAGE *msg);

struct s_tm_msg;
typedef VOID(* TM_MSG_CB)(struct s_tm_msg *tm_msg); // timer message callback

/**
 * @brief timer message definition
 */
typedef struct s_tm_msg {
    TIMER_ID timer;              // timer id
    MSG_QUE_HANDLE msgQueHandle; // timer queue handle
    TM_MSG_CB cb;                // timer callback function
    MSG_ID msgID;                // message id
    VOID *data;                  // message data
}TM_MSG_S;

/**
 * @brief message entry definition
 */
typedef struct {
    MSG_ID *mid;         // message id
    MSG_CALLBACK msg_cb; // message callback
}MSG_ENTRY_S;

/**
 * @brief timer message entry definition
 */
typedef struct {
    TM_MSG_S **tm_msg;    //timer message info
    TM_MSG_CB tmm_msg_cb; //timer message callback
}TM_MSG_ENTRY_S;
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
