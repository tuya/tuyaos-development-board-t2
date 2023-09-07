/**
 * @file tdd_sensor_voltage.c
 * @brief tdd_sensor_voltage module is used to drive voltage sensor
 * @version 0.1
 * @date 2022-08-15
 */

#include "tdl_sensor_driver.h"
#include "tdd_sensor_voltage.h"
#include "tkl_adc.h"
#include "tkl_memory.h"
#include "tal_log.h"

/***********************************************************
************************macro define************************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
********************function declaration********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
/**
 * @brief element value type
 */
STATIC SR_ELE_CFG_T sg_volt_ele_val_tp[SR_VOLT_ELE_NUM] = {
    {
        .id     = SR_VOLT_ELE_ID_VOLT,
        .val_tp = SR_VAL_TP_FLOAT,
    }
};

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief read voltage data
 *
 * @param[in] dev: device resource
 * @param[out] data: voltage data
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __read_volt_data(IN SR_RSRC_T *dev, OUT FLOAT_T *data)
{
    OPERATE_RET op_ret = OPRT_OK;
    FLOAT_T scale = 0.0f;
    INT_T pin_vol = 0;

    op_ret = tkl_adc_read_voltage(dev->port, &pin_vol, 1);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("Read voltage failed, op_ret: %d", op_ret);
        return op_ret;
    }

    tkl_system_memcpy(&scale, dev->info, SIZEOF(FLOAT_T));
    *data = pin_vol * scale / 1000.0f;

    return OPRT_OK;
}

/**
 * @brief open voltage device
 *
 * @param[in] dev: device resource
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdd_sensor_voltage_open(IN SR_RSRC_T *dev)
{
    return OPRT_OK;
}

/**
 * @brief close voltage device
 *
 * @param[in] dev: device resource
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdd_sensor_voltage_close(IN SR_RSRC_T *dev)
{
    return OPRT_OK;
}

/**
 * @brief control voltage device
 *
 * @param[in] dev: device resource
 * @param[in] cmd: control command
 * @param[in] param: command parameter, data type depends on the specified command
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdd_sensor_voltage_control(IN SR_RSRC_T* dev, IN UCHAR_T cmd, IN VOID_T *param)
{
    return OPRT_OK;
}

/**
 * @brief read data from voltage device
 *
 * @param[in] dev: device
 * @param[out] ele_data: element data
 * @param[in] ele_num: number of elements
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdd_sensor_voltage_read(IN SR_RSRC_T* dev, OUT SR_ELE_DATA_T *ele_data, IN UCHAR_T ele_num)
{
    for (UCHAR_T i = 0; i < ele_num; i++) {
        switch (ele_data[i].id) {
        case SR_VOLT_ELE_ID_VOLT:
            __read_volt_data(dev, &ele_data[i].val.sr_float);
            break;
        default:
            TAL_PR_ERR("Element ID: %d is invalid.", ele_data[i].id);
            break;
        }
    }
    return OPRT_OK;
}

/**
 * @brief register voltage
 *
 * @param[in] name: device name
 * @param[in] adc_port: adc port number
 * @param[in] adc_cfg: adc base configuration
 * @param[in] scale: partial pressure coefficient
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdd_sensor_voltage_register(IN CHAR_T *name, IN TUYA_ADC_NUM_E adc_port,
                                        IN TUYA_ADC_BASE_CFG_T *adc_cfg, IN FLOAT_T scale)
{
    // check param
    if (NULL == name || NULL == adc_cfg) {
        return OPRT_INVALID_PARM;
    }

    // ADC init
    tkl_adc_init(adc_port, adc_cfg);

    // save resourse information
    SR_RSRC_T resource = {0};
    resource.port = adc_port;
    resource.handle = NULL;
    tkl_system_memcpy(resource.info, &scale, SIZEOF(FLOAT_T));

    // define driver interfaces
    STATIC SR_INTFS_T s_intfs;
    s_intfs.open = __tdd_sensor_voltage_open;
    s_intfs.close = __tdd_sensor_voltage_close;
    s_intfs.control = __tdd_sensor_voltage_control;
    s_intfs.read_ele = __tdd_sensor_voltage_read;

    // register the device
    OPERATE_RET op_ret = tdl_sensor_register(name, &s_intfs, SR_VOLT_ELE_NUM, sg_volt_ele_val_tp, &resource);
    if (OPRT_OK != op_ret) {
        return op_ret;
    }

    return OPRT_OK;
}
