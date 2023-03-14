/**
* @file svc_node.h
* @brief rcp node relate
* @version 0.1
* @date 2015-05-27
*
* @copyright Copyright 2015-2021 Tuya Inc. All Rights Reserved.
*
*/

#ifndef __RPC_NODE_H__
#define __RPC_NODE_H__

#include "svc_rpc.h"
#include "tuya_os_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t        ip;
    uint16_t        port;
    USHORT_T        cap;
    INT_T           seqno;
    TIME_S          timeout;
    char            id [TUYA_RPC_ID_LEN  + 1];
    uint8_t         key[TUYA_RPC_KEY_LEN + 1];
    SLIST_HEAD      next;
} rpc_node_t;

/**
* @brief create rpc node
*
* @param[in] id node id
* @param[in] addr address + port
* @param[in] key node key
*
* @note This API is used to create rpc node
*
* @return node adress
*/
rpc_node_t *rpc_node_create(char *id, char *addr, uint8_t key[16]);

/**
* @brief add rpc node
*
* @param[in] rpc rpc handle
* @param[in] node node need to add
*
* @note This API is used to create rpc node
*
* @return OPRT_OK when success. Others please refer to tuya_error_code.h
*/
int rpc_node_add(tuya_rpc_t *rpc, rpc_node_t *node);

/**
* @brief delete rpc node
*
* @param[in] rpc rpc handle
* @param[in] node node need to del
*
* @note This API is used to delete rpc node
*
* @return OPRT_OK when success. Others please refer to tuya_error_code.h
*/
int rpc_node_del(tuya_rpc_t *rpc, rpc_node_t *node);

/**
* @brief free rpc node
*
* @param[in] node node need to free
*
* @note This API is used to delete rpc node
*
*/
void rpc_node_free(rpc_node_t *node);

/**
* @brief found node by id
*
* @param[in] rpc rpc handler
* @param[in] id id
*
* @note This API is used to found one node
*
* @return node adress
*
*/
rpc_node_t *rpc_node_find_with_id(tuya_rpc_t *rpc, char *id);

/**
* @brief found node by address
*
* @param[in] rpc rpc handler
* @param[in] addr addr
*
* @note This API is used to found one node
*
* @return node adress
*
*/
rpc_node_t *rpc_node_find_with_addr(tuya_rpc_t *rpc, char *addr);

/**
* @brief found node by ip and port
*
* @param[in] rpc rpc handler
* @param[in] ip addr
* @param[in] port port
*
* @note This API is used to found one node
*
* @return node adress
*
*/
rpc_node_t *rpc_node_find_with_ip_port(tuya_rpc_t *rpc, uint32_t ip, uint16_t port);

/**
* @brief set node key
*
* @param[in] rpc rpc handler
* @param[in] id id
* @param[in] key key
*
* @note This API is used to set node key
*
* @return OPRT_OK when success. Others please refer to tuya_error_code.h
*
*/
int rpc_node_key_set_with_id(tuya_rpc_t *rpc, char *id, uint8_t key[16]);

/**
* @brief rpc node call back
*
* @param[in] rpc rpc handler
* @param[in] node id
* @param[in] args arguments
*
*/
typedef void rpc_node_cb_t(tuya_rpc_t *rpc, rpc_node_t *node, void *args);

/**
* @brief rpc node foreach
*
* @param[in] rpc rpc handler
* @param[in] node_cb callback
* @param[in] args arguments
*
*/
int rpc_node_foreach(tuya_rpc_t *rpc, rpc_node_cb_t *node_cb, void *args);

#ifdef __cplusplus
}
#endif

#endif
