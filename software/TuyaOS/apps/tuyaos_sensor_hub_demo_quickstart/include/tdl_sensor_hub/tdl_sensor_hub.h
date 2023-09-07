/**
 * @file tdl_sensor_hub.h
 * @brief tdl_sensor_hub module is used to manage sensors
 * @version 0.1
 * @date 2022-09-07
 */

#ifndef __TDL_SENSOR_HUB_H__
#define __TDL_SENSOR_HUB_H__

#include "tdl_sensor_type.h"
#include "tkl_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************macro define************************
***********************************************************/
/**
 * @brief timer interval (ms)
 */
#define SR_SOFT_TM_MIN_INTV_MS  10u     // software timer minimum interval
#define SR_HARD_TM_MIN_INTV_MS  1u      // hardware timer minimum interval

/***********************************************************
***********************typedef define***********************
***********************************************************/
/**
 * @brief acquisition trigger mode
 */
typedef BYTE_T SR_TRIG_MODE_E;
#define SR_MODE_POLL_SOFT_TM    0       // polling (software timer)
#define SR_MODE_POLL_HARD_TM    1       // polling (hardware timer)
#define SR_MODE_EXTI            2       // external interrupt (gpio)

/**
 * @brief element subscription type
 */
typedef BYTE_T SR_ELE_SUB_TP_E;
#define SR_ELE_SUB_TP_GROUP     0       // report all elements in groups
#define SR_ELE_SUB_TP_SINGLE    1       // report each element one by one

/**
 * @brief work mode
 */
typedef struct {
    SR_TRIG_MODE_E      trig_mode;      // acquisition trigger mode
    UINT_T              poll_intv_ms;   // polling interval (sampling time)
    TUYA_GPIO_NUM_E     irq_pin;        // intrrupt pin
    TUYA_GPIO_IRQ_E     irq_mode;       // intrrupt mode
} SR_WORK_MODE_T;

/**
 * @brief element buffer
 */
typedef struct {
    UCHAR_T         id;                 // element ID
    SR_VAL_TP_E     val_tp;             // element value type
    UCHAR_T         val_num;            // number of the element value
    SR_VAL_U       *val;                // element value
} SR_ELE_BUFF_T;

/**
 * @brief data filtering rules
 */
typedef union {
    struct {
        INT_T       max;
        INT_T       min;
        UINT_T      step;
    } i;

    struct {
        FLOAT_T     max;
        FLOAT_T     min;
        FLOAT_T     step;
    } f;
} SR_FILTETR_U;

/**
 * @brief subscription rule
 * @note val_num is invalid when ele_sub_tp is SR_ELE_SUB_TP_GROUP, and it will be set to fifo_size
 */
typedef struct {
    UCHAR_T         id;                 // element ID
    UCHAR_T         val_num;            // number of element value per subscription, must not exceed fifo_size
    SR_FILTETR_U    filter;             // data filtering rules
} SR_ELE_SUB_RULE_T;

/**
 * @brief element subscription configuration
 *        advanced function (optional)
 */
typedef struct {
    SR_ELE_SUB_TP_E     tp;             // element subscription type
    UCHAR_T             num;            // number of elements that application want to subscribe
    SR_ELE_SUB_RULE_T  *rule;           // subscription rule for each element
} SR_ELE_SUB_CFG_T;

/**
 * @brief inform callback prototype
 */
typedef VOID_T (*SR_ELE_INFORM_CB)(CHAR_T* name, UCHAR_T ele_num, SR_ELE_BUFF_T *ele_data);
typedef VOID_T (*SR_RAW_INFORM_CB)(CHAR_T* name, UINT_T raw_num, SR_RAW_DATA_T *raw_data);
typedef union {
    SR_ELE_INFORM_CB    ele;            // for element data ready
    SR_RAW_INFORM_CB    raw;            // for raw data ready
} SR_INFORM_CB_T;

/**
 * @brief device configuration
 */
typedef struct {
    SR_WORK_MODE_T      mode;           // acquisition trigger mode
    SR_INFORM_CB_T      inform_cb;      // inform callback for data ready
    UCHAR_T             fifo_size;      // data buffer size (default: 1)
    SR_ELE_SUB_CFG_T   *ele_sub;        // subscription function for element (NULL means unused)
} SR_DEV_CFG_T;

/**
 * @brief sensor handle type
 */
typedef VOID_T* SENSOR_HANDLE_T;

/***********************************************************
********************function declaration********************
***********************************************************/
/**
 * @brief find sensor device
 *
 * @param[in] name: device name (string)
 * @param[out] handle: device handle
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdl_sensor_dev_find(IN CHAR_T *name, OUT SENSOR_HANDLE_T* handle);

/**
 * @brief open sensor device
 *
 * @param[in] handle: device handle
 * @param[in] config: configuration parameters
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdl_sensor_dev_open(IN SENSOR_HANDLE_T handle, IN SR_DEV_CFG_T *config);

/**
 * @brief close sensor device
 *
 * @param[in] handle: device handle
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdl_sensor_dev_close(IN SENSOR_HANDLE_T handle);

/**
 * @brief configure sensor device
 *
 * @param[in] handle: device handle
 * @param[in] cmd: configuration command
 * @param[inout] param: configuration parameters
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdl_sensor_dev_config(IN SENSOR_HANDLE_T handle, IN UCHAR_T cmd, INOUT VOID_T *param);

/**
 * @brief read real-time data from sensor device
 *
 * @param[in] handle: device handle
 * @param[in] ele_num: number of elements, write 0 when not in use
 * @param[inout] ele_data: data in element type, write NULL when not in use
 * @param[out] raw_data: data in raw type, write NULL when not in use
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdl_sensor_dev_read(IN SENSOR_HANDLE_T handle, IN UCHAR_T ele_num,
                                INOUT SR_ELE_DATA_T *ele_data, OUT SR_RAW_DATA_T *raw_data);

#ifdef __cplusplus
}
#endif

#endif /* __TDL_SENSOR_HUB_H__ */
