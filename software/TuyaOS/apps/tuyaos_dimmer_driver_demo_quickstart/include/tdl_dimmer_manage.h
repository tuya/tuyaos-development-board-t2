/**
 * @file tdl_dimmer_manage.h
 * @author www.tuya.com
 * @brief tdl_dimmer_manage module is used to manage dimmer,
 *        provide unified driver interfaces for the upper layer
 * @version 0.1
 * @date 2022-07-11
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#ifndef __TDL_DIMMER_MANAGE_H__
#define __TDL_DIMMER_MANAGE_H__

#include "tdu_light_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************macro define************************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/
/**
 * @brief dimmer handle type
 */
typedef VOID_T* TDL_DIMMER_HANDLE;

/***********************************************************
********************function declaration********************
***********************************************************/
/**
 * @brief find dimmer
 *
 * @param[in] name: dimmer name
 * @param[out] handle: dimmer handle
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_dimmer_find(IN CHAR_T *name, OUT TDL_DIMMER_HANDLE *handle);


/**
 * @brief open dimmer
 *
 * @param[in] handle: dimmer handle
 * @param[in] app_data_max: the maximum value of the output data that application sets
 * @param[in] get_switch_cb: get switch state callback function
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_dimmer_open(IN TDL_DIMMER_HANDLE handle, IN USHORT_T app_data_max);


/**
 * @brief close dimmer
 *
 * @param[in] handle: dimmer handle
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_dimmer_close(IN TDL_DIMMER_HANDLE handle);


/**
 * @brief output data to dimmer driver
 *
 * @param[in] handle: dimmer handle
 * @param[in] p_rgbcw: set current rgbcw
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_dimmer_output(IN TDL_DIMMER_HANDLE handle, IN LIGHT_RGBCW_T *data);


/**
 * @brief output data to dimmer driver for_shade
 *
 * @param[in] handle: dimmer handle
 * @param[in] p_src_rgbcw: src rgbcw
 * @param[in] p_curr_rgbcw: current rgbcw
 * @param[in] p_target_rgbcw: target rgbcw
 * 
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_dimmer_output_for_shade(IN TDL_DIMMER_HANDLE handle, IN LIGHT_RGBCW_T *p_src_rgbcw, \
                                        IN LIGHT_RGBCW_T *p_curr_rgbcw, IN LIGHT_RGBCW_T *p_target_rgbcw);
                                        

/**
 * @brief get output data
 *
 * @param[in] handle: dimmer handle
 * @param[out] p_rgbcw: current rgbcw
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_dimmer_get_current_rgbcw(IN TDL_DIMMER_HANDLE handle, OUT LIGHT_RGBCW_T *p_rgbcw);


/**
 * @brief get app resolution
 *
 * @param[in] handle: dimmer handle
 * @param[out] resolution: app resolution
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_dimmer_get_app_resolution(IN TDL_DIMMER_HANDLE handle, OUT USHORT_T *resolution);

#ifdef __cplusplus
}
#endif

#endif /* __TDL_DIMMER_MANAGE_H__ */
