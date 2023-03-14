/**
 * @file tdl_dimmer_manage.c
 * @author www.tuya.com
 * @brief tdl_dimmer_manage module is used to manage dimmer
 * @version 0.1
 * @date 2022-08-22
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */
#include <string.h>

#include "tal_mutex.h"
#include "tal_memory.h"
#include "tal_log.h"

#include "tdl_dimmer_manage.h"
#include "tdl_dimmer_driver.h"

/***********************************************************
************************macro define************************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/
/**
 * @brief dimmer list type
 */
typedef struct dimmer_list {
    struct dimmer_list     *next;
    CHAR_T                  name[LIGHT_NAME_MAX_LEN+1];
    MUTEX_HANDLE            mutex;
    USHORT_T                app_resolution;
    LIGHT_RGBCW_U           rgbcw;

    UCHAR_T                 drv_type;
    USHORT_T                drv_resolution;
    DIMMER_DRV_HANDLE       drv_handle;
    DIMMER_DRV_INTFS_T      intfs;
    LIGHT_RGBCW_U           output_rgbcw;
} DIMMER_NODE_T, DIMMER_LIST_T;

/***********************************************************
********************function declaration********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
/**
 * @brief dimmer list root node
 */
STATIC DIMMER_LIST_T sg_dimmer_list_head = {
    .next = NULL,
};

/***********************************************************
***********************function define**********************
***********************************************************/

/**
 * @brief find dimmer node
 *
 * @param[in] list: dimmer list head
 * @param[in] name: dimmer name
 *
 * @return dimmer node on found. NULL on unfound.
 */
STATIC DIMMER_NODE_T *__tdl_dimmer_find(IN DIMMER_LIST_T *list, IN CHAR_T *name)
{
    DIMMER_NODE_T *node = NULL;

    if (NULL == list || NULL == name) {
        return NULL;
    }

    node = list->next;
    while (node != NULL) {
        if (0 == strcmp(name, node->name)) {
            return node;
        }
        node = node->next;
    }

    return NULL;
}


/**
 * @brief register dimmer
 *
 * @param[in] name: dimmer name
 * @param[in] p_intfs: interfaces for operating the dimmer
 * @param[in] drv_resolution: the resolution of the dimmer driver
 * @param[in] drv_type: dimmer driver type
 * @param[in] drv_handle: dimmer driver handle
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
STATIC OPERATE_RET __tdl_dimmer_register(IN CHAR_T *name, IN DIMMER_DRV_INTFS_T *p_intfs, IN UCHAR_T drv_type, \
                                         IN USHORT_T drv_resolution, IN DIMMER_DRV_HANDLE drv_handle)
{
    /* check if the dimmer is already registered */
    if (NULL != __tdl_dimmer_find(&sg_dimmer_list_head, name)) {
        TAL_PR_ERR("The dimmer %s already exists.", name);
        return OPRT_COM_ERROR;
    }

    /* malloc for this new dimmer */
    DIMMER_NODE_T *device = (DIMMER_NODE_T *)tal_malloc(SIZEOF(DIMMER_NODE_T));
    if (NULL == device) {
        return OPRT_MALLOC_FAILED;
    }
    memset(device, 0x00, SIZEOF(DIMMER_NODE_T));

    /* create a mutex for this dimmer */
    OPERATE_RET op_ret = tal_mutex_create_init(&device->mutex);
    if (OPRT_OK != op_ret) {
        tal_free(device);
        device = NULL;
        return op_ret;
    }

    /* save information of this dimmer */
    strncpy(device->name, name, LIGHT_NAME_MAX_LEN);
    memcpy(&device->intfs, p_intfs, SIZEOF(DIMMER_DRV_INTFS_T));
    device->drv_handle = drv_handle;
    device->drv_type = drv_type;
    device->drv_resolution = drv_resolution;

    /* find the last node and add this new dimmer */
    DIMMER_NODE_T *last_node = &sg_dimmer_list_head;
    while (last_node->next) {
        last_node = last_node->next;
    }
    last_node->next = device;

    return OPRT_OK;
}


