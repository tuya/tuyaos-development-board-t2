#ifndef __TKL_REGISTER_H__
#define __TKL_REGISTER_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"

/**
 * @brief read of chip addr
 *
 * @param[in] addr: chip addr 
 *
 * @return return  data of addr
 */
UINT32_T tkl_reg_read(UINT32_T addr);

/**
 * @brief bit read
 *
 * @param[in] addr: chip addr 
 *
 * @return return data of bits
 */
UINT32_T tkl_reg_bit_read(UINT32_T addr, TUYA_ADDR_BITS_DEF_E start_bit, TUYA_ADDR_BITS_DEF_E end_bit);

/**
 * @brief tuya write of chip addr
 *
 * @param[in] addr :  chip addr
 * @param[in] data: data  , write data
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_reg_write(UINT32_T addr, UINT32_T data);

/**
 * @brief bit write of chip addr
 *
 * @param[in] addr : chip addr
 * @param[in] start_bit : start bit to write
 *  @param[in] end_bit  : end bit to write
 *  @param[in] data     : data to write
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_reg_bit_write(UINT32_T addr, TUYA_ADDR_BITS_DEF_E start_bit, TUYA_ADDR_BITS_DEF_E end_bit, UINT32_T data);


#ifdef __cplusplus
} // extern "C"
#endif

#endif // __TKL_REGISTER_H__

