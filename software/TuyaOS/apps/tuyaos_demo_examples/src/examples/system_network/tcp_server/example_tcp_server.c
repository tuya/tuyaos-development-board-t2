/**
 * @file example_tcp_server.c
 * @author www.tuya.com
 * @brief example_tcp_server module is used to 
 * @version 0.1
 * @date 2022-11-22
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include <stdio.h>

#include "tuya_cloud_types.h"
#include "tuya_wifi_status.h"
#include "tuya_cloud_wifi_defs.h"
#include "tal_log.h"
#include "tal_system.h"
#include "tal_thread.h"
#include "tal_network.h"

/***********************************************************
************************macro define************************
***********************************************************/
#define SERVER_PORT     1234

/***********************************************************
***********************typedef define***********************
***********************************************************/


/***********************************************************
********************function declaration********************
***********************************************************/


/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC THREAD_HANDLE tcp_server = NULL;

/***********************************************************
***********************function define**********************
***********************************************************/

/**
 * @brief tcp server task
 *
 * @param[in] : 
 *
 * @return none
 */
STATIC VOID __tcp_server_task(PVOID_T args)
{
    OPERATE_RET rt = OPRT_OK;
    CHAR_T recv_buf[255] = {0};
    INT_T listen_fd, sock_fd;
    TUYA_ERRNO net_errno = 0;

    /* wait connect network */
    GW_WIFI_NW_STAT_E nw_status = STAT_LOW_POWER;
    TAL_PR_NOTICE("=== You must call the \"example_soc_init\" command to connect to the network to continue the execution");
    while (nw_status<STAT_CLOUD_CONN || nw_status>STAT_MQTT_OFFLINE) {
        TUYA_CALL_ERR_LOG(wifi_get_network_status(&nw_status));
        tal_system_sleep(500);
    }

    /* TCP server */
    listen_fd = tal_net_socket_create(PROTOCOL_TCP);

    tal_net_bind(listen_fd, TY_IPADDR_ANY, SERVER_PORT);

    tal_net_listen(listen_fd, 1);

    TUYA_IP_ADDR_T client_ip = 0;
    UINT16_T client_port = 0;
    sock_fd = tal_net_accept(listen_fd, &client_ip, &client_port);
    CHAR_T *client_ip_str = tal_net_addr2str(client_ip);
    TAL_PR_DEBUG("accept client ip:%s, port:%d", client_ip_str, client_port);

    memset(recv_buf, 0, 255);
    snprintf(recv_buf, 255, "You can send \"stop\" to stop the tcp service.");
    tal_net_send(sock_fd, recv_buf, strlen(recv_buf));

    for (;;) {
        memset(recv_buf, 0, 255);
        net_errno = tal_net_recv(sock_fd, recv_buf, 255);
        TAL_PR_DEBUG("%s:%d==> %s", client_ip_str, client_port, recv_buf);
        if (0 == strcmp("stop", recv_buf) || net_errno < 0) {
            break;
        }

        tal_net_send(sock_fd, recv_buf, strlen(recv_buf));
    }

    tal_net_close(sock_fd);
    tal_net_close(listen_fd);

    tal_thread_delete(tcp_server);
    tcp_server = NULL;

    return;
}

/**
 * @brief tcp server example
 *
 * @param[in] : 
 *
 * @return none
 */
VOID example_tcp_server(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;

    THREAD_CFG_T thread_cfg = {
        .thrdname = "eg_tcp_server",
        .stackDepth = 1024*3,
        .priority = THREAD_PRIO_2,
    };
    if (NULL == tcp_server) {
        TUYA_CALL_ERR_GOTO(tal_thread_create_and_start(&tcp_server, NULL, NULL, __tcp_server_task, NULL, &thread_cfg), __EXIT);
    }

__EXIT:
    return;
}
