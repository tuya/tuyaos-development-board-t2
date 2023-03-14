/**
* @file tuya_hal_network.h
* @brief Common process - Initialization
* @version 0.1
* @date 2020-11-09
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/
#ifndef __TUYA_HAL_NETWORK_H__
#define __TUYA_HAL_NETWORK_H__

#include "tal_network.h"
#include <errno.h>

#ifdef __cplusplus
    extern "C" {
#endif

/* tuya sdk definition of shutdown type */
#define UNW_SHUT_RD   0
#define UNW_SHUT_WR   1
#define UNW_SHUT_RDWR 2

typedef TUYA_IP_ADDR_T UNW_IP_ADDR_T;
typedef TUYA_TRANS_TYPE_E UNW_TRANS_TYPE_E;
typedef TUYA_PROTOCOL_TYPE_E UNW_PROTOCOL_TYPE;
typedef TUYA_FD_SET_T UNW_FD_SET_T;


/**
* @brief Get error code of network
*
* @param void
*
* @note This API is used for getting error code of network.
*
* @return UNW_SUCCESS on success. Others on error, please refer to tuya_os_adapter_error_code.h
*/
#define tuya_hal_net_get_errno() tal_net_get_errno()

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
#define tuya_hal_net_fd_set(fd, fds) tal_net_fd_set(fd, fds)

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
#define tuya_hal_net_fd_clear(fd, fds) tal_net_fd_clear(fd, fds)

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
#define tuya_hal_net_fd_isset(fd, fds) tal_net_fd_isset(fd, fds)

/**
* @brief Clear all file descriptor in set
*
* @param[in] fds: set of file descriptor
*
* @note This API is used to clear all file descriptor in set.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
#define tuya_hal_net_fd_zero(fds) tal_net_fd_zero(fds)

//Add file descriptor to set
#define UNW_FD_SET(n,p)     TAL_FD_SET(n, p)
//Clear file descriptor from set
#define UNW_FD_CLR(n, p)    TAL_FD_CLR(n, p)
//Check file descriptor is in set
#define UNW_FD_ISSET(n,p)   TAL_FD_ISSET(n,p)
//Clear all descriptor in set
#define UNW_FD_ZERO(p)      TAL_FD_ZERO(p)

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
#define tuya_hal_net_select(maxfd, readfds, writefds, errorfds, ms_timeout) \
    tal_net_select(maxfd, readfds, writefds, errorfds, ms_timeout)

/**
* @brief Get no block file descriptors
*
* @param[in] fd: file descriptor
*
* @note This API is used to get no block file descriptors.
*
* @return the count of no block file descriptors.
*/
#define tuya_hal_net_get_nonblock(fd) tal_net_get_nonblock(fd)

/**
* @brief Set block flag for file descriptors
*
* @param[in] fd: file descriptor
* @param[in] block: block flag
*
* @note This API is used to set block flag for file descriptors.
*
* @return UNW_SUCCESS on success. Others on error, please refer to tuya_os_adapter_error_code.h
*/
#define tuya_hal_net_set_block(fd, block) tal_net_set_block(fd, block)

/**
* @brief Close file descriptors
*
* @param[in] fd: file descriptor
*
* @note This API is used to close file descriptors.
*
* @return UNW_SUCCESS on success. Others on error, please refer to tuya_os_adapter_error_code.h
*/
#define tuya_hal_net_close(fd) tal_net_close(fd)

/**
* @brief Create a tcp/udp socket
*
* @param[in] type: protocol type, tcp or udp
*
* @note This API is used for creating a tcp/udp socket.
*
* @return file descriptor
*/
#define tuya_hal_net_socket_create(type) tal_net_socket_create(type)

/**
* @brief Connect to network
*
* @param[in] fd: file descriptor
* @param[in] addr: address information of server
* @param[in] port: port information of server
*
* @note This API is used for connecting to network.
*
* @return UNW_SUCCESS on success. Others on error, please refer to tuya_os_adapter_error_code.h
*/
#define tuya_hal_net_connect(fd, addr, port) tal_net_connect(fd, addr, port)

