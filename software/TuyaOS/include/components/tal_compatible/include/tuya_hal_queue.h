/**
 * @file tuya_hal_queue.h
 * @author shiqq@tuya.com
 * @brief queue相关接口封装
 * @version 0.1
 * @date 2021-01-04
 *
 * @copyright Copyright (c) tuya.inc 2019
 *
 */


#ifndef _TUYA_HAL_QUEUE_H
#define _TUYA_HAL_QUEUE_H

#include "tal_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief tuya_hal_queue_create_init用于创建并初始化tuya 消息队列
 *
 * @param[in] msgsize 消息体的大小，msgcount 消息体的个数
 * @param[out] pQueue_hanle 返回queue句柄
 * @return int 0=成功，非0=失败
 */
#define tuya_hal_queue_create_init      tal_queue_create_init

/**
 * @brief tuya_hal_queue_post用于发送一个消息到指定的队列中
 *
 * @param[in] pQueue_hanle tuya queue句柄，data消息体指针，timeout 超时时间
 * @return int 0=成功，非0=失败
 */
#define tuya_hal_queue_post             tal_queue_post
/**
 * @brief tuya_hal_queue_fetch用于等待一个消息队列
 *
 * @param[in] pQueue_hanle  tuya queue句柄，data消息体指针，timeout 超时时间
 * @return int 0=成功，非0=失败
 */
#define tuya_hal_queue_fetch            tal_queue_fetch

/**
 * @brief tuya_hal_queue_free 释放消息队列
 *
 * @param[in] mutexHandle QUEUE_HANDLE tuya queue句柄
 * @return int 0=成功，非0=失败
 */
#define tuya_hal_queue_free             tal_queue_free


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
