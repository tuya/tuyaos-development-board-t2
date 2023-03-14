/**
 * @file cloud_storage.h
 * @author Odin (aoding.xia@tuya.com)
 * @brief cloud storage
 * @version 0.1
 * @date 2021-11-11
 * 
 * @copyright Copyright (c) 2015-2021 Tuya Inc. All Rights Reserved.
 * 
 */

#ifndef __CLOUD_STORAGE_H__
#define __CLOUD_STORAGE_H__

#include "tuya_cloud_types.h"
#include "ty_cJSON.h"

#ifdef __cplusplus
    extern "C" {
#endif

#define AES128_CBC_LEN_WITH_PKCS7(len) ((len) + (16 - (len) % 16))
#define CLOUD_STORAGE_ENC_LEN(len) AES128_CBC_LEN_WITH_PKCS7(len)

struct storage_ctx;
typedef struct storage_ctx storage_ctx;

/**
 * @brief Data backup(simple mode)
 * 
 * @param dev_id device id string
 * @param scene_code scene code string
 * @param file_name file name string, path name is not permit
 * @param encrypt Whether encrypted, AES128_CBC is the default encrypt algorithm
 * @param data data which want to upload to cloud
 * @param data_len data length
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET cloud_storage_backup_simple(CONST CHAR_T *dev_id,
                                        CONST CHAR_T *scene_code,
                                        CONST CHAR_T *file_name,
                                        BOOL_T encrypt,
                                        CONST BYTE_T *data,
                                        UINT_T data_len);

/**
 * @brief Data restore(simple mode)
 * 
 * @param dev_id device id string
 * @param scene_code scene code string
 * @param file_name file name string, path name is not permit
 * @param encrypt Whether encrypted, AES128_CBC is the default encrypt algorithm
 * @param out_data data which restored from cloud, need manual Free() by caller user
 * @param out_len data length
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET cloud_storage_restore_simple(CONST CHAR_T *dev_id,
                                        CONST CHAR_T *scene_code,
                                        CONST CHAR_T *file_name,
                                        BOOL_T encrypt,
                                        BYTE_T **out_data,
                                        UINT_T *out_len);

/**
 * @brief Data destory(simple mode)
 * 
 * @param dev_id device id string
 * @param scene_code scene code string
 * @param file_name file name string, path name is not permit
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET cloud_storage_destory_simple(CONST CHAR_T *dev_id,
                                        CONST CHAR_T *scene_code,
                                        CONST CHAR_T *file_name);

/**
 * @brief Checking backup data exist
 * 
 * @param dev_id device id string
 * @param scene_code scene code string
 * @param file_name file name string, path name is not permit
 * @param exist Exist status
 * @return OPERATE_RET 
 */
OPERATE_RET cloud_storage_is_backup_exist(CONST CHAR_T *dev_id,
                              CONST CHAR_T *scene_code,
                              CONST CHAR_T *file_name,
                              BOOL_T *exist);

/**
 * @brief Get file meta data(json formatted)
 * 
 * @param dev_id device id string
 * @param file_name file name string, path name is not permit
 * @param scene_code scene code string
 * @param result 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET cloud_storage_meta_data_get(CONST CHAR_T *dev_id,
                                        CONST CHAR_T *scene_code,
                                        CONST CHAR_T * file_name,
                                        ty_cJSON **result);

/**
 * @brief create storage context
 * 
 * @param dev_id device id string
 * @param scene_code scene code string
 * @param file_name file name string, path name is not permit
 * @return storage_ctx* 
 */
storage_ctx *cloud_storage_ctx_new(CONST CHAR_T *dev_id,
                                    CONST CHAR_T *scene_code,
                                    CONST CHAR_T *file_name);

/**
 * @brief release storage context resource
 * 
 * @param ctx Cloud storage context handler
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET cloud_storage_ctx_free(storage_ctx *ctx);

/**
 * @brief backup prepare
 * 
 * @param ctx Cloud storage context handler
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET cloud_storage_backup_prepare(storage_ctx *ctx);

/**
 * @brief backup uploading
 * 
 * @param ctx Cloud storage context handler
 * @param data data for backup
 * @param data_len data length
 * @param encrypt Set as TRUE if need data encrypted
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET cloud_storage_backup(storage_ctx *ctx,
                         CONST BYTE_T *data,
                         UINT_T data_len,
                         BOOL_T encrypt);

/**
 * @brief Backup data prepare
 * 
 * @param ctx Cloud storage context handler
 * @param data_total_len total length for downloading
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET cloud_storage_backup_raw_begin(storage_ctx *ctx, UINT_T data_total_len);

/**
 * @brief Backup data to the cloud in raw mode
 * 
 * @param ctx Cloud storage context handler
 * @param data data for backup
 * @param data_len data length
 * @param encrypt Set as TRUE if need data encrypted
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET cloud_storage_backup_raw_write(storage_ctx *ctx,
                                    BYTE_T *data,
                                    UINT_T data_len,
                                    BOOL_T encrypt);

/**
 * @brief Data backup
 * 
 * @param ctx Cloud storage context handler
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET cloud_storage_backup_raw_end(storage_ctx *ctx);

/**
 * @brief Data backup postpone
 * 
 * @param ctx Cloud storage context handler
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET cloud_storage_backup_post(storage_ctx *ctx);

/**
 * @brief Prepare for data restore
 * 
 * @param ctx Cloud storage context handler
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET cloud_storage_restore_prepare(storage_ctx *ctx);

/**
 * @brief Data restore
 * 
 * @param ctx Cloud storage context handler
 * @param out_data output data point for restore, need Free manually
 * @param out_len output data lngth point for restore, need Free manually
 * @param encrypt Whether encrypted, AES128_CBC is the default encrypt algorithm
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET cloud_storage_restore(storage_ctx *ctx,
                          BYTE_T **out_data,
                          UINT_T *out_len,
                          BOOL_T encrypt);

/**
 * @brief Data restore postpone
 * 
 * @param ctx Cloud storage context handler
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET cloud_storage_restore_post(storage_ctx *ctx);

/**
 * @brief Data destory prepare
 * 
 * @param ctx Cloud storage context handler
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET cloud_storage_destory_prepare(storage_ctx *ctx);

/**
 * @brief Data destory
 * 
 * @param ctx Cloud storage context handler
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET cloud_storage_destory(storage_ctx *ctx);

/**
 * @brief Data destory postpone
 * 
 * @param ctx Cloud storage context handler
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET cloud_storage_destory_post(storage_ctx *ctx);

/**
 * @brief Get secret key
 * 
 * @param ctx Cloud storage context handler
 * @return CONST CHAR_T* 
 */
CONST CHAR_T *cloud_storage_secret_key_get(IN CONST storage_ctx *ctx);

/**
 * @brief Set secret key
 * 
 * @param ctx Cloud storage context handler
 * @param key secret key
 * @return VOID
 */
VOID cloud_storage_secret_key_set(IN storage_ctx *ctx, IN CONST CHAR_T *key);

/**
 * @brief Checking secret key is exist
 * 
 * @param ctx Cloud storage context handler
 * @return TRUE or FALSE 
 */
BOOL_T cloud_storage_is_secret_key_exist(IN CONST storage_ctx *ctx);

/**
 * @brief Fetch IV, for AES128-CBC
 * 
 * @param ctx Cloud storage context handler
 * @return iv string
 */
CONST CHAR_T *cloud_storage_secret_iv_get(IN CONST storage_ctx *ctx);

/**
 * @brief 
 * 
 * @param ctx Cloud storage context handler
 * @param iv 
 * @return VOID
 */
VOID cloud_storage_secret_iv_set(IN storage_ctx *ctx, IN CONST CHAR_T *iv);

/**
 * @brief 获取签名URL, 返回值生命周期与ctx关联
 * 
 * @param ctx Cloud storage context handler
 * @return CONST CHAR_T* 
 */
CONST CHAR_T *cloud_storage_get_sign_url(IN CONST storage_ctx *ctx);

/**
 * @brief AES128-CBC Encrypt
 * 
 * @param ctx Cloud storage context handler
 * @param data data for encrypt
 * @param len data length for encrypt
 * @param ec_data data encrypted
 * @param ec_len data length encrypted
 * @return OPERATE_RET 
 */
OPERATE_RET cloud_storage_aes128_cbc_encrypt(IN CONST storage_ctx *ctx,
                                            IN CONST BYTE_T *data, IN CONST UINT_T len,
                                            OUT BYTE_T **ec_data, OUT UINT_T *ec_len);

/**
 * @brief AES128-CBC Decrypt
 * 
 * @param ctx Cloud storage context handler
 * @param data data for decrypted
 * @param len data length for decrypted
 * @param dec_data data decrypted
 * @param dec_len data length decrypted
 * @return OPERATE_RET 
 */
OPERATE_RET cloud_storage_aes128_cbc_decrypt(IN CONST storage_ctx *ctx,
                                            IN CONST BYTE_T *data, IN CONST UINT_T len,
                                            OUT BYTE_T **dec_data, OUT UINT_T *dec_len);
#ifdef __cplusplus
}
#endif

#endif