/**
 * @brief open dimmer
 *
 * @param[inout] device: dimmer node
 * @param[in] app_resolution: the maximum value of the output data that application sets
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
STATIC OPERATE_RET __tdl_dimmer_open(INOUT DIMMER_NODE_T *device, IN USHORT_T app_resolution)
{
    OPERATE_RET op_ret = OPRT_COM_ERROR;

    device->app_resolution = app_resolution;

    op_ret = device->intfs.open(device->drv_handle);
    if (OPRT_OK != op_ret) {
        return op_ret;
    }

    return OPRT_OK;
}


/**
 * @brief close dimmer
 *
 * @param[inout] device: dimmer node
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
STATIC OPERATE_RET __tdl_dimmer_close(INOUT DIMMER_NODE_T *device)
{
    OPERATE_RET op_ret = OPRT_COM_ERROR;

    op_ret = device->intfs.close(device->drv_handle);
    if (OPRT_OK != op_ret) {
        return op_ret;
    }

    return OPRT_OK;
}



/**
 * @brief output data to dimmer driver
 *
 * @param[in] device: dimmer node
 * @param[in] data: data to output
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
STATIC OPERATE_RET __tdl_dimmer_output(IN DIMMER_NODE_T *device, IN LIGHT_RGBCW_U *p_rgbcw)
{
    OPERATE_RET op_ret = OPRT_OK;
    UINT_T i = 0;

    for(i = 0; i < LIGHT_COLOR_CHANNEL_MAX; i++){
        device->output_rgbcw.array[i] = (p_rgbcw->array[i] >= device->app_resolution) ? device->drv_resolution : \
                                       (p_rgbcw->array[i] * device->drv_resolution / device->app_resolution);
    }

    op_ret = device->intfs.output(device->drv_handle, &device->output_rgbcw);

    return op_ret;
}


/**
 * @brief register dimmer
 *
 * @param[in] name: dimmer name
 * @param[in] p_intfs: interfaces for operating the dimmer
 * @param[in] drv_resolution: the resolution of the dimmer driver
 * @param[in] tp: dimmer driver type
 * @param[in] drv_handle: dimmer driver handle
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tdl_dimmer_register(IN CHAR_T *name, IN DIMMER_DRV_INTFS_T *p_intfs, USHORT_T drv_resolution,\
                                DIMMER_DRIVER_TYPE_E tp, IN DIMMER_DRV_HANDLE drv_handle)
{
    if ((NULL == name || NULL == p_intfs || NULL == drv_handle) ||
        (NULL == p_intfs->open || NULL == p_intfs->close || NULL == p_intfs->output)) {
        return OPRT_INVALID_PARM;
    }

    return __tdl_dimmer_register(name, p_intfs, tp, drv_resolution, drv_handle);
}


/**
 * @brief find dimmer
 *
 * @param[in] name: dimmer name
 * @param[out] handle: dimmer handle
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_dimmer_find(IN CHAR_T *name, OUT TDL_DIMMER_HANDLE *handle)
{
    DIMMER_NODE_T *device = NULL;

    device = __tdl_dimmer_find(&sg_dimmer_list_head, name);
    if (NULL == device) {
        return OPRT_COM_ERROR;
    }

    *handle = (TDL_DIMMER_HANDLE)device;

    return OPRT_OK;
}


/**
 * @brief open dimmer
 *
 * @param[in] handle: dimmer handle
 * @param[in] app_resolution: the maximum value of the output data that application sets
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_dimmer_open(IN TDL_DIMMER_HANDLE handle, IN USHORT_T app_resolution)
{
    OPERATE_RET op_ret = OPRT_OK;
    DIMMER_NODE_T *device = NULL;

    if (NULL == handle) {
        return OPRT_INVALID_PARM;
    }

    device = (DIMMER_NODE_T *)handle;

    tal_mutex_lock(device->mutex);

    op_ret = __tdl_dimmer_open(device, app_resolution);

    tal_mutex_unlock(device->mutex);

    return op_ret;
}


/**
 * @brief close dimmer
 *
 * @param[in] handle: dimmer handle
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_dimmer_close(IN TDL_DIMMER_HANDLE handle)
{
    OPERATE_RET op_ret = OPRT_OK;
    DIMMER_NODE_T *device = NULL;

    if (NULL == handle) {
        return OPRT_INVALID_PARM;
    }

    device = (DIMMER_NODE_T *)handle;

    tal_mutex_lock(device->mutex);

    op_ret = __tdl_dimmer_close(device);

    tal_mutex_unlock(device->mutex);

    return op_ret;
}


/**
 * @brief Processing warm in driver cct
 *
 * @param[in] p_src_rgbcw: src rgbcw
 * @param[in] p_target_rgbcw: target rgbcw
 * @param[inout] p_curr_rgbcw: current rgbcw
 * 
 * @return none
 */
