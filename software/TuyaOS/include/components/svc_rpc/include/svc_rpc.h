/**
* @file svc_rpc.h
* @brief rcp relate
* @version 0.1
* @date 2015-05-27
*
* @copyright Copyright 2015-2021 Tuya Inc. All Rights Reserved.
*
*/
#ifndef __SVC_RPC_H__
#define __SVC_RPC_H__

#include <stdint.h>
#include "tuya_cloud_com_defs.h"
#include "tuya_os_adapter.h"
#include "uni_slist.h"
#include "rpc_base.h"
#include "json_rpc.h"
#include "tal_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OPRT_RPC_CALL_TIMEOUT       -10
#define OPRT_RPC_CALL_NOT_FOUND     -11
#define OPRT_RPC_CALL_ERROR         -11
#define TUYA_RPC_DEFAULT_BUFSZ      256
#define TUYA_RPC_ID_LEN             (GW_ID_LEN)
#define TUYA_RPC_KEY_LEN            (LOCAL_KEY_LEN)

typedef enum {
    TUYA_RPC_ENCRYPT,
    TUYA_RPC_DECRYPT,
    TUYA_RPC_FREECRYPT,
} tuya_rpc_crypt_mode_t;

typedef enum {
    RPC_BUFSIZE_SET_CMD,
} tuya_rpc_cmd_t;

typedef int (*rpc_crypt_cb)(uint8_t **data, uint32_t *len, uint8_t key[16], tuya_rpc_crypt_mode_t mode);

typedef struct {
    rpc_base_t      base;
    int             fd;
    uint32_t        flag;
    MUTEX_HANDLE    mutex;
    MUTEX_HANDLE    node_mutex;
    THREAD_HANDLE   thread;
    SLIST_HEAD      node_list;
    SLIST_HEAD      declare_list;
    SLIST_HEAD      publish_list;
    SLIST_HEAD      subscribe_list;
    rpc_crypt_cb    crypt_cb;
    char            id [TUYA_RPC_ID_LEN + 1];
    uint8_t         key[16];
    uint16_t        buffer_size;
    uint8_t        *buffer;
    uint8_t         default_buffer[TUYA_RPC_DEFAULT_BUFSZ];
    void           *pipe_recv_cb;
    void           *pipe_args;
} tuya_rpc_t;

/**
* @brief rpc method callback
*
* @param[in] request msg
* @param[in] arg request argument
* @param[in] reply reply
*
*/
typedef void (*rpc_method_cb)(jrpc_msg_t *request, void *arg, jrpc_msg_t *reply);

/**
* @brief rpc subscribe callback
*
* @param[in] request msg
* @param[in] arg request argument
*
*/
typedef void (*rpc_subscribe_cb)(jrpc_msg_t *request, void *arg);

/**
* @brief rpc async callback
*
* @param[in] result msg
* @param[in] arg request argument
*
*/
typedef void (*rpc_async_cb)(jrpc_msg_t *result, void *arg);

/**
* @brief rpc function init
*
* @param[in] rpc rpc handle
* @param[in] id rpc id
* @param[in] addr remote ip address
*
* @note This API is used to init rpc function
*
* @return OPRT_OK when success. Others please refer to tuya_error_code.h
*/
int tuya_rpc_init(tuya_rpc_t *rpc, char *id, char *addr);

/**
* @brief rpc control
*
* @param[in] rpc rpc handle
* @param[in] cmd control command
* @param[in] arg control parameter
*
* @note This API is used to control rpc function
*
* @return OPRT_OK when success. Others please refer to tuya_error_code.h
*/
int tuya_rpc_control(tuya_rpc_t *rpc, uint8_t cmd, void *arg);

/**
* @brief create rpc task
*
* @param[in] rpc rpc handle
* @param[in] stack_size task stack size
* @param[in] prio task prio
*
* @note This API is used to create rpc task
*
* @return OPRT_OK when success. Others please refer to tuya_error_code.h
*/
int tuya_rpc_run(tuya_rpc_t *rpc, uint32_t stack_size, uint32_t prio);

/**
* @brief rpc asynchronous call
*
* @param[in] rpc rpc handle
* @param[in] id rpc id
* @param[in] requset request message
* @param[in] cb asynchronous call back
* @param[in] arg call parameter
* @param[in] sec timeout seconds
*
* @note This API is used to rpc asynchronous call
*
* @return OPRT_OK when success. Others please refer to tuya_error_code.h
*/
int tuya_rpc_call_async(tuya_rpc_t      *rpc,  char *id, jrpc_msg_t *requset, rpc_async_cb cb, void *arg, uint32_t sec);

