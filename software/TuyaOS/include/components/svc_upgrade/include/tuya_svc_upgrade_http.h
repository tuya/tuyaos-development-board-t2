/**
* @file tuya_svc_upgrade_http.h
* @brief TUYA OTA service
* @version 0.1
* @date 2019-08-28
*
* @copyright Copyright 2019-2021 Tuya Inc. All Rights Reserved.
*/

#ifndef __TUYA_SVC_UPGRADE_HTTP_H__
#define __TUYA_SVC_UPGRADE_HTTP_H__

#include "tuya_svc_upgrade.h"
#include "tuya_base_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Retrieve silent upgrade info from cloud
 *
 * @param[in] sub_id ID of sub-device, or NULL for GW
 * @param[out] result JSON encoded data
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET http_device_upgrade_info_get_silent(CONST CHAR_T *sub_id, ty_cJSON **result);

/**
 * @brief Retrieve default upgrade info from cloud
 *
 * @param[in] dev_id ID of sub-device, or NULL for GW
 * @param[in] tp OTA channel, see DEV_TYPE_T
 * @param[out] result JSON encoded data
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET http_device_upgrade_info_get(CONST CHAR_T *dev_id, CONST DEV_TYPE_T tp, ty_cJSON **result);

/**
 * @brief Sync upgrade status to cloud
 *
 * @param[in] dev_id ID of sub-device, or NULL for GW
 * @param[in] tp OTA channel, see DEV_TYPE_T
 * @param[in] status Upgrade status, see TI_UPGRD_STAT_S
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET http_device_upgrade_stat_update(CONST CHAR_T *dev_id, CONST DEV_TYPE_T tp, CONST BYTE_T status);

/**
 * @brief Retrieve lastest schema from cloud
 *
 * @param[in] schemaId ID of local schema
 * @param[in] version Version of local schema
 * @param[in] nodeId ID of sub-device, or NULL for GW
 * @param[out] result JSON encoded data
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET http_device_schema_newest_get(CONST CHAR_T* schemaId, CONST CHAR_T* version,
                                          CONST CHAR_T* nodeId, ty_cJSON **result);

/**
 * @brief Sync schema upgrade result to cloud
 *
 * @param[in] schemaId ID of local schema
 * @param[in] version Version of local schema
 * @param[in] nodeIds IDs of sub-device, or NULL for GW
 * @param[in] result Upgrade result
 * @param[in] result Reason
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET http_device_schema_version_update(CONST CHAR_T* schemaId, CONST CHAR_T* version,
                                              ty_cJSON *nodeIds, CONST CHAR_T* result, CONST CHAR_T* reason);

#ifdef __cplusplus
}
#endif

#endif


