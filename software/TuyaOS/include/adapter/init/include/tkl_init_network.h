/**
* @file tkl_init_network.h
* @brief Common process - tkl init network description
* @version 0.1
* @date 2021-08-06
*
* @copyright Copyright 2021-2030 Tuya Inc. All Rights Reserved.
*
*/
#ifndef __TKL_INIT_NETWORK_H__
#define __TKL_INIT_NETWORK_H__

#include "tkl_network.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * @brief the description of tuya kernel adapter layer network
 *
 */
typedef struct {
    TUYA_ERRNO            (*get_errno)          (VOID);
    OPERATE_RET           (*fd_set1)            (CONST INT_T fd, TUYA_FD_SET_T* fds);
    OPERATE_RET           (*fd_clear)           (CONST INT_T fd, TUYA_FD_SET_T* fds);
    OPERATE_RET           (*fd_isset)           (CONST INT_T fd, TUYA_FD_SET_T* fds);
    OPERATE_RET           (*fd_zero)            (TUYA_FD_SET_T* fds);
    INT_T                 (*select)             (CONST INT_T maxfd, TUYA_FD_SET_T *readfds, TUYA_FD_SET_T *writefds, TUYA_FD_SET_T *errorfds, CONST UINT_T ms_timeout);
    TUYA_ERRNO            (*close)              (CONST INT_T fd);
    INT_T                 (*socket_create)      (CONST TUYA_PROTOCOL_TYPE_E type);
    TUYA_ERRNO            (*connect)            (CONST INT_T fd, CONST TUYA_IP_ADDR_T addr, CONST UINT16_T port);
    TUYA_ERRNO            (*connect_raw)        (CONST INT_T fd, VOID *p_socket, CONST INT_T len);
    TUYA_ERRNO            (*bind)               (CONST INT_T fd, CONST TUYA_IP_ADDR_T addr, CONST UINT16_T port);
    TUYA_ERRNO            (*listen)             (CONST INT_T fd, CONST INT_T backlog);
    TUYA_ERRNO            (*send)               (CONST INT_T fd, CONST VOID *buf, CONST UINT_T nbytes);
    TUYA_ERRNO            (*send_to)            (CONST INT_T fd, CONST VOID *buf, CONST UINT_T nbytes, CONST TUYA_IP_ADDR_T addr, CONST UINT16_T port);
    TUYA_ERRNO            (*recv)               (CONST INT_T fd, VOID *buf, CONST UINT_T nbytes);
    TUYA_ERRNO            (*recvfrom)           (CONST INT_T fd, VOID *buf, CONST UINT_T nbytes, TUYA_IP_ADDR_T *addr, UINT16_T *port);
    TUYA_ERRNO            (*accept)             (CONST INT_T fd, TUYA_IP_ADDR_T *addr, UINT16_T *port);
    INT_T                 (*recv_nd_size)       (CONST INT_T fd, VOID *buf, CONST UINT_T buf_size, CONST UINT_T nd_size);
    OPERATE_RET           (*socket_bind)        (CONST INT_T fd, CONST CHAR_T *ip);
    OPERATE_RET           (*set_block)          (CONST INT_T fd, CONST BOOL_T block);
    OPERATE_RET           (*set_cloexec)        (CONST INT_T fd);
    OPERATE_RET           (*get_socket_ip)      (CONST INT_T fd, TUYA_IP_ADDR_T *addr);
    INT_T                 (*get_nonblock)       (CONST INT_T fd);
    OPERATE_RET           (*gethostbyname)      (CONST CHAR_T *domain, TUYA_IP_ADDR_T *addr);
    TUYA_IP_ADDR_T        (*str2addr)           (CONST CHAR_T *ip_str);
    CHAR_T*               (*addr2str)           (CONST TUYA_IP_ADDR_T ipaddr);
    OPERATE_RET           (*setsockopt)         (CONST INT_T fd, CONST TUYA_OPT_LEVEL level, CONST TUYA_OPT_NAME optname, CONST VOID_T *optval, CONST INT_T optlen);
    OPERATE_RET           (*getsockopt)         (CONST INT_T fd, CONST TUYA_OPT_LEVEL level, CONST TUYA_OPT_NAME optname, VOID_T *optval, INT_T *optlen);
} TKL_NETWORK_DESC_T;

/**
 * @brief register wired description to tuya object manage
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
TKL_NETWORK_DESC_T* tkl_network_desc_get(VOID_T);


#ifdef __cplusplus
} // extern "C"
#endif

#endif // __TKL_INIT_WIRED_H__