/**
* @brief Connect to network with raw data
*
* @param[in] fd: file descriptor
* @param[in] p_socket: raw socket data
* @param[in] len: data lenth
*
* @note This API is used for connecting to network with raw data.
*
* @return UNW_SUCCESS on success. Others on error, please refer to tuya_os_adapter_error_code.h
*/
#define tuya_hal_net_connect_raw(fd, p_socket, len) tal_net_connect_raw(fd, p_socket, len)

/**
* @brief Bind to network
*
* @param[in] fd: file descriptor
* @param[in] addr: address information of server
* @param[in] port: port information of server
*
* @note This API is used for binding to network.
*
* @return UNW_SUCCESS on success. Others on error, please refer to tuya_os_adapter_error_code.h
*/
#define tuya_hal_net_bind(fd, addr, port) tal_net_bind(fd, addr, port)

/**
* @brief Listen to network
*
* @param[in] fd: file descriptor
* @param[in] backlog: max count of backlog connection
*
* @note This API is used for listening to network.
*
* @return UNW_SUCCESS on success. Others on error, please refer to tuya_os_adapter_error_code.h
*/
#define tuya_hal_net_listen(fd, backlog) tal_net_listen(fd, backlog)

/**
* @brief Send data to network
*
* @param[in] fd: file descriptor
* @param[in] buf: send data buffer
* @param[in] nbytes: buffer lenth
*
* @note This API is used for sending data to network
*
* @return UNW_SUCCESS on success. Others on error, please refer to tuya_os_adapter_error_code.h
*/
#define tuya_hal_net_send(fd, buf, nbytes) tal_net_send(fd, buf, nbytes)

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
* @return UNW_SUCCESS on success. Others on error, please refer to tuya_os_adapter_error_code.h
*/
#define tuya_hal_net_send_to(fd, buf, nbytes, addr, port) \
    tal_net_send_to(fd, buf, nbytes, addr, port)

/**
* @brief Receive data from network
*
* @param[in] fd: file descriptor
* @param[in] buf: receive data buffer
* @param[in] nbytes: buffer lenth
*
* @note This API is used for receiving data from network
*
* @return UNW_SUCCESS on success. Others on error, please refer to tuya_os_adapter_error_code.h
*/
#define tuya_hal_net_recv(fd, buf, nbytes) tal_net_recv(fd, buf, nbytes)

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
* @return UNW_SUCCESS on success. Others on error, please refer to tuya_os_adapter_error_code.h
*/
#define tuya_hal_net_recvfrom(fd, buf, nbytes, addr, port) \
    tal_net_recvfrom(fd, buf, nbytes, addr, port)


/**
* @brief Set timeout option of socket fd
*
* @param[in] fd: file descriptor
* @param[in] ms_timeout: timeout in ms
* @param[in] type: transfer type, receive or send
*
* @note This API is used for setting timeout option of socket fd.
*
* @return UNW_SUCCESS on success. Others on error, please refer to tuya_os_adapter_error_code.h
*/
#define tuya_hal_net_set_timeout(fd, ms_timeout, type) tal_net_set_timeout(fd, ms_timeout, type)

/**
* @brief Set buffer_size option of socket fd
*
* @param[in] fd: file descriptor
* @param[in] buf_size: buffer size in byte
* @param[in] type: transfer type, receive or send
*
* @note This API is used for setting buffer_size option of socket fd.
*
* @return UNW_SUCCESS on success. Others on error, please refer to tuya_os_adapter_error_code.h
*/
#define tuya_hal_net_set_bufsize(fd, buf_size, type) tal_net_set_bufsize(fd, buf_size, type)

/**
* @brief Enable reuse option of socket fd
*
* @param[in] fd: file descriptor
*
* @note This API is used to enable reuse option of socket fd.
*
* @return UNW_SUCCESS on success. Others on error, please refer to tuya_os_adapter_error_code.h
*/
#define tuya_hal_net_set_reuse(fd) tal_net_set_reuse(fd)

