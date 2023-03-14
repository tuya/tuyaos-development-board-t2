/**
* @file tal_fs.h
* @brief Common process - Initialization
* @version 0.1
* @date 2021-09-09
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/
#ifndef __TAL_FS_H__
#define __TAL_FS_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************
 ********************************************************************************
 ********************************************************************************/


 /**
 * @brief Make directory
 *
 * @param[in] path: path of directory
 *
 * @note This API is used for making a directory
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
INT_T tal_fs_mkdir(CONST CHAR_T* path);

/**
* @brief Remove directory
*
* @param[in] path: path of directory
*
* @note This API is used for removing a directory
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
INT_T tal_fs_remove(CONST CHAR_T* path);

/**
* @brief Get file mode
*
* @param[in] path: path of directory
* @param[out] mode: bit attibute of directory
*
* @note This API is used for getting file mode.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
INT_T tal_fs_mode(CONST CHAR_T* path, UINT_T* mode);

/**
* @brief Check whether the file or directory exists
*
* @param[in] path: path of directory
* @param[out] is_exist: the file or directory exists or not
*
* @note This API is used to check whether the file or directory exists.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
INT_T tal_fs_is_exist(CONST CHAR_T* path, BOOL_T* is_exist);

/**
* @brief File rename
*
* @param[in] path_old: old path of directory
* @param[in] path_new: new path of directory
*
* @note This API is used to rename the file.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
INT_T tal_fs_rename(CONST CHAR_T* path_old, CONST CHAR_T* path_new);

/**
* @brief Open directory
*
* @param[in] path: path of directory
* @param[out] dir: handle of directory
*
* @note This API is used to open a directory
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
INT_T tal_dir_open(CONST CHAR_T* path, TUYA_DIR* dir);

/**
* @brief Close directory
*
* @param[in] dir: handle of directory
*
* @note This API is used to close a directory
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
INT_T tal_dir_close(TUYA_DIR dir);

/**
* @brief Read directory
*
* @param[in] dir: handle of directory
* @param[out] info: file information
*
* @note This API is used to read a directory.
* Read the file information of the current node, and the internal pointer points to the next node.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
INT_T tal_dir_read(TUYA_DIR dir, TUYA_FILEINFO* info);

/**
* @brief Get the name of the file node
*
* @param[in] info: file information
* @param[out] name: file name
*
* @note This API is used to get the name of the file node.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
INT_T tal_dir_name(TUYA_FILEINFO info, CONST CHAR_T** name);

/**
* @brief Check whether the node is a directory
*
* @param[in] info: file information
* @param[out] is_dir: is directory or not
*
* @note This API is used to check whether the node is a directory.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
INT_T tal_dir_is_directory(TUYA_FILEINFO info, BOOL_T* is_dir);

/**
* @brief Check whether the node is a normal file
*
* @param[in] info: file information
* @param[out] is_regular: is normal file or not
*
* @note This API is used to check whether the node is a normal file.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/

INT_T tal_dir_is_regular(TUYA_FILEINFO info, BOOL_T* is_regular);

/**
* @brief Open file
*
* @param[in] path: path of file
* @param[in] mode: file open mode: "r","w"...
*
* @note This API is used to open a file
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
TUYA_FILE tal_fopen(CONST CHAR_T* path, CONST CHAR_T* mode);

/**
* @brief Close file
*
* @param[in] file: file handle
*
* @note This API is used to close a file
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
INT_T tal_fclose(TUYA_FILE file);

/**
* @brief Read file
*
* @param[in] buf: buffer for reading file
* @param[in] bytes: buffer size
* @param[in] file: file handle
*
* @note This API is used to read a file
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
INT_T tal_fread(VOID_T* buf, INT_T bytes, TUYA_FILE file);

/**
* @brief write file
*
* @param[in] buf: buffer for writing file
* @param[in] bytes: buffer size
* @param[in] file: file handle
*
* @note This API is used to write a file
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
INT_T tal_fwrite(VOID_T* buf, INT_T bytes, TUYA_FILE file);

/**
* @brief write buffer to flash
*
* @param[in] fd: file fd
*
* @note This API is used to write buffer to flash
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
INT_T tal_fsync(INT_T fd);

/**
* @brief Read string from file
*
* @param[in] buf: buffer for reading file
* @param[in] len: buffer size
* @param[in] file: file handle
*
* @note This API is used to read string from file
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
CHAR_T* tal_fgets(CHAR_T* buf, INT_T len, TUYA_FILE file);

/**
* @brief Check wheather to reach the end fo the file
*
* @param[in] file: file handle
*
* @note This API is used to check wheather to reach the end fo the file
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
INT_T tal_feof(TUYA_FILE file);

/**
* @brief Seek to the offset position of the file
*
* @param[in] file: file handle
* @param[in] offs: offset
* @param[in] whence: seek start point mode
*
* @note This API is used to seek to the offset position of the file.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
INT_T tal_fseek(TUYA_FILE file, INT64_T offs, INT_T whence);

/**
* @brief Get current position of file
*
* @param[in] file: file handle
*
* @note This API is used to get current position of file.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
INT64_T tal_ftell(TUYA_FILE file);

/**
* @brief Get file size
*
* @param[in] filepath file path + file name
*
* @note This API is used to get the size of file.
*
* @return the sizeof of file
*/
INT_T tal_fgetsize(IN CONST CHAR_T *filepath);

/**
* @brief Judge if the file can be access
*
* @param[in] filepath file path + file name
*
* @param[in] mode access mode
*
* @note This API is used to access one file.
*
* @return 0 success,-1 failed
*/
INT_T tal_faccess(IN CONST CHAR_T *filepath,IN INT_T mode);

/**
* @brief read the next character from stream
*
* @param[in] file char stream
*
* @note This API is used to get one char from stream.
*
* @return as an unsigned char cast to a int ,or EOF on end of file or error
*/
INT_T tal_fgetc(IN TUYA_FILE file);

/**
* @brief flush the IO read/write stream
*
* @param[in] file char stream
*
* @note This API is used to flush the IO read/write stream.
*
* @return 0 success,-1 failed
*/
INT_T tal_fflush(IN TUYA_FILE file);

/**
* @brief get the file fd
*
* @param[in] file char stream
*
* @note This API is used to get the file fd.
*
* @return the file fd
*/
INT_T tal_fileno(IN TUYA_FILE file);

/**
* @brief truncate one file according to the length
*
* @param[in] fd file description
*
* @param[in] length the length want to truncate
*
* @note This API is used to truncate one file.
*
* @return 0 success,-1 failed
*/
INT_T tal_ftruncate(INT_T fd, UINT64_T length);

#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif

