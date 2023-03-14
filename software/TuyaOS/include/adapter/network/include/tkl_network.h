/**
* @file tkl_wired.h
* @brief Common process - adapter the wired api
* @version 0.1
* @date 2020-11-09
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/

#ifndef __TKL_NETWORK_H__
#define __TKL_NETWORK_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief Get error code of network
*
* @param void
*
* @note This API is used for getting error code of network.
*
* @return 0 on success. Others on error, please refer to the error no of the target system
*/
TUYA_ERRNO tkl_net_get_errno(VOID);

/**
* @brief Add file descriptor to set
*
* @param[in] fd: file descriptor
* @param[in] fds: set of file descriptor
*
* @note This API is used to add file descriptor to set.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tkl_net_fd_set(CONST INT_T fd, TUYA_FD_SET_T* fds);

/**
* @brief Clear file descriptor from set
*
* @param[in] fd: file descriptor
* @param[in] fds: set of file descriptor
*
* @note This API is used to clear file descriptor from set.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tkl_net_fd_clear(CONST INT_T fd, TUYA_FD_SET_T* fds);

/**
* @brief Check file descriptor is in set
*
* @param[in] fd: file descriptor
* @param[in] fds: set of file descriptor
*
* @note This API is used to check the file descriptor is in set.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tkl_net_fd_isset(CONST INT_T fd, TUYA_FD_SET_T* fds);

/**
* @brief Clear all file descriptor in set
*
* @param[in] fds: set of file descriptor
*
* @note This API is used to clear all file descriptor in set.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tkl_net_fd_zero(TUYA_FD_SET_T* fds);

/**
* @brief Get available file descriptors
*
* @param[in] maxfd: max count of file descriptor
* @param[out] readfds: a set of readalbe file descriptor
* @param[out] writefds: a set of writable file descriptor
* @param[out] errorfds: a set of except file descriptor
* @param[in] ms_timeout: time out
*
* @note This API is used to get available file descriptors.
*
* @return the count of available file descriptors.
*/
 INT_T tkl_net_select(CONST INT_T maxfd, TUYA_FD_SET_T *readfds, TUYA_FD_SET_T *writefds, TUYA_FD_SET_T *errorfds, CONST UINT_T ms_timeout);

/**
* @brief Get no block file descriptors
*
* @param[in] fd: file descriptor
*
* @note This API is used to get no block file descriptors.
*
* @return the count of no block file descriptors.
*/
INT_T tkl_net_get_nonblock(CONST INT_T fd);

/**
* @brief Set block flag for file descriptors
*
* @param[in] fd: file descriptor
* @param[in] block: block flag
*
* @note This API is used to set block flag for file descriptors.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tkl_net_set_block(CONST INT_T fd, CONST BOOL_T block);

/**
* @brief Close file descriptors
*
* @param[in] fd: file descriptor
*
* @note This API is used to close file descriptors.
*
* @return 0 on success. Others on error, please refer to the error no of the target system
*/
TUYA_ERRNO tkl_net_close(CONST INT_T fd);

/**
* @brief Shutdown file descriptors
*
* @param[in] fd: file descriptor
* @param[in] how: shutdown type
*
* @note This API is used to shutdown file descriptors.
*
* @return 0 on success. Others on error, please refer to the error no of the target system
*/
TUYA_ERRNO tkl_net_shutdown(CONST INT_T fd, CONST INT_T how);

/**
* @brief Create a tcp/udp socket
*
* @param[in] type: protocol type, tcp or udp
*
* @note This API is used for creating a tcp/udp socket.
*
* @return file descriptor
*/
INT_T tkl_net_socket_create(CONST TUYA_PROTOCOL_TYPE_E type);

/**
* @brief Connect to network
*
* @param[in] fd: file descriptor
* @param[in] addr: address information of server
* @param[in] port: port information of server
*
* @note This API is used for connecting to network.
*
* @return 0 on success. Others on error, please refer to the error no of the target system
*/
TUYA_ERRNO tkl_net_connect(CONST INT_T fd, CONST TUYA_IP_ADDR_T addr, CONST UINT16_T port);

/**
* @brief Connect to network with raw data
*
* @param[in] fd: file descriptor
* @param[in] p_socket: raw socket data
* @param[in] len: data lenth
*
* @note This API is used for connecting to network with raw data.
*
* @return 0 on success. Others on error, please refer to the error no of the target system
*/
TUYA_ERRNO tkl_net_connect_raw(CONST INT_T fd, VOID *p_socket_addr, CONST INT_T len);

/**
* @brief Bind to network
*
* @param[in] fd: file descriptor
* @param[in] addr: address information of server
* @param[in] port: port information of server
*
* @note This API is used for binding to network.
*
* @return 0 on success. Others on error, please refer to the error no of the target system
*/
TUYA_ERRNO tkl_net_bind(CONST INT_T fd, CONST TUYA_IP_ADDR_T addr, CONST UINT16_T port);

/**
* @brief Listen to network
*
* @param[in] fd: file descriptor
* @param[in] backlog: max count of backlog connection
*
* @note This API is used for listening to network.
*
* @return 0 on success. Others on error, please refer to the error no of the target system
*/
TUYA_ERRNO tkl_net_listen(CONST INT_T fd, CONST INT_T backlog);

