/**
 * @file uni_rfc.h
 * @brief tuya reference data module
 * @version 1.0
 * @date 2019-10-30
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */
#ifndef _UNI_RFC_H
#define _UNI_RFC_H

#include "tuya_smartpointer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief the reference data
 * 
 */
typedef SMARTPOINTER_T RFC_DATA_S;

/**
 * @brief create a reference data
 * 
 * @param[in] data the data buffer
 * @param[in] data_len the date length
 * @param[in] malk need malloc memory for the data
 * @param[in] cnt the Initial value of the reference
 * @return the reference data address
 */
#define cr_rfc_data(data, data_len,  malk, cnt) tuya_smartpointer_create(data, data_len,  malk, cnt)

/**
 * @brief get the reference data, increase the reference
 * 
 * @param[inout] rfc_data the reference data
 * @return VOID 
 */
#define get_rfc_data(rfc_data) tuya_smartpointer_get(rfc_data)

/**
 * @brief put the reference data, decrease the reference
 * 
 * @param[inout] rfc_data the reference data 
 * @return VOID 
 * 
 * @note the reference data will be released when reference is 0
 */
#define put_rfc_data(rfc_data) tuya_smartpointer_put(rfc_data)

/**
 * @brief delete the reference data, ignore the reference
 * 
 * @param[inout] rfc_data the reference data 
 * @return VOID 
 */
#define del_rfc_data(rfc_data) tuya_smartpointer_put(rfc_data)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
