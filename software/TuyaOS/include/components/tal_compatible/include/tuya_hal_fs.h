/**
* @file tuya_hal_fs.h
* @brief Common process - Initialization
* @version 0.1
* @date 2021-09-13
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/
#ifndef __TUYA_HAL_FS_H__
#define __TUYA_HAL_FS_H__

#include <stdint.h>
#include <stdbool.h>
#include "tuya_os_adapter.h"
#include "tal_fs.h"

#ifdef __cplusplus
extern "C" {
#endif

 /**
 * @brief Make directory
 *
 * @param[in] path: path of directory
 *
 * @note This API is used for making a directory
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
#define  tuya_hal_fs_mkdir(path)                    tal_fs_mkdir(path)

/**
* @brief Remove directory
*
* @param[in] path: path of directory
*
* @note This API is used for removing a directory
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
#define tuya_hal_fs_remove(path)                    tal_fs_remove(path)

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
#define tuya_hal_fs_mode(path, mode)                tal_fs_mode(path, mode) 

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
#define tuya_hal_fs_is_exist(path, is_exist)        tal_fs_is_exist(path, is_exist)

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
#define tuya_hal_fs_rename(path_old, path_new)      tal_fs_rename(path_old, path_new)

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
#define tuya_hal_dir_open(path, dir)                tal_dir_open(path, dir)

/**
* @brief Close directory
*
* @param[in] dir: handle of directory
*
* @note This API is used to close a directory
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
#define tuya_hal_dir_close(dir)                     tal_dir_close(dir)

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
#define tuya_hal_dir_read(dir, info)                tal_dir_read(dir, info)

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
#define tuya_hal_dir_name(info, name)               tal_dir_name(info, name)

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
#define tuya_hal_dir_is_directory(info, is_dir)     tal_dir_is_directory(info, is_dir)

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

#define tuya_hal_dir_is_regular(info, is_regular)   tal_dir_is_regular(info, is_regular)

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
#define tuya_hal_fopen(path, mode)                  tal_fopen(path, mode)    

/**
* @brief Close file
*
* @param[in] file: file handle
*
* @note This API is used to close a file
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
#define tuya_hal_fclose(file)                       tal_fclose(file)

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
#define tuya_hal_fread(buf, bytes, file)            tal_fread(buf, bytes, file)      

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
#define tuya_hal_fwrite(buf, bytes, file)           tal_fwrite(buf, bytes, file)

/**
* @brief write buffer to flash
*
* @param[in] fd: file fd
*
* @note This API is used to write buffer to flash
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
#define tuya_hal_fsync(fd)                          tal_fsync(fd)

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
#define tuya_hal_fgets(buf, len, file)              tal_fgets(buf, len, file)

/**
* @brief Check wheather to reach the end fo the file
*
* @param[in] file: file handle
*
* @note This API is used to check wheather to reach the end fo the file
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
#define tuya_hal_feof(file)                         tal_feof(file)

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
#define tuya_hal_fseek(file, offs, whence)          tal_fseek(file, offs, whence)   

/**
* @brief Get current position of file
*
* @param[in] file: file handle
*
* @note This API is used to get current position of file.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
#define tuya_hal_ftell(file)                        tal_ftell(file)

/**
* @brief Get file size
*
* @param[in] filepath file path + file name
*
* @note This API is used to get the size of file.
*
* @return the sizeof of file
*/
#define tuya_hal_fgetsize(filepath)                 tal_fgetsize(filepath)

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
#define tuya_hal_faccess(Cfilepath, mode)           tal_faccess(Cfilepath, mode)

/**
* @brief read the next character from stream
*
* @param[in] file char stream
*
* @note This API is used to get one char from stream.
*
* @return as an unsigned char cast to a int ,or EOF on end of file or error
*/
#define tuya_hal_fgetc(file)                        tal_fgetc(file)

/**
* @brief flush the IO read/write stream
*
* @param[in] file char stream
*
* @note This API is used to flush the IO read/write stream.
*
* @return 0 success,-1 failed
*/
#define tuya_hal_fflush(file)                       tal_fflush(file)

/**
* @brief get the file fd
*
* @param[in] file char stream
*
* @note This API is used to get the file fd.
*
* @return the file fd
*/
#define tuya_hal_fileno(file)                       tal_fileno(file)

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
#define tuya_hal_ftruncate(fd, length)              tal_ftruncate(fd, length)


#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif // __TUYA_HAL_FS_H__
