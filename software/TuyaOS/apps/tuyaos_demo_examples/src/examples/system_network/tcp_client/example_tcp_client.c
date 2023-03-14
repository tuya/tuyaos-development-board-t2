/**
 * @file example_tcp_client.c
 * @author www.tuya.com
 * @brief example_tcp_client module is used to 
 * @version 0.1
 * @date 2022-11-22
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

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
#define TCP_SERVER_IP "192.168.137.1"
#define TCP_SERVER_PORT 1234

/***********************************************************
***********************typedef define***********************
***********************************************************/


/***********************************************************
********************function declaration********************
***********************************************************/


/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC THREAD_HANDLE tcp_client = NULL;

/***********************************************************
***********************function define**********************
***********************************************************/

/**
 * @brief tcp client task, close the tcp client after sending 5 times.
 *
 * @param[in] : 
 *
 * @return none
 */
STATIC VOID __tcp_client_task(PVOID_T args)
{
    OPERATE_RET rt = OPRT_OK;
    INT_T sock_fd;
    TUYA_IP_ADDR_T server_ip;
    TUYA_ERRNO net_errno = 0;
    UINT8_T cnt = 0;
    CHAR_T send_buf[] = {"Hello Tuya"};

    /* wait connect network */
    GW_WIFI_NW_STAT_E nw_status = STAT_LOW_POWER;
    TAL_PR_NOTICE("--- You must call the \"example_soc_init\" command to connect to the network to continue the execution");
    while (nw_status<STAT_CLOUD_CONN || nw_status>STAT_MQTT_OFFLINE) {
        TUYA_CALL_ERR_LOG(wifi_get_network_status(&nw_status));
        tal_system_sleep(500);
    }

    /* TCP server */
    sock_fd = tal_net_socket_create(PROTOCOL_TCP);

    server_ip = tal_net_str2addr(TCP_SERVER_IP);
    TAL_PR_DEBUG("connect tcp server ip: %s, port: %d", TCP_SERVER_IP, TCP_SERVER_PORT);
    net_errno = tal_net_connect(sock_fd, server_ip, TCP_SERVER_PORT);
    if (net_errno < 0) {
        TAL_PR_ERR("connect fail, exit");
        goto __EXIT;
    }

    for (;;) {
        net_errno = tal_net_send(sock_fd, send_buf, strlen(send_buf));
        if (net_errno < 0) {
            TAL_PR_ERR("send fail, exit");
            break;
        }

        cnt++;
        if (cnt >= 5) {
            break;
        }
        tal_system_sleep(2000);
    }

__EXIT:
    tal_net_close(sock_fd);

    tal_thread_delete(tcp_client);
    tcp_client=NULL;

    return;
}

/**
 * @brief tcp client example
 *
 * @param[in] : 
 *
 * @return none
 */
VOID example_tcp_client(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;

    THREAD_CFG_T thread_cfg = {
        .thrdname = "eg_tcp_client",
        .stackDepth = 1024*3,
        .priority = THREAD_PRIO_2,
    };
    if (NULL == tcp_client) {
        TUYA_CALL_ERR_GOTO(tal_thread_create_and_start(&tcp_client, NULL, NULL, __tcp_client_task, NULL, &thread_cfg), __EXIT);
    }

__EXIT:
    return;
}