/**
* @brief rpc synchronous call
*
* @param[in] rpc rpc handle
* @param[in] id rpc id
* @param[in] requset request message
* @param[in] reply asynchronous reply message
* @param[in] sec timeout seconds
*
* @note This API is used to rpc synchronous call
*
* @return OPRT_OK when success. Others please refer to tuya_error_code.h
*/
int tuya_rpc_call_sync(tuya_rpc_t       *rpc, char *id, jrpc_msg_t *requset, jrpc_msg_t *reply, uint32_t sec);

/**
* @brief rpc call notify
*
* @param[in] rpc rpc handle
* @param[in] id rpc id
* @param[in] requset request message
*
* @note This API is used to rpc call notify
*
* @return OPRT_OK when success. Others please refer to tuya_error_code.h
*/
int tuya_rpc_call_notify(tuya_rpc_t *rpc, char *id, jrpc_msg_t *requset);

/**
* @brief declare one rpc according name
*
* @param[in] rpc rpc handle
* @param[in] name declare name
* @param[in] cb call back
* @param[in] arg call back parameter
*
* @note This API is used to declare one rpc
*
* @return OPRT_OK when success. Others please refer to tuya_error_code.h
*/
int tuya_rpc_declare(tuya_rpc_t *rpc, char *name, rpc_method_cb cb, void *arg);

/**
* @brief undeclare one rpc according name
*
* @param[in] rpc rpc handle
* @param[in] name undeclare name
*
* @note This API is used to undeclare one rpc
*
* @return OPRT_OK when success. Others please refer to tuya_error_code.h
*/
int tuya_rpc_undeclare(tuya_rpc_t *rpc, char *name);

/**
* @brief publish one rpc
*
* @param[in] rpc rpc handle
* @param[in] name the key name want to publish
* @param[in] msg msg
*
* @note This API is used to publish one rpc
*
* @return OPRT_OK when success. Others please refer to tuya_error_code.h
*/
int tuya_rpc_publish(tuya_rpc_t  *rpc, char *name, jrpc_msg_t *msg);

/**
* @brief subscribe one rpc
*
* @param[in] rpc rpc handle
* @param[in] id rpc id
* @param[in] name the key name want to subscribe
* @param[in] cb callback
* @param[in] arg callback paremater
*
* @note This API is used to subscribe one rpc
*
* @return OPRT_OK when success. Others please refer to tuya_error_code.h
*/
int tuya_rpc_subscribe(tuya_rpc_t *rpc, char *id, char *name, rpc_subscribe_cb cb, void *arg);

/**
* @brief unsubscribe one rpc
*
* @param[in] rpc rpc handle
* @param[in] id rpc id
* @param[in] name the key name want to subscribe
*
* @note This API is used to unsubscribe one rpc
*
* @return OPRT_OK when success. Others please refer to tuya_error_code.h
*/
int tuya_rpc_unsubscribe(tuya_rpc_t *rpc, char *id, char *name);

/**
* @brief set encryption key and callback
*
* @param[in] rpc rpc handle
* @param[in] crypt_cb encryption callback function
* @param[in] key encryption key
*
* @note This API is used to set encryption key and callback
*
* @return OPRT_OK when success. Others please refer to tuya_error_code.h
*/
int tuya_rpc_set_crypt(tuya_rpc_t *rpc, rpc_crypt_cb  crypt_cb, uint8_t key[TUYA_RPC_KEY_LEN]);

/**
* @brief set encryption key
*
* @param[in] rpc rpc handle
* @param[in] key encryption key
*
* @note This API is used to set encryption key
*
* @return OPRT_OK when success. Others please refer to tuya_error_code.h
*/
int tuya_rpc_set_crypt_key(tuya_rpc_t *rpc, uint8_t key[TUYA_RPC_KEY_LEN]);

/**
* @brief set encryption callback
*
* @param[in] rpc rpc handle
* @param[in] crypt_cb callback
*
* @note This API is used to set encryption callback
*
* @return OPRT_OK when success. Others please refer to tuya_error_code.h
*/
int tuya_rpc_set_crypt_cb(tuya_rpc_t *rpc,  rpc_crypt_cb        crypt_cb);


#ifdef __cplusplus
}
#endif

#endif