VOID __tdl_get_cct_warm(IN LIGHT_RGBCW_T *p_src_rgbcw, IN LIGHT_RGBCW_T *p_target_rgbcw, INOUT LIGHT_RGBCW_T *p_curr_rgbcw)
{
    //cct驱动渐变开关需要特殊处理w值变化
    //1.渐变灭灯:暖光值保持原始值不能变化，调光w等于原始w值
    //2.渐变开灯:暖光值保持目标值不能变化，调光w等于目标w值
    if(0 == p_src_rgbcw->cold && 0 != p_target_rgbcw->cold){
        //白光 0 -> 1
        p_curr_rgbcw->warm = p_target_rgbcw->warm;
    }else if(0 != p_src_rgbcw->cold && 0 == p_target_rgbcw->cold){
        //白光 1 -> 0
        p_curr_rgbcw->warm = p_src_rgbcw->warm;
    }
}


/**
 * @brief output data to dimmer driver
 *
 * @param[in] handle: dimmer handle
 * @param[in] p_rgbcw: set current rgbcw
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_dimmer_output(IN TDL_DIMMER_HANDLE handle, IN LIGHT_RGBCW_T *p_rgbcw)
{
    OPERATE_RET op_ret = OPRT_OK;
    DIMMER_NODE_T *device = NULL;
    LIGHT_RGBCW_T last_rgbcw = {0};

    if (NULL == handle || NULL == p_rgbcw) {
        return OPRT_INVALID_PARM;
    }

    device = (DIMMER_NODE_T *)handle;

    tal_mutex_lock(device->mutex);

    memcpy(&last_rgbcw, &device->rgbcw.s, SIZEOF(LIGHT_RGBCW_T)); 
    memcpy(&device->rgbcw.s, p_rgbcw, SIZEOF(LIGHT_RGBCW_T));

    if(DIMMER_DRIVER_PWM_CCT == device->drv_type){
        __tdl_get_cct_warm(&last_rgbcw, &device->rgbcw.s, &device->rgbcw.s);
    }

    op_ret = __tdl_dimmer_output(device, &device->rgbcw);

    tal_mutex_unlock(device->mutex);

    return op_ret;
}


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
                                        IN LIGHT_RGBCW_T *p_curr_rgbcw, IN LIGHT_RGBCW_T *p_target_rgbcw)
{
    OPERATE_RET op_ret = OPRT_OK;
    DIMMER_NODE_T *device = NULL;

    if (NULL == handle || NULL == p_src_rgbcw || NULL == p_curr_rgbcw || NULL == p_target_rgbcw) {
        return OPRT_INVALID_PARM;
    }

    device = (DIMMER_NODE_T *)handle;

    tal_mutex_lock(device->mutex);

    memcpy(&device->rgbcw.s, p_curr_rgbcw, SIZEOF(LIGHT_RGBCW_T));

    if(DIMMER_DRIVER_PWM_CCT == device->drv_type){
        __tdl_get_cct_warm(p_src_rgbcw, p_target_rgbcw, &device->rgbcw.s);
    }

    op_ret = __tdl_dimmer_output(device, &device->rgbcw);

    tal_mutex_unlock(device->mutex);

    return op_ret;
}


/**
 * @brief get output data
 *
 * @param[in] handle: dimmer handle
 * @param[out] p_rgbcw: current rgbcw
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_dimmer_get_current_rgbcw(IN TDL_DIMMER_HANDLE handle, OUT LIGHT_RGBCW_T *p_rgbcw)
{
    DIMMER_NODE_T *device = NULL;

    device = (DIMMER_NODE_T *)handle;

    if (NULL == handle || NULL == p_rgbcw) {
        return OPRT_INVALID_PARM;
    }

    tal_mutex_lock(device->mutex);

    memcpy(p_rgbcw, &device->rgbcw.s, SIZEOF(LIGHT_RGBCW_T));

    tal_mutex_unlock(device->mutex);

    return OPRT_OK;
}


/**
 * @brief get app resolution
 *
 * @param[in] handle: dimmer handle
 * @param[out] resolution: app resolution
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_dimmer_get_app_resolution(IN TDL_DIMMER_HANDLE handle, OUT USHORT_T *resolution)
{
    DIMMER_NODE_T *device = NULL;

    device = (DIMMER_NODE_T *)handle;

    if (NULL == handle || NULL == resolution) {
        return OPRT_INVALID_PARM;
    }

    tal_mutex_lock(device->mutex);

    *resolution = device->app_resolution;

    tal_mutex_unlock(device->mutex);

    return OPRT_OK;
}
