/**
* @file rpc_base.h
* @brief Common process - base rpc
* @version 0.1
* @date 2018-09-01
*
* @copyright Copyright 2018-2021 Tuya Inc. All Rights Reserved.
*
*/

#ifndef __RPC_BASE_H__
#define __RPC_BASE_H__

/*============================ INCLUDES ======================================*/
#include "tuya_os_adapter.h"
#include "uni_slist.h"
#include "tal_workq_service.h"
#include "tal_sw_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/**
 * @brief rpc send callback
 *
 * @param[in] addr peer addr
 * @param[in] data send data
 * @param[in] len send length
 * @param[in] priv_data user private data
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
typedef int (*rpc_send_cb)(char *addr,  uint8_t *data, uint32_t data_len, void *priv_data);
/**
 * @brief rpc reply callback
 *
 * @param[in] addr peer addr, if NULL means timeout
 * @param[out] data received data
 * @param[out] data_len received length
 * @param[in] priv_data user private data
 * @param[in] fuck_key fast useful channel key
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
typedef int (*rpc_reply_cb)(char *addr, uint8_t *data, uint32_t data_len, void *priv_data, uint8_t *fuck_key);

/**
 * @brief rpc received callback
 *
 * @param[in] addr peer addr
 * @param[in] data received data
 * @param[in] data_len received length
 * @param[in] priv_data user private data
 * @param[in] sequence sequence
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
typedef int (*rpc_recv_cb)(char *addr,  uint8_t *data, uint32_t data_len, void *priv_data, uint16_t sequence);


typedef struct {
    rpc_send_cb         send_cb;
    rpc_recv_cb         request_cb;
    rpc_recv_cb         notify_cb;
    void               *priv_data;
} rpc_base_cfg_t;

typedef struct {
    void              *data;
    uint32_t           data_len;
    uint16_t           timeout;
    rpc_reply_cb       reply_cb;
    char              *addr;
    void              *priv_data;
    uint8_t           *fuck_key;
} rpc_msg_t;

typedef struct {
    MUTEX_HANDLE        mutex;
    uint16_t            sequence;
    rpc_send_cb         send_cb;
    rpc_recv_cb         request_cb;
    rpc_recv_cb         notify_cb;
    rpc_recv_cb         pipe_cb;
    TIMER_ID            async_timer;
    MUTEX_HANDLE        async_mutex;
    SLIST_HEAD          async_list;
    void               *priv_data;
} rpc_base_t;

/*============================ PROTOTYPES ====================================*/
/**
 * @brief rpc base init
 * @param[in] base handle
 * @param[in] cfg refer to rpc_base_cfg_t struct
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
int rpc_base_init(rpc_base_t *base, rpc_base_cfg_t *cfg);

/**
 * @brief rpc base request
 * @param[in] base handle
 * @param[in] msg refer to rpc_msg_t struct
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
int rpc_base_request(rpc_base_t *base, rpc_msg_t *msg);

/**
 * @brief rpc base notify
 * @param[in] base handle
 * @param[in] cfg refer to rpc_base_cfg_t struct
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
int rpc_base_notify(rpc_base_t  *base, rpc_msg_t *msg);

/**
 * @brief rpc base reply
 * @param[in] base handle
 * @param[in] cfg refer to rpc_base_cfg_t struct
 * @param[in] sequence sequence
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
int rpc_base_reply(rpc_base_t *base, rpc_msg_t *msg, uint16_t sequence);


/**
 * @brief rpc base reply
 * @param[in] base handle
 * @param[in] addr peer addr
 * @param[in] frame received frame data
 * @param[in] frame_len received frame len
 * @param[in] fuck_key fast useful channel key
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
int rpc_base_frame_process(rpc_base_t     *base,
                           char           *addr,
                           uint8_t        *frame,
                           uint32_t        frame_len,
                           uint8_t        *fuck_key);
#ifdef __cplusplus
}
#endif

#endif
