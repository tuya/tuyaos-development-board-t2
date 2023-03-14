/**
* @file tal_memory.h
* @brief Common process - adapter the memory api provide by OS
* @version 0.1
* @date 2021-08-24
*
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
*
*/
#ifndef __TAL_MEMORY_H__
#define __TAL_MEMORY_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief malloc memory
 * 
 * @param[in] reqSize the required memory size 
 * @return  NULL on failed, others on the address of the memory
 */
#define Malloc(req_size) tal_malloc(req_size)

/**
 * @brief malloc consecutive memory, the size is reqCount*reqSize
 * 
 * @param[in] reqCount the required memory count
 * @param[in] reqSize the required memory szie
 * @return NULL on failed, others on the address of the memory
 */
#define Calloc(req_count, req_size) tal_calloc(req_count, req_size)

/**
 * @brief free memory
 * 
 * @param[in] pReqMem the memory address which got from Malloc or Calloc function
 * @return VOID 
 */
#define Free(ptr) tal_free(ptr)

/**
* @brief Alloc memory of system
*
* @param[in] size: memory size
*
* @note This API is used to alloc memory of system.
*
* @return the memory address malloced
*/
VOID_T *tal_malloc(SIZE_T size);

/**
* @brief Free memory of system
*
* @param[in] ptr: memory point
*
* @note This API is used to free memory of system.
*
* @return VOID_T
*/
VOID_T tal_free(VOID_T* ptr);

/**
* @brief set memory
*
* @param[in] size: memory size
*
* @note This API is used to alloc memory of system.
*
* @return the memory address malloced
*/
VOID_T *tal_system_memset(VOID_T* src, INT_T ch, SIZE_T n);

/**
* @brief Alloc memory of system
*
* @param[in] size: memory size
*
* @note This API is used to alloc memory of system.
*
* @return the memory address malloced
*/
VOID_T *tal_system_memcpy(VOID_T* dst, CONST VOID_T* src, SIZE_T n);

/**
* @brief Compare the first n bytes of store str1 and store str2
*
* @param[in] str1: point to string1
* @param[in] str2: point to string2
* @param[in] n:    first n bytes
*
* @note This API is used to compare two strings
*
* @return value < 0: str1 is less than str2;
          value > 0: str1 is greater than str2;
          value = 0: str1 is equal to str2;
*/
INT_T tal_system_memcmp(CONST VOID_T *str1, CONST VOID_T *str2, SIZE_T n);

/**
* @brief 
*
* @param[in] str:
*
* @note 
*
* @return 
*/
SIZE_T tal_system_strlen(CONST CHAR_T *str);

/**
 * @brief Allocate and clear the memory
 * 
 * @param[in]       nitems      the numbers of memory block
 * @param[in]       size        the size of the memory block
 *
 * @return the memory address calloced
 */
VOID_T *tal_calloc(SIZE_T nitems, SIZE_T size);

/**
 * @brief Re-allocate the memory
 * 
 * @param[in]       nitems      source memory address
 * @param[in]       size        the size after re-allocate
 *
 * @return VOID_T
 */
VOID_T *tal_realloc(VOID_T* ptr, SIZE_T size);


#ifdef __cplusplus
} // extern "C"
#endif

#endif // __COMPONENT_HELLO_H__

