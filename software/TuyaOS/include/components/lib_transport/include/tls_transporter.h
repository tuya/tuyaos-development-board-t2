#ifndef __TLS_TRANSPORTER_H__
#define __TLS_TRANSPORTER_H__
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 创建tls transporter，并返回handler
 *
 * @note: 创建tls transporter，并返回handler
 *
 * @return tuya_transport_array_handle_t: transport handle
 */
tuya_transporter_t tuya_tls_transporter_create();

/**
* @brief 销毁transporter
*
* @param[in] transporter: tls_transporter handle
*
* @note: 销毁transport，释放资源
*
* @return 0: 成功; <0:请参照tuya error code描述文档
*/
OPERATE_RET tuya_tls_transporter_destroy(tuya_transporter_t transporter);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
