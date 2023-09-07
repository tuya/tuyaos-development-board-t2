/**
 * @file tdl_sensor_hub.c
 * @brief tdl_sensor_hub module is used to manage sensors
 * @version 0.1
 * @date 2022-09-07
 */

#include "tdl_sensor_hub.h"
#include "tdl_sensor_driver.h"
#include "tkl_semaphore.h"
#include "tkl_mutex.h"
#include "tkl_thread.h"
#include "tkl_memory.h"
#include "tkl_timer.h"
#include "tal_sw_timer.h"
#include "tal_log.h"

/***********************************************************
************************macro define************************
***********************************************************/
/**
 * @brief filter judgement
 *        max and min are both 0 for unlimited range
 */
#define SR_IS_RANG_ERR(max, min)                   (((max)<=(min)) && ((min)!=0||(min)!=0))
#define SR_IS_VAL_OVER_RANG(val, max, min)         (((val)>(max)||(val)<(min)) && ((min)!=0||(min)!=0))
#define SR_IS_VAL_DIFF_IN_STEP(v1, v2, step)       (((v1)<((v2)+(step))) && ((v1)>((v2)-(step))))

/***********************************************************
***********************typedef define***********************
***********************************************************/
/**
 * @brief sensor data type
 */
typedef BYTE_T SR_DATA_TP_E;
#define SR_DATA_TP_ELE  0
#define SR_DATA_TP_RAW  1

/**
 * @brief flag for sensor hub
 */
typedef union {
    struct {
        UCHAR_T is_start        : 1;
        UCHAR_T is_need_acquire : 1;
        UCHAR_T is_irq_occur    : 1;
        UCHAR_T reserved        : 5;
    };
    UCHAR_T whole;
} SR_FLAG_U;

/**
 * @brief param for work mode
 */
typedef union {
    struct {
        TUYA_GPIO_NUM_E irq_pin;
        TUYA_GPIO_IRQ_E irq_mode;
    } exti;
    struct {
        UINT_T          intv_ms;
        UINT_T          run_time_ms;
    } poll;
} SR_WORK_PARAM_T;

/**
 * @brief fifo buffer
 */
typedef union {
    SR_ELE_BUFF_T      *ele;
    struct {
        UINT_T          len;
        SR_RAW_DATA_T  *data;
    } raw;
} SR_APP_DATA_FIFO_U;

/**
 * @brief device configuration list
 */
typedef struct sensor_list {
    struct sensor_list *next;

    CHAR_T              name[SR_DEV_NAME_MAX_LEN+1];
    TKL_MUTEX_HANDLE    mutex;
    SR_FLAG_U           flag;
    SR_WORK_PARAM_T     work_param;
    SR_APP_DATA_FIFO_U  app_fifo;

    UCHAR_T             ele_num;
    SR_ELE_CFG_T       *ele_info;
    SR_RSRC_T           resource;
    SR_INTFS_T         *intfs;

    SR_TRIG_MODE_E      trig_mode;
    SR_INFORM_CB_T      inform_cb;
    UCHAR_T             fifo_size;
    SR_ELE_SUB_CFG_T   *ele_sub;

} SR_DEV_NODE_T, SR_DEV_LIST_T;

/***********************************************************
********************function declaration********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
/**
 * @brief device list
 */
STATIC SR_DEV_LIST_T sg_sr_dev_list_head = {
    .next = NULL,
};

/**
 * @brief thread
 */
STATIC TKL_THREAD_HANDLE sg_sensor_thrd = NULL;
STATIC TKL_SEM_HANDLE sg_task_sem;

/**
 * @brief timer handle
 */
STATIC TIMER_ID sg_sr_soft_timer;
STATIC TUYA_TIMER_NUM_E sg_sr_hard_timer;

/**
 * @brief timer counter
 */
STATIC UINT_T sg_soft_tm_cnt = 0;
STATIC volatile UINT_T sg_hard_tm_cnt = 0;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief get data type
 *
 * @param[in] ele_num: number of elements
 *
 * @return data type
 */
STATIC SR_DATA_TP_E __tdl_sensor_get_data_tp(IN UCHAR_T ele_num)
{
    return (ele_num == 0 ? SR_DATA_TP_RAW : SR_DATA_TP_ELE);
}

/**
 * @brief find device node
 *
 * @param[in] list: device list
 * @param[in] name: device name (string)
 *
 * @return device node on found, NULL on unfound
 */
