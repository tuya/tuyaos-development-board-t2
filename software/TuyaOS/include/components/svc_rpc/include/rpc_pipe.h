/**
* @file rpc_pipe.h
* @brief rcp pipe
* @version 0.1
* @date 2021-08-01
*
* @copyright Copyright 2015-2021 Tuya Inc. All Rights Reserved.
*
*/
#ifndef __RPC_PIPE_H__
#define __RPC_PIPE_H__

#include "svc_rpc.h"
#include "rpc_node.h"

typedef void (*tuya_rpc_pipe_reply_cb)(tuya_rpc_t *rpc, rpc_node_t *node, uint8_t *data, uint16_t data_len, void *args);
typedef void (*tuya_rpc_pipe_recv_cb)(tuya_rpc_t *rpc, rpc_node_t *node, uint8_t *data, uint16_t data_len, uint16_t sequence);

//! call is a combination of send and reply, e.g A send B, B reply A
//! if reply_cb is null, then call means send without reply
typedef struct {
    uint8_t                 *data;
    uint16_t                 data_len;
    tuya_rpc_pipe_reply_cb   reply_cb;
    void                    *args;
    uint32_t                 sec;
} tuya_rpc_pipe_call_t;

int tuya_rpc_pipe_call_async(tuya_rpc_t *rpc, char *id, tuya_rpc_pipe_call_t *pipe);
int tuya_rpc_pipe_reply(tuya_rpc_t *rpc, rpc_node_t *node, uint8_t *data, uint16_t data_len, uint16_t sequence);
int tuya_rpc_pipe_recv_register(tuya_rpc_t *rpc, tuya_rpc_pipe_recv_cb pipe_recv_cb, void *args);


#endif
