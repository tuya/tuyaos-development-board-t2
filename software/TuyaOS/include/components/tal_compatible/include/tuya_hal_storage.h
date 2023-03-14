/**
* @file tuya_hal_storage.h
* @brief Common process - Initialization
* @version 0.1
* @date 2020-11-09
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/
#ifndef __TUYA_HAL_STORAGE_H__
#define __TUYA_HAL_STORAGE_H__

#include "tal_flash.h"

#ifdef __cplusplus
    extern "C" {
#endif

/**
 * @brief storage description
 * 
 */
typedef struct {
    unsigned int start_addr;    ///< user physical flash start address 
    unsigned int flash_sz;      ///< user flash size
    unsigned int block_sz;      ///< flash block/sector size

    // For data backup and power down protection data recovery
    unsigned int swap_start_addr;   ///< swap flash start address
    unsigned int swap_flash_sz;     ///< swap flash size    

    /// for restore factor of flash encryption key 
    unsigned int key_restore_addr;
    
    // for protected data
    unsigned int protected_addr;
    unsigned int flash_physical_block_sz;
} UNI_STORAGE_DESC_S;


#define PARTATION_NUM_MAX 3

/**
 * @brief UF partition
 * 
 */
typedef struct {
    unsigned int uf_partition_start_addr;
    unsigned int uf_partiton_flash_sz;
} UF_PARTITION;

/**
 * @brief UF description
 * 
 */
typedef struct {
    unsigned int sector_size;
    unsigned int  uf_partition_num;
    UF_PARTITION uf_partition[PARTATION_NUM_MAX];
} UF_PARTITION_TABLE_S;

typedef UF_PARTITION_TABLE_S PARTITION_TABLE_S;

/**
* @brief Read flash
*
* @param[in] addr: flash address
* @param[out] dst: pointer of buffer
* @param[in] size: size of buffer
*
* @note This API is used for reading flash.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
#define tuya_hal_flash_read(addr, dst, size) tal_flash_read(addr, dst, size)

/**
* @brief Write flash
*
* @param[in] addr: flash address
* @param[in] src: pointer of buffer
* @param[in] size: size of buffer
*
* @note This API is used for reading flash.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
#define tuya_hal_flash_write(addr, src, size) tal_flash_write(addr, src, size)

/**
* @brief Erase flash
*
* @param[in] addr: flash address
* @param[in] size: size of flash block
*
* @note This API is used for erasing flash.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
#define tuya_hal_flash_erase(addr, size) tal_flash_erase(addr, size)

/**
* @brief Get description of storage
*
* @param void
*
* @note This API is used to get description of storage.
*
* @return the description of storage
*/
UNI_STORAGE_DESC_S* tuya_hal_storage_get_desc(void);

/**
* @brief Get UF file description
*
* @param void
*
* @note This API is used to get UF file description.
*
* @return the UF file description
*/
UF_PARTITION_TABLE_S* tuya_hal_uf_get_desc(void);

/**
 * @brief get rcd flash description
 *
 * @return  pointer to descrtion of rcd flash
 */
PARTITION_TABLE_S *tuya_hal_get_rcd_flash_desc(void);


#ifdef __cplusplus
}
#endif

#endif // __TUYA_HAL_STORAGE_H__