/**
* @brief Disable nagle option of socket fd
*
* @param[in] fd: file descriptor
*
* @note This API is used to disable nagle option of socket fd.
*
* @return UNW_SUCCESS on success. Others on error, please refer to tuya_os_adapter_error_code.h
*/
#define tuya_hal_net_disable_nagle(fd) tal_net_disable_nagle(fd)

/**
* @brief Enable broadcast option of socket fd
*
* @param[in] fd: file descriptor
*
* @note This API is used to enable broadcast option of socket fd.
*
* @return UNW_SUCCESS on success. Others on error, please refer to tuya_os_adapter_error_code.h
*/
#define tuya_hal_net_set_boardcast(fd) tal_net_set_broadcast(fd)

/**
* @brief Set keepalive option of socket fd to monitor the connection
*
* @param[in] fd: file descriptor
* @param[in] alive: keepalive option, enable or disable option
* @param[in] idle: keep idle option, if the connection has no data exchange with the idle time(in seconds), start probe.
* @param[in] intr: keep interval option, the probe time interval.
* @param[in] cnt: keep count option, probe count.
*
* @note This API is used to set keepalive option of socket fd to monitor the connection.
*
* @return UNW_SUCCESS on success. Others on error, please refer to tuya_os_adapter_error_code.h
*/
#define tuya_hal_net_set_keepalive(fd, alive, idle, intr, cnt) tal_net_set_keepalive(fd, alive, idle, intr, cnt)

/**
* @brief Get address information by domain
*
* @param[in] domain: domain information
* @param[in] addr: address information
*
* @note This API is used for getting address information by domain.
*
* @return UNW_SUCCESS on success. Others on error, please refer to tuya_os_adapter_error_code.h
*/
#define tuya_hal_net_gethostbyname(domain, addr) tal_net_gethostbyname(domain, addr)

/**
* @brief Accept the coming socket connection of the server fd
*
* @param[in] fd: file descriptor
* @param[in] addr: address information of server
* @param[in] port: port information of server
*
* @note This API is used for accepting the coming socket connection of the server fd.
*
* @return UNW_SUCCESS on success. Others on error, please refer to tuya_os_adapter_error_code.h
*/
#define tuya_hal_net_accept(fd, addr, port) tal_net_accept(fd, addr, port)

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
* @return UNW_SUCCESS on success. Others on error, please refer to tuya_os_adapter_error_code.h
*/
#define tuya_hal_net_recv_nd_size(fd, buf, buf_size, nd_size) \
    tal_net_recv_nd_size(fd, buf, buf_size, nd_size)

/**
* @brief Change ip string to address
*
* @param[in] ip_str: ip string
*
* @note This API is used to change ip string to address.
*
* @return ip address
*/
#define tuya_hal_net_str2addr(ip_str) tal_net_str2addr(ip_str)

/**
* @brief Change ip address to string
*
* @param[in] ipaddr: ip address
*
* @note This API is used to change ip address(in host byte order) to string(in IPv4 numbers-and-dots(xx.xx.xx.xx) notion).
*
* @return ip string
*/
#define tuya_hal_net_addr2str(ipaddr) tal_net_addr2str(ipaddr)

/**
* @brief Bind to network with specified ip
*
* @param[in] fd: file descriptor
* @param[in] ip: ip address
*
* @note This API is used for binding to network with specified ip.
*
* @return UNW_SUCCESS on success. Others on error, please refer to tuya_os_adapter_error_code.h
*/
int tuya_hal_net_socket_bind(int fd, const char *ip);

/**
* @brief Get ip address by socket fd
*
* @param[in] fd: file descriptor
* @param[out] addr: ip address
*
* @note This API is used for getting ip address by socket fd.
*
* @return UNW_SUCCESS on success. Others on error, please refer to tuya_os_adapter_error_code.h
*/
#define tuya_hal_net_get_socket_ip(fd, addr) tal_net_get_socket_ip(fd, addr)

/**
* @brief Get ip address by ip string
*
* @param[in] ip: ip string
*
* @note This API is used for getting ip address by ip string
*
* @return ip address
*/
#define tuya_hal_net_addr(ip) tal_net_str2addr(ip)

#ifdef __cplusplus
}
#endif

#endif // __TUYA_HAL_NETWORK_H__

