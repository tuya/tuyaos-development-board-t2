/**
 * @file hashmap.h
 * @brief tuya hasmap module
 * @version 1.0
 * @date 2019-10-30
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */

#ifndef __HASHMAP_H__
#define __HASHMAP_H__

#include "tuya_hashmap.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef MAP_T map_t;
typedef ANY_T any_t;
typedef ANY_T_ITER any_t_iter;

/**
 * @brief create a new empty hashmap
 * 
 * @param[in] table_size the hash table size
 * @return a new empty hashmap 
 */
#define hashmap_new(table_size) tuya_hashmap_new(table_size)


/**
 * @brief Add an element to the hashmap
 * 
 * @param[in] in the hashmap
 * @param[in] key the key of hash element
 * @param[in] data the data of hash element
 * @return MAP_OK on success, others on failed, please refer to the define of hashmap error code 
 * 
 * @note For same key, it does not replace it. it is inserted in the head of the list
 */
#define hashmap_put(in, key , data) tuya_hashmap_put(in, key , data)

/**
 * @brief get an element from the hashmap
 * 
 * @param[in] in the hashmap
 * @param[in] key the key of the element
 * @param[out] arg the first value that the key matches
 * @return MAP_OK on success, others on failed, please refer to the define of hashmap error code  
 */
#define hashmap_get(in, key, arg) tuya_hashmap_get(in, key, arg)

/**
 * @brief traverse all data with same key
 * 
 * @param[in] in the hashmap
 * @param[in] key the key of element
 * @param[inout] arg_iterator the traverse iterator
 * @return MAP_OK on success, others on failed, please refer to the define of hashmap error code  
 * 
 * @note if arg_iterator is NULL, fetch the first element, otherwise, fetch the next element
 */
#define hashmap_data_traversal(in, key, arg_iterator) tuya_hashmap_data_traversal(in, key, arg_iterator)

/**
 * @brief traverse all data with same key
 * 
 */
#define HASHMAP_FOR_EACH_DATA(in, key, data_iter) TUYA_HASHMAP_FOR_EACH_DATA(in, key, data_iter)

/**
 * @brief remove an element from the hashmap
 * 
 * @param[in] in the hashmap
 * @param[in] key the key of element
 * @param[in] data the data of the element
 * @return MAP_OK on success, others on failed, please refer to the define of hashmap error code  
 * 
 * @note if data is NULL,then delete the first note match key.if data is not null, then delete the node match key and data.
 */
#define hashmap_remove(in, key, data) tuya_hashmap_remove(in, key, data)

/**
 * @brief free the hashmap
 * 
 * @param[in] in the hashmap need to free
 * 
 * @warning must remove all element first, otherwise, it will cause element leak
 */
#define hashmap_free(in) tuya_hashmap_free(in)

/**
 * @brief get current size of the hashmap
 * 
 * @param[in] in the hashmap
 * @return the current size
 */
#define hashmap_length(in) tuya_hashmap_length(in)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __HASHMAP_H__