/**
* @brief Listen to network
*
* @param[in] fd: file descriptor
* @param[out] addr: the accept ip addr
* @param[out] port: the accept port number
*
* @note This API is used for listening to network.
*
* @return 0 on success. Others on error, please refer to the error no of the target system
*/
TUYA_ERRNO tkl_net_accept(CONST INT_T fd, TUYA_IP_ADDR_T *addr, UINT16_T *port);

/**
* @brief Send data to network
*
* @param[in] fd: file descriptor
* @param[in] buf: send data buffer
* @param[in] nbytes: buffer lenth
*
* @note This API is used for sending data to network
*
* @return 0 on success. Others on error, please refer to the error no of the target system
*/
TUYA_ERRNO tkl_net_send(CONST INT_T fd, CONST VOID *buf, CONST UINT_T nbytes);

/**
* @brief Send data to specified server
*
* @param[in] fd: file descriptor
* @param[in] buf: send data buffer
* @param[in] nbytes: buffer lenth
* @param[in] addr: address information of server
* @param[in] port: port information of server
*
* @note This API is used for sending data to network
*
* @return 0 on success. Others on error, please refer to the error no of the target system
*/
TUYA_ERRNO tkl_net_send_to(CONST INT_T fd, CONST VOID *buf, CONST UINT_T nbytes, CONST TUYA_IP_ADDR_T addr,CONST UINT16_T port);

/**
* @brief Receive data from network
*
* @param[in] fd: file descriptor
* @param[in] buf: receive data buffer
* @param[in] nbytes: buffer lenth
*
* @note This API is used for receiving data from network
*
* @return 0 on success. Others on error, please refer to the error no of the target system
*/
TUYA_ERRNO tkl_net_recv(CONST INT_T fd, VOID *buf, CONST UINT_T nbytes);

/**
* @brief Receive data from network with need size
*
* @param[in] fd: file descriptor
* @param[in] buf: receive data buffer
* @param[in] nbytes: buffer lenth
* @param[in] nd_size: the need size
*
* @note This API is used for receiving data from network with need size
*
* @return >0 on success. Others on error
*/
INT_T tkl_net_recv_nd_size(CONST INT_T fd, VOID *buf, CONST UINT_T buf_size, CONST UINT_T nd_size);

/**
* @brief Receive data from specified server
*
* @param[in] fd: file descriptor
* @param[in] buf: receive data buffer
* @param[in] nbytes: buffer lenth
* @param[in] addr: address information of server
* @param[in] port: port information of server
*
* @note This API is used for receiving data from specified server
*
* @return 0 on success. Others on error, please refer to the error no of the target system
*/
TUYA_ERRNO tkl_net_recvfrom(CONST INT_T fd, VOID *buf, CONST UINT_T nbytes, TUYA_IP_ADDR_T *addr, UINT16_T *port); 
 
/**
* @brief Get address information by domain
*
* @param[in] domain: domain information
* @param[in] addr: address information
*
* @note This API is used for getting address information by domain.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tkl_net_gethostbyname(CONST CHAR_T *domain, TUYA_IP_ADDR_T *addr);

/**
* @brief Bind to network with specified ip
*
* @param[in] fd: file descriptor
* @param[in] ip: ip address
*
* @note This API is used for binding to network with specified ip.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tkl_net_socket_bind(CONST INT_T fd, CONST CHAR_T *ip);

/**
* @brief Set socket fd close mode
*
* @param[in] fd: file descriptor
*
* @note This API is used for setting socket fd close mode, the socket fd will not be closed in child processes generated by fork calls.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tkl_net_set_cloexec(CONST INT_T fd);
 
/**
* @brief Get ip address by socket fd
*
* @param[in] fd: file descriptor
* @param[out] addr: ip address
*
* @note This API is used for getting ip address by socket fd.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tkl_net_get_socket_ip(CONST INT_T fd, TUYA_IP_ADDR_T *addr);

/**
* @brief Change ip string to address
*
* @param[in] ip_str: ip string
*
* @note This API is used to change ip string to address.
*
* @return ip address
*/
TUYA_IP_ADDR_T tkl_net_str2addr(CONST CHAR_T *ip_str);

/**
* @brief Change ip address to string
*
* @param[in] ipaddr: ip address
*
* @note This API is used to change ip address(in host byte order) to string(in IPv4 numbers-and-dots(xx.xx.xx.xx) notion).
*
* @return ip string
*/
CHAR_T* tkl_net_addr2str(CONST TUYA_IP_ADDR_T ipaddr);

/**
* @brief Set socket options
*
* @param[in] fd: file descriptor
* @param[in] level: setting level
* @param[in] optname: the name of the option
* @param[in] optval: the value of option
* @param[in] optlen: the length of the option value
*
* @note This API is used for setting socket options.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tkl_net_setsockopt(CONST INT_T fd, CONST TUYA_OPT_LEVEL level, CONST TUYA_OPT_NAME optname, CONST VOID_T *optval, CONST INT_T optlen);

/**
* @brief Get socket options
*
* @param[in] fd: file descriptor
* @param[in] level: getting level
* @param[in] optname: the name of the option
* @param[out] optval: the value of option
* @param[out] optlen: the length of the option value
*
* @note This API is used for getting socket options.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tkl_net_getsockopt(CONST INT_T fd, CONST TUYA_OPT_LEVEL level, CONST TUYA_OPT_NAME optname, VOID_T *optval, INT_T *optlen);

#ifdef __cplusplus
}
#endif

#endif // __TAL_NETWORK_H__

