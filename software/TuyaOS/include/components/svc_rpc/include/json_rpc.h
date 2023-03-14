/**
* @file lan_rpc.h
* @brief Common process - json rpc
* @version 0.1
* @date 2020-03-16
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/

#ifndef __JSON_RPC_H__
#define __JSON_RPC_H__


#include <stdint.h>
#include "tuya_cloud_types.h"
#include "cJSON.h"

#ifdef __cplusplus
extern "C" {
#endif


#define JRPC_PASER_ERROR            -32700
#define JPRC_INVALID_REQUEST        -32600
#define JPRC_METHOD_NOT_FOUND       -32601


typedef enum {
    JRPC_REQUEST        = 0,
    JRPC_RESPONSE,
    JRPC_NOTIFICATION,
    JRPC_ERROR,
    JRPC_WRONG_OBJECT
} jrpc_cmd_t;

typedef enum {
    JRPC_PARAMS         = 0,
    JRPC_RESULT
} jrpc_type_t;

typedef struct {
    cJSON           *json;
    uint8_t         *binary;
    jrpc_cmd_t       cmd;
} jrpc_msg_t;

typedef jrpc_msg_t JRPC_MSG_S;

/**
 * @brief create request
 *
 * @param[in] msg refer to JRPC_MSG_S
 * @param[in] method method
 * @param[in] id node id
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
int jrpc_create_request(jrpc_msg_t *msg, char *method, int id);
/**
 * @brief create response
 *
 * @param[in] msg refer to JRPC_MSG_S
 * @param[in] id node id
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
int jrpc_create_response(jrpc_msg_t *msg, int id);
/**
 * @brief create error
 *
 * @param[in] msg refer to JRPC_MSG_S
 * @param[in] id node id
 * @param[in] code json int
 * @param[in] message json message
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
int jrpc_create_error(jrpc_msg_t *msg, int id, int code, char *message);
/**
 * @brief create notification
 *
 * @param[in] msg refer to JRPC_MSG_S
 * @param[in] method refer to JRPC_NODE_TYPE_E
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
int jrpc_create_notification(jrpc_msg_t *msg, char *method);

/**
 * @brief write int
 *
 * @param[in] msg refer to JRPC_MSG_S
 * @param[in] type refer to JRPC_NODE_TYPE_E
 * @param[in] key json key
 * @param[in] value json value
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
int jrpc_write_int(jrpc_msg_t *msg, jrpc_type_t type, char *key, int value);
/**
 * @brief write string
 *
 * @param[in] msg refer to JRPC_MSG_S
 * @param[in] type refer to JRPC_NODE_TYPE_E
 * @param[in] key json key
 * @param[in] value json value
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
int jrpc_write_string(jrpc_msg_t *msg, jrpc_type_t type, char *key, char *value);

/**
 * @brief read int
 *
 * @param[in] msg refer to JRPC_MSG_S
 * @param[in] type refer to JRPC_NODE_TYPE_E
 * @param[in] key json key
 * @param[out] value json value
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
int jrpc_read_int(jrpc_msg_t *msg, jrpc_type_t type, char *key, int *value);
/**
 * @brief read string
 *
 * @param[in] msg refer to JRPC_MSG_S
 * @param[in] type refer to JRPC_NODE_TYPE_E
 * @param[in] key json key
 * @param[out] value json value
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
int jrpc_read_string(jrpc_msg_t *msg, jrpc_type_t type, char *key, char **value);
/**
 * @brief read method
 *
 * @param[in] msg refer to JRPC_MSG_S
 * @param[out] method value
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
int jrpc_read_method(jrpc_msg_t *msg, char **method);
/**
 * @brief read error
 *
 * @param[in] msg refer to JRPC_MSG_S
 * @param[out] code error code
 * @param[out] message error message
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
int jrpc_read_error(jrpc_msg_t *msg, int *code, char **message);
/**
 * @brief binary write
 *
 * @param[in] msg refer to LAN_RPC_NODE_OUTPUT_CB
 * @param[in] type refer to JRPC_NODE_TYPE_E
 * @param[in] key json key
 * @param[in] binary write value
 * @param[in] size write size
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
int jrpc_write_binary(jrpc_msg_t *msg, jrpc_type_t type, char *key, uint8_t *binary, uint16_t size);
/**
 * @brief binary read
 *
 * @param[in] msg refer to LAN_RPC_NODE_OUTPUT_CB
 * @param[in] type refer to JRPC_NODE_TYPE_E
 * @param[in] key json key
 * @param[out] binary read value
 * @param[out] size read size
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
int jrpc_read_binary(jrpc_msg_t *msg, jrpc_type_t type, char *key, uint8_t **binary, uint16_t *size);

/**
 * @brief json rpc delete
 *
 * @param[in] msg refer to JRPC_MSG_S
 *
 */
void jrpc_delete(jrpc_msg_t *msg);

#ifdef __cplusplus
}
#endif

#endif