STATIC SR_DEV_NODE_T *__tdl_sensor_find_dev_node(IN SR_DEV_LIST_T *list, IN CHAR_T *name)
{
    SR_DEV_NODE_T *node = NULL;

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
 * @brief register sensor
 *
 * @param[in] name: device name
 * @param[in] intfs: interface for operating the sensor
 * @param[in] ele_num: number of elements, 0 means the element type is raw
 * @param[in] ele_cfg: data type configuration for each element
 * @param[in] resource: resource that device depends on
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdl_sensor_register(IN CHAR_T *name, IN SR_INTFS_T *intfs, IN UCHAR_T ele_num,
                                         IN SR_ELE_CFG_T *ele_cfg, IN SR_RSRC_T *resource)
{
    OPERATE_RET op_ret = OPRT_OK;
    SR_DEV_NODE_T *device = NULL;
    SR_DEV_NODE_T *last_node = NULL;

    // check if the device is already registered
    if (NULL != __tdl_sensor_find_dev_node(&sg_sr_dev_list_head, name)) {
        TAL_PR_ERR("The device %s already exists !", name);
        return OPRT_COM_ERROR;
    }

    // malloc for this new device
    device = (SR_DEV_NODE_T *)tkl_system_malloc(SIZEOF(SR_DEV_NODE_T));
    if (NULL == device) {
        TAL_PR_ERR("Malloc for %s failed !", name);
        return OPRT_MALLOC_FAILED;
    }
    tkl_system_memset(device, 0, SIZEOF(SR_DEV_NODE_T));

    // save the name, resource and interface
    strncpy(device->name, name, SR_DEV_NAME_MAX_LEN);
    tkl_system_memcpy(&device->resource, resource, SIZEOF(SR_RSRC_T));
    device->intfs = intfs;

    // judge data type and save base information
    device->ele_num = ele_num;
    if (SR_DATA_TP_ELE == __tdl_sensor_get_data_tp(ele_num)) {
        device->ele_info = ele_cfg;
    }

    // create mutex
    op_ret = tkl_mutex_create_init(&device->mutex);
    if (OPRT_OK != op_ret) {
        tkl_system_free(device);
        device = NULL;
        return op_ret;
    }

    // find the last node and add this new device
    last_node = &sg_sr_dev_list_head;
    while (last_node->next) {
        last_node = last_node->next;
    }
    last_node->next = device;

    return OPRT_OK;
}

/**
 * @brief check application configuration
 *
 * @param[in] device: device node
 * @param[inout] config: device configuration
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdl_sensor_check_config(IN SR_DEV_NODE_T *device, INOUT SR_DEV_CFG_T *config)
{
    UCHAR_T i, j;

    // mode
    if (config->mode.trig_mode > SR_MODE_EXTI) {
        TAL_PR_ERR("mode.trig_mode: %d is undefined !", config->mode.trig_mode);
        return OPRT_INVALID_PARM;
    }
    if ((SR_MODE_POLL_SOFT_TM == config->mode.trig_mode && config->mode.poll_intv_ms < SR_SOFT_TM_MIN_INTV_MS) ||
        (SR_MODE_POLL_HARD_TM == config->mode.trig_mode && config->mode.poll_intv_ms < SR_HARD_TM_MIN_INTV_MS)) {
        TAL_PR_ERR("mode.poll_intv_ms: %d is too short !", config->mode.poll_intv_ms);
        return OPRT_INVALID_PARM;
    }

    // inform_cb
    if ((SR_DATA_TP_ELE == __tdl_sensor_get_data_tp(device->ele_num) && NULL == config->inform_cb.ele) ||
        (SR_DATA_TP_RAW == __tdl_sensor_get_data_tp(device->ele_num) && NULL == config->inform_cb.raw)) {
        TAL_PR_ERR("inform_cb is null !");
        return OPRT_INVALID_PARM;
    }

    // fifo_size
    if (0 == config->fifo_size) {
        TAL_PR_NOTICE("fifo_size cannot be 0, it has been corrected to the default value of 1 !");
        config->fifo_size = 1;
    }

    // ele_sub
    if (NULL == config->ele_sub) {
        return OPRT_OK;
    } else {
        if (SR_DATA_TP_ELE != __tdl_sensor_get_data_tp(device->ele_num)) {
            TAL_PR_NOTICE("ele_sub is only valid for SR_DATA_TP_ELE, it has been corrected to NULL !");
            config->ele_sub = NULL;
            return OPRT_OK;
        }
    }

    if (SR_ELE_SUB_TP_SINGLE < config->ele_sub->tp) {
        TAL_PR_ERR("ele_sub->tp: %d is undefined !", config->ele_sub->tp);
        return OPRT_INVALID_PARM;
    }
    if (0 == config->ele_sub->num) {
        TAL_PR_ERR("ele_sub->num cannot be 0 !");
        return OPRT_INVALID_PARM;
    }
    if (NULL == config->ele_sub->rule) {
        TAL_PR_ERR("ele_sub->rule cannot be NULL !");
        return OPRT_INVALID_PARM;
    }

    for (i = 0; i < config->ele_sub->num; i++) {
        // id
        for (j = 0; j < device->ele_num; j++) {
            if (device->ele_info[j].id == config->ele_sub->rule[i].id) {
                break;
            }
        }
        if (j >= device->ele_num) {
            TAL_PR_ERR("ele_sub->rule[%d].id is invalid !", i);
            return OPRT_INVALID_PARM;
        }

        // val_num
        if (SR_ELE_SUB_TP_SINGLE == config->ele_sub->tp) {
            if (config->ele_sub->rule[i].val_num > config->fifo_size) {
                TAL_PR_ERR("ele_sub->rule[%d].val_num is over fifo_size !", i);
                return OPRT_INVALID_PARM;
            }
            if (0 == config->ele_sub->rule[i].val_num) {
                config->ele_sub->rule[i].val_num = 1;
                TAL_PR_NOTICE("val_num cannot be 0, ele_sub->rule[%d].val_num has been \
                              corrected to the default value of 1 !", i);
            }
        } else {
            config->ele_sub->rule[i].val_num = config->fifo_size;
        }

        // filter
        if (SR_VAL_TP_INT == device->ele_info[j].val_tp) {
            if (SR_IS_RANG_ERR(config->ele_sub->rule[i].filter.i.max, config->ele_sub->rule[i].filter.i.min)) {
                TAL_PR_ERR("The setting of ele_sub->rule[%d].filter is not reasonable !", i);
                return OPRT_INVALID_PARM;
            }
        } else {
            if (SR_IS_RANG_ERR(config->ele_sub->rule[i].filter.f.max, config->ele_sub->rule[i].filter.f.min)) {
                TAL_PR_ERR("The setting of ele_sub->rule[%d].filter is not reasonable !", i);
                return OPRT_INVALID_PARM;
            }
        }
    }

    return OPRT_OK;
}

/**
 * @brief load application configuration
 *
 * @param[in] device: device node
 * @param[in] config: device configuration
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdl_sensor_load_config(IN SR_DEV_NODE_T *device, IN SR_DEV_CFG_T *config)
{
    // mode
    device->trig_mode = config->mode.trig_mode;
    if (SR_MODE_EXTI == config->mode.trig_mode) {
        device->work_param.exti.irq_pin = config->mode.irq_pin;
        device->work_param.exti.irq_mode = config->mode.irq_mode;
    } else {
        device->work_param.poll.intv_ms = config->mode.poll_intv_ms;
    }

    // inform_cb
    if (SR_DATA_TP_ELE == __tdl_sensor_get_data_tp(device->ele_num)) {
        device->inform_cb.ele = config->inform_cb.ele;
    } else {
        device->inform_cb.raw = config->inform_cb.raw;
    }

    // fifo_size
    device->fifo_size = config->fifo_size;

    // ele_sub
    if (NULL == config->ele_sub) {
        return OPRT_OK;
    }

    if (NULL != device->ele_sub) {
        if (NULL != device->ele_sub->rule) {
            tkl_system_free(device->ele_sub->rule);
            device->ele_sub->rule = NULL;
        }
        tkl_system_free(device->ele_sub);
        device->ele_sub = NULL;
    }

    device->ele_sub = (SR_ELE_SUB_CFG_T *)tkl_system_malloc(SIZEOF(SR_ELE_SUB_CFG_T));
    if (NULL == device->ele_sub) {
        TAL_PR_ERR("Malloc for ele_sub failed !");
        return OPRT_MALLOC_FAILED;
    }
    tkl_system_memset(device->ele_sub, 0, SIZEOF(SR_ELE_SUB_CFG_T));

    UINT_T rule_size = SIZEOF(SR_ELE_SUB_RULE_T) * config->ele_sub->num;
    device->ele_sub->rule = (SR_ELE_SUB_RULE_T *)tkl_system_malloc(rule_size);
    if (NULL == device->ele_sub->rule) {
        TAL_PR_ERR("Malloc for rule failed !");
        tkl_system_free(device->ele_sub);
        device->ele_sub = NULL;
        return OPRT_MALLOC_FAILED;
    }

    device->ele_sub->tp = config->ele_sub->tp;
    device->ele_sub->num = config->ele_sub->num;
    tkl_system_memcpy(device->ele_sub->rule, config->ele_sub->rule, rule_size);

    return OPRT_OK;
}

/**
 * @brief soft timer callback function
 *
 * @param[in] timer_id: timer ID
 * @param[in] arg: argument
 *
 * @return none
 */
STATIC VOID_T __tdl_sensor_soft_timer_cb(IN TIMER_ID timer_id, IN VOID_T *arg)
{
    sg_soft_tm_cnt += SR_SOFT_TM_MIN_INTV_MS;
    tkl_semaphore_post(sg_task_sem);
    return;
}

/**
 * @brief hard timer callback function
 *
 * @param[in] args: arguments
 *
 * @return none
 */
STATIC VOID_T __tdl_sensor_hard_timer_cb(IN VOID_T *args)
{
    sg_hard_tm_cnt += SR_HARD_TM_MIN_INTV_MS;
    tkl_semaphore_post(sg_task_sem);
    return;
}

/**
 * @brief external interrupt callback
 *
 * @param[in] args: arguments
 *
 * @return none
 */
STATIC VOID_T __tdl_sensor_exti_cb(IN VOID_T *args)
{
    SR_DEV_NODE_T *device = sg_sr_dev_list_head.next;
    TUYA_GPIO_NUM_E *pin = (TUYA_GPIO_NUM_E *)args;

    while (device != NULL) {
        if (FALSE == device->flag.is_start ||
            device->trig_mode != SR_MODE_EXTI) {
            device = device->next;
            continue;
        }

        if (device->work_param.exti.irq_pin == *pin) {
            device->flag.is_irq_occur = TRUE;
            break;
        }

        device = device->next;
    }

    tkl_semaphore_post(sg_task_sem);

    return;
}

/**
 * @brief add and start a soft timer for tdl_sensor_hub
 *
 * @param none
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdl_sensor_soft_timer_init(VOID_T)
{
    STATIC BOOL_T s_is_soft_timer_init = FALSE;

    if (s_is_soft_timer_init) {
        return OPRT_OK;
    }

    OPERATE_RET op_ret;
    op_ret = tal_sw_timer_create(__tdl_sensor_soft_timer_cb, NULL, &sg_sr_soft_timer);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("Failed to init sg_sr_soft_timer, error code: %d !", op_ret);
        return op_ret;
    }
    op_ret = tal_sw_timer_start(sg_sr_soft_timer, SR_SOFT_TM_MIN_INTV_MS, TAL_TIMER_CYCLE);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("Failed to start sg_sr_soft_timer, error code: %d !", op_ret);
        return op_ret;
    }

    s_is_soft_timer_init = TRUE;
    TAL_PR_INFO("Software timer for tdl_sensor_hub has started.");

    return OPRT_OK;
}

/**
 * @brief add and start a hard timer for tdl_sensor_hub
 *
 * @param none
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdl_sensor_hard_timer_init(VOID_T)
{
    STATIC BOOL_T s_is_hard_timer_init = FALSE;

    if (s_is_hard_timer_init) {
        return OPRT_OK;
    }

    OPERATE_RET op_ret;
    TUYA_TIMER_NUM_E i;
    TUYA_TIMER_BASE_CFG_E cfg;
    cfg.mode = TUYA_TIMER_MODE_PERIOD;
    cfg.cb = __tdl_sensor_hard_timer_cb;
    cfg.args = NULL;

    for (i = 0; i < TIMER_NUM_MAX; i++) {
        op_ret = tkl_timer_init(i, &cfg);
        if (OPRT_OK == op_ret) {
            sg_sr_hard_timer = i;
            TAL_PR_INFO("TIMER %d will be used as sg_sr_hard_timer.", i);
            break;
        }
    }
    if (i >= TIMER_NUM_MAX) {
        TAL_PR_ERR("Failed to init sg_sr_hard_timer, error code !");
        return OPRT_NOT_FOUND;
    }

    op_ret = tkl_timer_start(sg_sr_hard_timer, SR_HARD_TM_MIN_INTV_MS * 1000);
    if (op_ret != OPRT_OK) {
        TAL_PR_ERR("Failed to start sg_sr_hard_timer, error code: %d !", op_ret);
        return op_ret;
    }

    s_is_hard_timer_init = TRUE;
    TAL_PR_INFO("Hardware timer for tdl_sensor_hub has started.");

    return OPRT_OK;
}

/**
 * @brief add and start a exti for tdl_sensor_hub
 *
 * @param[in] device: device node
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdl_sensor_exti_init(IN SR_DEV_NODE_T *device)
{
    OPERATE_RET op_ret;

    TUYA_GPIO_IRQ_T irq;
    irq.mode = device->work_param.exti.irq_mode;
    irq.cb = __tdl_sensor_exti_cb;
    irq.arg = &device->work_param.exti.irq_pin;

    op_ret = tkl_gpio_irq_init(device->work_param.exti.irq_pin, &irq);
    if (op_ret != OPRT_OK) {
        TAL_PR_ERR("Failed to init interrupt of P%d, error code: %d !", device->work_param.exti.irq_pin, op_ret);
        return op_ret;
    }

    op_ret = tkl_gpio_irq_enable(device->work_param.exti.irq_pin);
    if (op_ret != OPRT_OK) {
        TAL_PR_ERR("Failed to enable interrupt, error code: %d !", op_ret);
        return op_ret;
    }

    TAL_PR_INFO("P%d interrupt for tdl_sensor_hub has enabled.", device->work_param.exti.irq_pin);

    return OPRT_OK;
}

/**
 * @brief data acquisition trigger init
 *
 * @param[in] device: device node
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdl_sensor_daq_trig_init(IN SR_DEV_NODE_T *device)
{
    OPERATE_RET op_ret = OPRT_OK;

    switch (device->trig_mode) {
    default:
    case SR_MODE_POLL_SOFT_TM:
        op_ret = __tdl_sensor_soft_timer_init();
        break;
    case SR_MODE_POLL_HARD_TM:
        op_ret = __tdl_sensor_hard_timer_init();
        break;
    case SR_MODE_EXTI:
        op_ret = __tdl_sensor_exti_init(device);
        break;
    }

    return op_ret;
}

/**
 * @brief prepare application fifo buffer
 *
 * @param[in] device: device node
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdl_sensor_prepare_app_fifo(IN SR_DEV_NODE_T *device)
{
    UINT_T total_size = 0;
    UCHAR_T num = device->fifo_size;

    if (SR_DATA_TP_ELE == __tdl_sensor_get_data_tp(device->ele_num)) {
        UCHAR_T ele_num;
        if (NULL == device->ele_sub) {
            ele_num = device->ele_num;
        } else {
            ele_num = device->ele_sub->num;
        }

        if (NULL != device->app_fifo.ele) {
            tkl_system_free(device->app_fifo.ele);
            device->app_fifo.ele = NULL;
        }
        total_size = (SIZEOF(SR_ELE_BUFF_T) + SIZEOF(SR_VAL_U) * num) * ele_num;
        device->app_fifo.ele = (SR_ELE_BUFF_T *)tkl_system_malloc(total_size);
        if (NULL == device->app_fifo.ele) {
            return OPRT_MALLOC_FAILED;
        }
        tkl_system_memset(device->app_fifo.ele, 0, total_size);

        SR_VAL_U *val_buf = (SR_VAL_U *)&(device->app_fifo.ele[ele_num]);
        for (UCHAR_T i = 0; i < ele_num; i++) {
            if (NULL == device->ele_sub) {
                device->app_fifo.ele[i].id = device->ele_info[i].id;
                device->app_fifo.ele[i].val_tp = device->ele_info[i].val_tp;
            } else {
                device->app_fifo.ele[i].id = device->ele_sub->rule[i].id;
                for (UCHAR_T j = 0; j < device->ele_num; j++) {
                    if (device->app_fifo.ele[i].id == device->ele_info[j].id) {
                        device->app_fifo.ele[i].val_tp = device->ele_info[j].val_tp;
                    }
                }
            }
            device->app_fifo.ele[i].val = &val_buf[i*num];
        }
    } else {
        if (NULL != device->app_fifo.raw.data) {
            tkl_system_free(device->app_fifo.raw.data);
            device->app_fifo.raw.data = NULL;
        }
        total_size = SIZEOF(SR_RAW_DATA_T) * num;
        device->app_fifo.raw.data = (SR_RAW_DATA_T *)tkl_system_malloc(total_size);
        if (NULL == device->app_fifo.raw.data) {
            return OPRT_MALLOC_FAILED;
        }
        tkl_system_memset(device->app_fifo.raw.data, 0, total_size);
    }

    return OPRT_OK;
}

/**
 * @brief open sensor device
 *
 * @param[inout] device: device node
 * @param[in] config: device configuration
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdl_sensor_dev_open(INOUT SR_DEV_NODE_T *device, IN SR_DEV_CFG_T* config)
{
    OPERATE_RET op_ret;

    // load configuration from application
    op_ret = __tdl_sensor_load_config(device, config);
    if (OPRT_OK != op_ret) {
        return op_ret;
    }

    // prepare fifo buffer for application
    op_ret = __tdl_sensor_prepare_app_fifo(device);
    if (OPRT_OK != op_ret) {
        return op_ret;
    }

    // open sensor hub
    op_ret = __tdl_sensor_daq_trig_init(device);
    if (OPRT_OK != op_ret) {
        return op_ret;
    }

    // open device (tdd sensor interface)
    op_ret = device->intfs->open(&device->resource);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("tdd_sensor_dev_open error: %d !", op_ret);
        return op_ret;
    }

    // mark "device is start"
    device->flag.is_start = TRUE;

    return OPRT_OK;
}

/**
 * @brief close sensor device
 *
 * @param[inout] device: device node
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdl_sensor_dev_close(INOUT SR_DEV_NODE_T *device)
{
    OPERATE_RET op_ret = OPRT_OK;

    // close device (tdd sensor interface)
    op_ret = device->intfs->close(&device->resource);
    if (OPRT_OK != op_ret) {
        return op_ret;
    }

    // clear opreation param
    device->flag.whole = 0x00;
    tkl_system_memset(&device->work_param, 0, SIZEOF(SR_WORK_PARAM_T));

    // release app_fifo
    if (SR_DATA_TP_ELE == __tdl_sensor_get_data_tp(device->ele_num)) {
        tkl_system_free(device->app_fifo.ele);
        device->app_fifo.ele = NULL;
    } else {
        tkl_system_free(device->app_fifo.raw.data);
        device->app_fifo.raw.data = NULL;
        device->app_fifo.raw.len = 0;
    }

    // clear app config
    device->trig_mode = 0;
    device->fifo_size = 0;
    device->inform_cb.ele = NULL;
    device->inform_cb.raw = NULL;
    if (NULL != device->ele_sub) {
        tkl_system_free(device->ele_sub->rule);
        device->ele_sub->rule = NULL;
        tkl_system_free(device->ele_sub);
        device->ele_sub = NULL;
    }

    return OPRT_OK;
}

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
OPERATE_RET __tdl_sensor_dev_read(IN SR_DEV_NODE_T *device, IN UCHAR_T ele_num,
                                  INOUT SR_ELE_DATA_T *ele_data, OUT SR_RAW_DATA_T *raw_data)
{
    OPERATE_RET op_ret = OPRT_OK;

    if (!device->flag.is_start) {
        return OPRT_COM_ERROR;
    }

    UCHAR_T i, j;
    if (SR_DATA_TP_ELE == __tdl_sensor_get_data_tp(device->ele_num)) {
        if (ele_num == device->ele_num) {
            // default
            for (i = 0; i < ele_num; i++) {
                ele_data[i].id = device->ele_info[i].id;
            }
        } else {
            // check element id
            for (i = 0; i < ele_num; i++) {
                for (j = 0; j < device->ele_num; j++) {
                    if (ele_data[i].id == device->ele_info[j].id) {
                        break;
                    }
                }
                if (j >= device->ele_num) {
                    TAL_PR_ERR("ele_data[%d].id is invalid !", i);
                    return OPRT_INVALID_PARM;
                }
            }
        }
        // read element data
        op_ret = device->intfs->read_ele(&device->resource, ele_data, ele_num);
    } else {
        // read raw data
        op_ret = device->intfs->read_raw(&device->resource, raw_data);
    }

    return op_ret;
}

/**
 * @brief timer polling processing
 *
 * @param[in] soft_tm_cnt: software timer count
 * @param[in] hard_tm_cnt: hardware timer count
 *
 * @return none
 */
STATIC VOID_T __tdl_sensor_timer_proc(IN UINT_T soft_tm_cnt, IN UINT_T hard_tm_cnt)
{
    if (0 == soft_tm_cnt && 0 == hard_tm_cnt) {
        return;
    }

    SR_DEV_NODE_T *device = sg_sr_dev_list_head.next;
    while (device != NULL) {
        if (FALSE == device->flag.is_start ||
            device->trig_mode > SR_MODE_POLL_HARD_TM) {
            device = device->next;
            continue;
        }

        tkl_mutex_lock(device->mutex);
        device->work_param.poll.run_time_ms += (device->trig_mode == SR_MODE_POLL_SOFT_TM) ? soft_tm_cnt : hard_tm_cnt;
        if (device->work_param.poll.run_time_ms >= device->work_param.poll.intv_ms) {
            device->flag.is_need_acquire = TRUE;
            device->work_param.poll.run_time_ms = 0;
        }
        tkl_mutex_unlock(device->mutex);

        device = device->next;
    }
}

/**
 * @brief timer count processing processing
 *
 * @param none
 *
 * @return none
 */
STATIC VOID_T __tdl_sensor_exti_proc(VOID_T)
{
    SR_DEV_NODE_T *device = sg_sr_dev_list_head.next;

    while (device != NULL) {
        if (FALSE == device->flag.is_start ||
            SR_MODE_EXTI != device->trig_mode) {
            device = device->next;
            continue;
        }

        if (TRUE == device->flag.is_irq_occur) {
            tkl_mutex_lock(device->mutex);
            device->flag.is_irq_occur = FALSE;
            device->flag.is_need_acquire = TRUE;
            tkl_mutex_unlock(device->mutex);
        }

        device = device->next;
    }
}

/**
 * @brief is need filter
 *
 * @param[in] cur_val: current value
 * @param[in] prv_val: previous value
 * @param[in] rule: filter rule
 * @param[in] val_tp: value type
 *
 * @return TRUE or FALSE.
 */
STATIC BOOL_T __tdl_sensor_is_need_filter(IN SR_VAL_U cur_val, IN SR_VAL_U prv_val,
                                          IN SR_FILTETR_U *rule, IN SR_VAL_TP_E val_tp)
{
    if (SR_VAL_TP_INT == val_tp) {
        if (SR_IS_VAL_OVER_RANG(cur_val.sr_int, rule->i.max, rule->i.min)) {
            TAL_PR_DEBUG("value: %d is over range", cur_val.sr_int);
            return TRUE;
        }

        if (SR_IS_VAL_DIFF_IN_STEP(cur_val.sr_int, prv_val.sr_int, rule->i.step)) {
            TAL_PR_DEBUG("value: %d is in step", cur_val.sr_int);
            return TRUE;
        }
    } else {
        if (SR_IS_VAL_OVER_RANG(cur_val.sr_float, rule->f.max, rule->f.min)) {
            TAL_PR_DEBUG("value: %f is over range", cur_val.sr_float);
            return TRUE;
        }

        if (SR_IS_VAL_DIFF_IN_STEP(cur_val.sr_float, prv_val.sr_float, rule->f.step)) {
            TAL_PR_DEBUG("value: %f is in step", cur_val.sr_float);
            return TRUE;
        }
    }

    return FALSE;
}

/**
 * @brief load element value
 *
 * @param[inout] device: device node
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdl_sensor_load_ele_value(INOUT SR_DEV_NODE_T *device)
{
    OPERATE_RET op_ret = OPRT_OK;
    SR_ELE_DATA_T *ele_data = NULL;
    UCHAR_T *is_need_filter = NULL;
    UCHAR_T val_num = 0;
    UCHAR_T i, ele_num;

    if (NULL == device->ele_sub) {
        ele_num = device->ele_num;
    } else {
        ele_num = device->ele_sub->num;
    }

    // prepare buffer
    ele_data = (SR_ELE_DATA_T *)tkl_system_malloc(SIZEOF(SR_ELE_DATA_T) * ele_num);
    if (NULL == ele_data) {
        op_ret = OPRT_MALLOC_FAILED;
        goto LOAD_ELE_END;
    }
    tkl_system_memset(ele_data, 0, SIZEOF(SR_ELE_DATA_T)*ele_num);
    for (i = 0; i < ele_num; i++) {
        ele_data[i].id = device->app_fifo.ele[i].id;
    }

    // read element data from device (tdd sensor interface)
    op_ret = device->intfs->read_ele(&device->resource, ele_data, ele_num);
    device->flag.is_need_acquire = FALSE;
    if (op_ret != OPRT_OK) {
        TAL_PR_ERR("device: %s -> read ele_data failed !", device->name);
        goto LOAD_ELE_END;
    }

    // filter
    if (SR_ELE_SUB_TP_SINGLE == device->ele_sub->tp) {
        is_need_filter = tkl_system_malloc(ele_num);
        if (NULL == is_need_filter) {
            op_ret = OPRT_MALLOC_FAILED;
            goto LOAD_ELE_END;
        }
        tkl_system_memset(is_need_filter, 0, ele_num);
    }

    if (NULL != device->ele_sub) {
        for (i = 0; i < ele_num; i++) {
            if (device->app_fifo.ele[i].val_num > 0) {
                val_num = device->app_fifo.ele[i].val_num - 1;
            }
            if (__tdl_sensor_is_need_filter(ele_data[i].val,
                                            device->app_fifo.ele[i].val[val_num],
                                            &device->ele_sub->rule[i].filter,
                                            device->app_fifo.ele[i].val_tp)) {
                if (SR_ELE_SUB_TP_GROUP == device->ele_sub->tp) {
                    TAL_PR_DEBUG("all ele_data need filter");
                    goto LOAD_ELE_END;
                } else {
                    TAL_PR_DEBUG("ele_id: %d data need filter", ele_data[i].id);
                    is_need_filter[i] = TRUE;
                }
            }
        }
    }

    // app fifo
    for (i = 0; i < ele_num; i++) {
        if (NULL == device->ele_sub || SR_ELE_SUB_TP_GROUP == device->ele_sub->tp ||
            (SR_ELE_SUB_TP_SINGLE == device->ele_sub->tp && FALSE == is_need_filter[i])) {
            val_num = device->app_fifo.ele[i].val_num;
            tkl_system_memcpy(&device->app_fifo.ele[i].val[val_num], &ele_data[i].val, SIZEOF(SR_VAL_U));
            device->app_fifo.ele[i].val_num++;
        }
    }

LOAD_ELE_END:
    if (NULL != ele_data) {
        tkl_system_free(ele_data);
        ele_data = NULL;
    }
    if (NULL != is_need_filter) {
        tkl_system_free(is_need_filter);
        is_need_filter = NULL;
    }
    return op_ret;
}

/**
 * @brief inform element value to application
 *
 * @param[inout] device: device node
 *
 * @return none
 */
STATIC VOID_T __tdl_sensor_inform_ele_value(INOUT SR_DEV_NODE_T *device)
{
    UINT_T i = 0;
    UCHAR_T ele_num;

    if (NULL == device->ele_sub) {
        ele_num = device->ele_num;
    } else {
        ele_num = device->ele_sub->num;
    }

    if (NULL == device->ele_sub ||
        (NULL != device->ele_sub && SR_ELE_SUB_TP_GROUP == device->ele_sub->tp)) {
        if (device->app_fifo.ele[0].val_num >= device->fifo_size) {
            device->inform_cb.ele(device->name, ele_num, device->app_fifo.ele);
            for (i = 0; i < ele_num; i++) {
                tkl_system_memset(device->app_fifo.ele[i].val, 0, SIZEOF(SR_VAL_U)*device->fifo_size);
                device->app_fifo.ele[i].val_num = 0;
            }
        }
    } else {
        for (i = 0; i < ele_num; i++) {
            if (device->app_fifo.ele[i].val_num < device->ele_sub->rule[i].val_num) {
                continue;
            }
            device->inform_cb.ele(device->name, 1, &device->app_fifo.ele[i]);
            tkl_system_memset(device->app_fifo.ele[i].val, 0, SIZEOF(SR_VAL_U)*device->app_fifo.ele[i].val_num);
            device->app_fifo.ele[i].val_num = 0;
        }
    }

    return;
}

/**
 * @brief inform raw value to application
 *
 * @param[inout] device: device node
 *
 * @return none
 */
STATIC VOID_T __tdl_sensor_load_and_inform_raw_value(INOUT SR_DEV_NODE_T *device)
{
    OPERATE_RET op_ret;

    UINT_T idx = device->app_fifo.raw.len;
    SR_RAW_DATA_T *raw_buf = &device->app_fifo.raw.data[idx];

    // read raw data from device (tdd sensor interface)
    op_ret = device->intfs->read_raw(&device->resource, raw_buf);
    if (op_ret != 0) {
        TAL_PR_ERR("device: %s -> read raw_data failed !", device->name);
        return;
    }

    device->app_fifo.raw.len++;

    if (device->app_fifo.raw.len >= device->fifo_size) {
        device->inform_cb.raw(device->name, device->app_fifo.raw.len, device->app_fifo.raw.data);
        tkl_system_memset(device->app_fifo.raw.data, 0, SIZEOF(SR_RAW_DATA_T)*device->app_fifo.raw.len);
        device->app_fifo.raw.len = 0;
    }

    return;
}

/**
 * @brief acquire handler
 *
 * @param none
 *
 * @return none
 */
STATIC VOID_T __tdl_sensor_acquire_handle(VOID_T)
{
    SR_DEV_NODE_T *device = NULL;

    device = sg_sr_dev_list_head.next;
    while (device != NULL) {
        if (FALSE == device->flag.is_start ||
            FALSE == device->flag.is_need_acquire) {
            device = device->next;
            continue;
        }

        tkl_mutex_lock(device->mutex);
        if (SR_DATA_TP_ELE == __tdl_sensor_get_data_tp(device->ele_num)) {
            if (OPRT_OK == __tdl_sensor_load_ele_value(device)) {
                __tdl_sensor_inform_ele_value(device);
            }
        } else {
            __tdl_sensor_load_and_inform_raw_value(device);
        }
        tkl_mutex_unlock(device->mutex);

        device = device->next;
    }
}

/**
 * @brief main task for tdl_sensor_hub
 *
 * @param[in] args: NULL
 *
 * @return none
 */
STATIC VOID_T __tdl_sensor_main_task(PVOID_T args)
{
    UINT_T soft_tm_cnt, hard_tm_cnt;

    while (1) {
        tkl_semaphore_wait(sg_task_sem, TKL_SEM_WAIT_FOREVER);

        // timer polling processing
        soft_tm_cnt = sg_soft_tm_cnt;
        sg_soft_tm_cnt = 0;
        hard_tm_cnt = sg_hard_tm_cnt;
        sg_hard_tm_cnt = 0;
        __tdl_sensor_timer_proc(soft_tm_cnt, hard_tm_cnt);

        // external interrupt processing
        __tdl_sensor_exti_proc();

        // acquire processing
        __tdl_sensor_acquire_handle();
    }

    tkl_thread_release(sg_sensor_thrd);
}

/**
 * @brief initialize the sensor hub frame
 *
 * @param none
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdl_sensor_hub_frame_init(VOID_T)
{
    OPERATE_RET op_ret;

    // create and start a thread for tdl_sensor_hub
    op_ret = tkl_thread_create(&sg_sensor_thrd, "sr_main", 1024, 3, __tdl_sensor_main_task, NULL);
    if (op_ret !=  OPRT_OK) {
        return op_ret;
    }

    // create a semaphore for tdl_sensor_hub
    op_ret = tkl_semaphore_create_init(&sg_task_sem, 0, 1);
    if (op_ret !=  OPRT_OK) {
        return op_ret;
    }

    return OPRT_OK;
}

/**
 * @brief find sensor device
 *
 * @param[in] name: device name (string)
 * @param[out] handle: device handle
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdl_sensor_dev_find(IN CHAR_T *name, OUT SENSOR_HANDLE_T* handle)
{
    SR_DEV_NODE_T *device = NULL;

    device = __tdl_sensor_find_dev_node(&sg_sr_dev_list_head, name);
    if (NULL == device) {
        return OPRT_COM_ERROR;
    }

    *handle = (SENSOR_HANDLE_T *)device;

    return OPRT_OK;
}

/**
 * @brief open sensor device
 *
 * @param[in] handle: device handle
 * @param[in] config: configuration parameters
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdl_sensor_dev_open(IN SENSOR_HANDLE_T handle, IN SR_DEV_CFG_T *config)
{
    OPERATE_RET op_ret = OPRT_OK;
    SR_DEV_NODE_T *device = NULL;

    if (NULL == handle || NULL == config) {
        return OPRT_INVALID_PARM;
    }

    device = (SR_DEV_NODE_T *)handle;

    op_ret = __tdl_sensor_check_config(device, config);
    if (OPRT_OK != op_ret) {
        return op_ret;
    }

    tkl_mutex_lock(device->mutex);

    op_ret = __tdl_sensor_dev_open(device, config);

    tkl_mutex_unlock(device->mutex);

    return op_ret;
}

/**
 * @brief close sensor device
 *
 * @param[in] handle: device handle
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdl_sensor_dev_close(IN SENSOR_HANDLE_T handle)
{
    OPERATE_RET op_ret = OPRT_OK;
    SR_DEV_NODE_T *device = NULL;

    if (NULL == handle) {
        return OPRT_INVALID_PARM;
    }

    device = (SR_DEV_NODE_T *)handle;

    tkl_mutex_lock(device->mutex);

    op_ret = __tdl_sensor_dev_close(device);

    tkl_mutex_unlock(device->mutex);

    return op_ret;
}

/**
 * @brief configure sensor device
 *
 * @param[in] handle: device handle
 * @param[in] cmd: configuration command
 * @param[inout] param: configuration parameters
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdl_sensor_dev_config(IN SENSOR_HANDLE_T handle, IN UCHAR_T cmd, INOUT VOID_T *param)
{
    OPERATE_RET op_ret = OPRT_OK;
    SR_DEV_NODE_T *device = NULL;

    if (NULL == handle) {
        return OPRT_INVALID_PARM;
    }

    device = (SR_DEV_NODE_T *)handle;

    tkl_mutex_lock(device->mutex);

    // control device (tdd sensor interface)
    op_ret = device->intfs->control(&device->resource, cmd, param);

    tkl_mutex_unlock(device->mutex);

    return op_ret;
}

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
                                INOUT SR_ELE_DATA_T *ele_data, OUT SR_RAW_DATA_T *raw_data)
{
    OPERATE_RET op_ret = OPRT_OK;
    SR_DEV_NODE_T *device = NULL;

    if (NULL == handle) {
        return OPRT_INVALID_PARM;
    }

    device = (SR_DEV_NODE_T *)handle;

    if ((0 != ele_num && NULL == ele_data) ||
        (0 == ele_num && NULL == raw_data) ||
        (ele_num > device->ele_num)) {
        return OPRT_INVALID_PARM;
    }

    tkl_mutex_lock(device->mutex);

    op_ret = __tdl_sensor_dev_read(device, ele_num, ele_data, raw_data);

    tkl_mutex_unlock(device->mutex);

    return op_ret;
}

/**
 * @brief register sensor
 *
 * @param[in] name: device name
 * @param[in] intfs: interface for operating the sensor
 * @param[in] ele_num: number of elements, 0 means the element type is raw
 * @param[in] ele_cfg: data type configuration for each element
 * @param[in] resource: resource that device depends on
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdl_sensor_register(IN CHAR_T *name, IN SR_INTFS_T *intfs, IN UCHAR_T ele_num,
                                IN SR_ELE_CFG_T *ele_cfg, IN SR_RSRC_T *resource)
{
    OPERATE_RET op_ret = OPRT_OK;
    STATIC BOOL_T s_is_frame_init = FALSE;

    // check parameters
    if (NULL == name || NULL == intfs ||
        (0 != ele_num && NULL == ele_cfg)) {
        return OPRT_INVALID_PARM;
    }
    if (NULL == intfs->open || NULL == intfs->close || NULL == intfs->control) {
        return OPRT_INVALID_PARM;
    }
    if ((0 == ele_num && NULL == intfs->read_raw) ||
        (0 != ele_num && NULL == intfs->read_ele)) {
        return OPRT_INVALID_PARM;
    }

    // initialize the sensor hub frame
    if (FALSE == s_is_frame_init) {
        op_ret = __tdl_sensor_hub_frame_init();
        if (op_ret != OPRT_OK) {
            return op_ret;
        }
        s_is_frame_init = TRUE;
    }

    // register sensor
    op_ret = __tdl_sensor_register(name, intfs, ele_num, ele_cfg, resource);

    return op_ret;
}
