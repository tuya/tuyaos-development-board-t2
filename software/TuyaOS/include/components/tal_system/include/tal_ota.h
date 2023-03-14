/**
* @file tal_ota.h
* @brief Common process - implement the ota operation of the ota
* @version 0.1
* @date 2021-08-24
*
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
*
*/
#ifndef __TAL_OTA_H__
#define __TAL_OTA_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif



/**
* @brief get ota ability
*
* @param[out] image_size:  max image size
* @param[out] type:        full package and compress package upgrade use TUYA_OTA_FULL
*                          diff package upgrade use TUYA_OTA_DIFF
* @note This API is used for get chip ota ability
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tal_ota_get_ability(UINT_T *image_size, TUYA_OTA_TYPE_E *type);

/**
* @brief ota start notify
*
* @param[in] image_size:  image size
* @param[in] type:        ota type
*
* @note This API is used for ota start notify
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tal_ota_start_notify(UINT_T image_size, TUYA_OTA_TYPE_E type, TUYA_OTA_PATH_E path);

/**
* @brief ota data process
*
* @param[in] pack:       point to ota pack
* @param[in] remain_len: ota pack remain len
*
* @note This API is used for ota data process
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tal_ota_data_process(TUYA_OTA_DATA_T *pack, UINT_T* remain_len);

/**
* @brief ota end notify
*
* @param[in] reset:  ota reset
*
* @note This API is used for ota end notify
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tal_ota_end_notify(BOOL_T reset);



/**
* @brief get old firmware info
*
* @param[out] image_size:  max image size
* @param[out] type:        ota type
*
* @note This API is used for old firmware info, and only used in resumes transmission at break-points
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tal_ota_get_old_firmware_info(TUYA_OTA_FIRMWARE_INFO_T **info);


#ifdef __cplusplus
}
#endif

#endif
