/**
* @file tkl_init_bt.h
* @brief Common process - tkl init adc description
* @version 0.1
* @date 2021-08-06
*
* @copyright Copyright 2021-2030 Tuya Inc. All Rights Reserved.
*
*/
typedef struct {
}TKL_BT_DESC_T;
/**
* @file tkl_init_bt.h
* @brief Common process - tkl init bt description
* @version 0.1
* @date 2021-08-06
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/
#ifndef __TKL_INIT_BT_H__
#define __TKL_INIT_BT_H__

#include "tkl_bluetooth.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief register bt description to tuya object manage
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
TKL_BT_DESC_T* tkl_bt_desc_get(VOID_T);


#ifdef __cplusplus
} // extern "C"
#endif

#endif // __TKL_INIT_BT_H__

