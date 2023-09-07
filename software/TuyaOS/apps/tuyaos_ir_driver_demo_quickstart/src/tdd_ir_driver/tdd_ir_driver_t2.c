/**
 * @file tdd_ir_driver_t2.c
 * @author www.tuya.com
 * @brief tdd_ir_driver_t2 module is used to 
 * @version 0.1
 * @date 2023-03-22
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#if 0
#include "BkDriverPwm.h"
#include "pwm_pub.h"
#endif

#include "tal_log.h"
#include "tal_memory.h"
#include "tal_system.h"
#include "tkl_pwm.h"
#include "tkl_timer.h"
#include "tkl_gpio.h"
#include "tkl_output.h"

#include "tdd_ir_driver.h"

/***********************************************************
************************macro define************************
***********************************************************/

#define CAPTURE_DRIVER_NUM_MAX      2

#define IR_SEND_TIMEOUT_US          (1*1000*1000)
#define IR_SEND_INTER_CODE_DELAY    (0) // unit: us, default: 300ms
#define IR_SEND_FREQ_DEFAULT        (38000)

#define IR_SEND_TIM_HIGH_ERR_VALUE  (-55)
#define IR_SEND_TIM_LOW_ERR_VALUE   (-115)

/***********************************************************
***********************typedef define***********************
***********************************************************/

typedef struct{
    volatile unsigned int irq_val; /* interrupt value */
}BK7231N_REG_T;

typedef struct {
    IR_MODE_E                       driver_mode;
    IR_DRIVER_TYPE_E                driver_type;
    unsigned char                   user_close_recv_flag;
    void                            *tdl_hdl;
    TUYA_PWM_NUM_E                  send_pwm_id;

    /* ir receive use irq+timer */
    volatile unsigned char          irq_edge_mode;
    volatile unsigned int           last_recv_time;

    /* ir receive use capture */
    int                             recv_pwm_id;

    /* tdl callback */
    IR_DRV_OUTPUT_FINISH_CB         output_finish_cb;
    IR_DRV_RECV_CB                  recv_value_cb;

    /* ir drive hardware config information */
    IR_DRV_CFG_T                    hw_cfg;
}IR_DRV_INFO_T;

/***********************************************************
********************function declaration********************
***********************************************************/

/* tdl adapter interface */
static int __tdd_ir_drv_open(IR_DRV_HANDLE_T drv_hdl, unsigned char mode, IR_TDL_TRANS_CB ir_tdl_cb, void *args);
static int __tdd_ir_drv_close(IR_DRV_HANDLE_T drv_hdl, unsigned char mode);
static int __tdd_ir_drv_output(IR_DRV_HANDLE_T drv_hdl, unsigned int freq, unsigned char is_active, unsigned int time_us);
static int __tdd_ir_drv_status_notification(IR_DRV_HANDLE_T drv_hdl, IR_DRIVER_STATE_E state, void *args);

static int __tdd_ir_pre_recv_process(IR_DRV_HANDLE_T drv_hdl);
static int __tdd_ir_recv_finish_process(IR_DRV_HANDLE_T drv_hdl);
static int __tdd_ir_pre_send_process(IR_DRV_HANDLE_T drv_hdl);
static int __tdd_ir_send_finish_process(IR_DRV_HANDLE_T drv_hdl);

/* hardware interface */
static int __tdd_ir_send_hw_init(IR_DRV_INFO_T *drv_info);
static void __tdd_ir_send_hw_deinit(IR_DRV_INFO_T *drv_info);
static int __tdd_ir_recv_hw_init(IR_DRV_INFO_T *drv_info);
static void __tdd_ir_recv_hw_deinit(IR_DRV_INFO_T *drv_info);

static int __tdd_ir_send_start(IR_DRV_INFO_T *drv_info);

static int __tdd_get_pwm_id(unsigned int gpio_id, TUYA_PWM_NUM_E *pwm_id);

/***********************************************************
***********************variable define**********************
***********************************************************/

static TDD_IR_INTFS_T tdd_ir_intfs = {
    .open           = __tdd_ir_drv_open,
    .close          = __tdd_ir_drv_close,
    .output         = __tdd_ir_drv_output,
    .status_notif   = __tdd_ir_drv_status_notification
};

static unsigned int *__cap_tdl_hdl_map[CAPTURE_DRIVER_NUM_MAX] = {NULL};

static volatile unsigned char __fiq_disable_cnt = 0;
static volatile unsigned char __irq_disable_cnt = 0;
static BK7231N_REG_T reg_val = {0};

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief bk7231n fiq enable
 *
 * @param[in] none: none
 *
 * @return none
 */
static void __tdd_bk7231n_fiq_enable(void)
{
    if (0 == __fiq_disable_cnt) {
        return;
    }

    __fiq_disable_cnt--;

    if (0 == __fiq_disable_cnt) {
        *(volatile unsigned long *) (0x00802000 + 0x11 * 4) |= 0x00000002;
    }

    return;
}

/**
 * @brief bk7231n fiq disable
 *
 * @param[in] none: none
 *
 * @return none
 */
static void __tdd_bk7231n_fiq_disable(void)
{
    if (0xFF == __fiq_disable_cnt) {
        return;
    }

    if (0 == __fiq_disable_cnt) {
        *(volatile unsigned long *) (0x00802000 + 0x11 * 4) &= 0xFFFFFFFFD;
    }

    __fiq_disable_cnt++;

    return;
}

/**
 * @brief bk7231n interrupt enable
 *
 * @param[in] reg_val: register value
 *
 * @return none
 */
static void __tdd_bk7231n_isr_enable(BK7231N_REG_T *reg_val)
{
    if (NULL == reg_val || 0 == __irq_disable_cnt) {
        return;
    }

    __irq_disable_cnt--;

    if (0 == __irq_disable_cnt && 0 != reg_val->irq_val) {
        *(volatile unsigned long *) (0x00802000 + 0x10 * 4) |= reg_val->irq_val;
        reg_val->irq_val = 0;
    }

    return;
}

/**
 * @brief bk7231n interrupt disable, only enable gpio, timer, pwm interrupt
 *
 * @param[in] reg_val: register value
 *
 * @return none
 */
static void __tdd_bk7231n_isr_disable(BK7231N_REG_T *reg_val)
{
    if (NULL == reg_val || 0xFF == __irq_disable_cnt) {
        return;
    }

    if (0 == __irq_disable_cnt) {
        /* enable timer, gpio, pwm interrupt only */
        reg_val->irq_val = (*(volatile unsigned long *) (0x00802000 + 0x10 * 4)) & 0xFFFF;
        *(volatile unsigned long *) (0x00802000 + 0x10 * 4) &= 0xFFFF8380;
    }

    __irq_disable_cnt++;

    return;
}

/**
 * @brief set pwm info and start pwm
 *
 * @param[in] pwm_id: pwm id
 * @param[in] pwm_freq: pwm frequency
 * @param[in] pwm_duty: pwm duty
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
static int __tdd_ir_pwm_start(int pwm_id, int pwm_freq, int pwm_duty)
{
    OPERATE_RET op_ret = OPRT_OK;

    if (0 == pwm_duty) {
        return tkl_pwm_stop(pwm_id);
    }

    TUYA_PWM_BASE_CFG_T pwm_cfg = {
        .polarity = TUYA_PWM_NEGATIVE,
        .duty = pwm_duty*100,
        .frequency = pwm_freq,
    };
    op_ret = tkl_pwm_info_set(pwm_id, &pwm_cfg);
    if (OPRT_OK != op_ret) {
        goto __EXIT;
    }

    op_ret = tkl_pwm_start(pwm_id);

__EXIT:
    return op_ret;
}

/**
 * @brief driver status notification function
 *
 * @param[in] drv_hdl: driver handle
 * @param[in] device_state: device state
 * @param[in] args: 
 *
 * @return none
 */
static int __tdd_ir_drv_status_notification(IR_DRV_HANDLE_T drv_hdl, IR_DRIVER_STATE_E state, void *args)
{
    if (NULL == drv_hdl) {
        return OPRT_INVALID_PARM;
    }
    IR_DRV_INFO_T *drv_info = drv_hdl;

    switch (state) {
        case IR_DRV_PRE_SEND_STATE:
            __tdd_ir_pre_send_process(drv_hdl);
        break;
        case IR_DRV_SEND_FINISH_STATE:
            __tdd_ir_send_finish_process(drv_hdl);
        break;
        case IR_DRV_PRE_RECV_STATE:
            __tdd_ir_pre_recv_process(drv_hdl);
        break;
        case IR_DRV_RECV_FINISH_STATE:
            __tdd_ir_recv_finish_process(drv_hdl);
        break;
        case IR_DRV_SEND_HW_RESET:
            __tdd_ir_send_hw_deinit(drv_hdl);
            __tdd_ir_send_hw_init(drv_hdl);
        break;
        case IR_DRV_RECV_HW_INIT:
            drv_info->user_close_recv_flag = 0;
            __tdd_ir_recv_hw_init(drv_hdl);
        break;
        case IR_DRV_RECV_HW_DEINIT:
            __tdd_ir_recv_hw_deinit(drv_hdl);
            drv_info->user_close_recv_flag = 1;
        break;
        default:break;
    }

    return OPRT_OK;
}

/**
 * @brief ir driver send callback
 *
 * @param[in] drv_handle: driver handle
 *
 * @return none
 */
static void __tdd_ir_timer_send_cb(void *drv_handle)
{
    IR_DRV_INFO_T *drv_info = NULL;

    drv_info = (IR_DRV_INFO_T *)drv_handle;
    if (NULL == drv_info) {
        tkl_log_output("send cb input is null\r\n");
        return;
    }

    /* stop timer */
    tkl_timer_stop(drv_info->hw_cfg.send_timer);

    if (NULL != drv_info->output_finish_cb) {
        drv_info->output_finish_cb(drv_info->tdl_hdl);
    }

    return;
}

/**
 * @brief gpio irq callback function
 *
 * @param[in] drv_handle: driver handle
 *
 * @return none
 */
static void __tdd_ir_irq_recv_cb(void *drv_handle)
{
    unsigned int cur_us = 0, out_us = 0;
    IR_DRV_INFO_T *drv_info = (IR_DRV_INFO_T *)drv_handle;

    if (NULL == drv_info) {
        return;
    }

    tkl_gpio_irq_disable(drv_info->hw_cfg.recv_pin);
    tkl_gpio_deinit(drv_info->hw_cfg.recv_pin);

    tkl_timer_get(drv_info->hw_cfg.recv_timer, &cur_us);

    if (drv_info->last_recv_time < cur_us) {
        out_us = cur_us - (drv_info->last_recv_time);
    } else {
        out_us = (0xffffffffUL-drv_info->last_recv_time)+cur_us+1;
    }
    drv_info->last_recv_time = cur_us;

    drv_info->recv_value_cb(drv_handle, out_us, drv_info->tdl_hdl);

    /* enable next irq interrupt */
    if (TUYA_GPIO_IRQ_FALL == drv_info->irq_edge_mode) {
        drv_info->irq_edge_mode = TUYA_GPIO_IRQ_RISE;
    } else if (TUYA_GPIO_IRQ_RISE == drv_info->irq_edge_mode) {
        drv_info->irq_edge_mode = TUYA_GPIO_IRQ_FALL;
    }

    TUYA_GPIO_IRQ_T gpio_irq_cfg = {
        .mode = drv_info->irq_edge_mode,
        .cb = __tdd_ir_irq_recv_cb,
        .arg = drv_handle
    };
    tkl_gpio_irq_init(drv_info->hw_cfg.recv_pin, &gpio_irq_cfg);
    tkl_gpio_irq_enable(drv_info->hw_cfg.recv_pin);

    return;
}

/**
 * @brief ir receive timer callback
 *
 * @param[in] args:
 *
 * @return none
 */
static void __tdd_timer_recv_cb(void *args)
{
    /* nothing to do here */
    return;
}

/**
 * @brief ir send hardware init
 *
 * @param[in] drv_info: driver info structure
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
static int __tdd_ir_send_hw_init(IR_DRV_INFO_T *drv_info)
{
    OPERATE_RET op_ret = OPRT_OK;

    if (NULL == drv_info) {
        return OPRT_INVALID_PARM;
    }

    if (IR_INPUT_INVALID == drv_info->send_pwm_id || \
        IR_INPUT_INVALID == drv_info->hw_cfg.send_timer)
    {
        return OPRT_NOT_SUPPORTED;
    }

    /* pwm init */
    TUYA_PWM_BASE_CFG_T pwm_cfg = {
        .polarity = TUYA_PWM_NEGATIVE,
        .duty = drv_info->hw_cfg.send_duty*100,
        .frequency = IR_SEND_FREQ_DEFAULT,
    };
    op_ret = tkl_pwm_init(drv_info->send_pwm_id, &pwm_cfg);
    if (OPRT_OK != op_ret) {
        tkl_log_output("pwm init error\r\n");
        return op_ret;
    }

    /* timer init */
    TUYA_TIMER_BASE_CFG_E timer_cfg = {
        .mode = TUYA_TIMER_MODE_PERIOD,
        .cb = __tdd_ir_timer_send_cb,
        .args = drv_info
    };
    op_ret = tkl_timer_init(drv_info->hw_cfg.send_timer, &timer_cfg);
    if (OPRT_OK != op_ret) {
        tkl_log_output("timer init error\r\n");
        return op_ret;
    }

    return OPRT_OK;
}

/**
 * @brief ir send hardware deinit
 *
 * @param[in] drv_info: driver info structure
 *
 * @return none
 */
static void __tdd_ir_send_hw_deinit(IR_DRV_INFO_T *drv_info)
{
    OPERATE_RET op_ret = OPRT_OK;
    
    if (NULL == drv_info) {
        tkl_log_output("send hardware deinit error, input invalid\r\n");
        return;
    }

    /* deinit pwm */
    op_ret = tkl_pwm_stop(drv_info->send_pwm_id);
    if (op_ret != OPRT_OK) {
        tkl_log_output("pwm stop error\r\n");
    }
    op_ret = tkl_pwm_deinit(drv_info->send_pwm_id);
    if (op_ret != OPRT_OK) {
        tkl_log_output("pwm deinit error\r\n");
    }

    /* deinit timer */
    op_ret = tkl_timer_stop(drv_info->hw_cfg.send_timer);
    if (op_ret != OPRT_OK) {
        tkl_log_output("timer stop error\r\n");
    }
    op_ret = tkl_timer_deinit(drv_info->hw_cfg.send_timer);
    if (op_ret != OPRT_OK) {
        tkl_log_output("timer deinit error\r\n");
    }

    return;
}

/**
 * @brief ir receive IR_DRV_SINGLE_TIMER, IR_DRV_DUAL_TIMER init
 *
 * @param[in] drv_info: driver info structure
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
static int __tdd_ir_recv_irq_timer_init(IR_DRV_INFO_T *drv_info)
{
    OPERATE_RET op_ret = OPRT_OK;

    if (IR_INPUT_INVALID == drv_info->hw_cfg.recv_pin || \
        IR_INPUT_INVALID == drv_info->hw_cfg.recv_timer)
    {
        return OPRT_NOT_SUPPORTED;
    }

    /* timer init */
    TUYA_TIMER_BASE_CFG_E timer_cfg = {
        .mode = TUYA_TIMER_MODE_PERIOD,
        .cb = __tdd_timer_recv_cb,
        .args = NULL
    };
    op_ret = tkl_timer_init(drv_info->hw_cfg.recv_timer, &timer_cfg);
    if (OPRT_OK != op_ret) {
        tkl_log_output("timer init err\r\n");
        op_ret = OPRT_COM_ERROR;
        goto __EXIT;
    }

    op_ret = tkl_timer_start(drv_info->hw_cfg.recv_timer, 0xFFFFFFFF);
    if (OPRT_OK != op_ret) {
        tkl_log_output("timer start err\r\n");
        op_ret = OPRT_COM_ERROR;
        goto __EXIT;
    }

    TUYA_GPIO_IRQ_T gpio_irq_cfg = {
        .mode = TUYA_GPIO_IRQ_FALL,
        .cb = __tdd_ir_irq_recv_cb,
        .arg = (void *)drv_info,
    };
    drv_info->irq_edge_mode = TUYA_GPIO_IRQ_FALL;
    op_ret = tkl_gpio_irq_init(drv_info->hw_cfg.recv_pin, &gpio_irq_cfg);
    if (OPRT_OK != op_ret) {
        tkl_log_output("gpio irq init err\r\n");
        op_ret = OPRT_COM_ERROR;
        goto __EXIT;
    }

    op_ret = tkl_gpio_irq_enable(drv_info->hw_cfg.recv_pin);
    if (OPRT_OK != op_ret) {
        tkl_log_output("enable gpio irq err\r\n");
        op_ret = OPRT_COM_ERROR;
    }

__EXIT:
    if (OPRT_OK != op_ret) {
        tkl_timer_stop(drv_info->hw_cfg.recv_timer);
        tkl_timer_deinit(drv_info->hw_cfg.recv_timer);
    }

    return OPRT_OK;
}

/**
 * @brief pwm capture callback
 *
 * @param[in] pwm_id: PWM id for triggering capture interrupt
 *
 * @return none
 */
#if 0
static void __tdd_ir_capture_callback(unsigned char pwm_id)
{
    IR_DRV_INFO_T *drv_info = (IR_DRV_INFO_T *)__cap_tdl_hdl_map[pwm_id];

    if (NULL != drv_info && NULL != drv_info->output_finish_cb) {
        drv_info->recv_value_cb((IR_DRV_HANDLE_T)drv_info, bk_pwm_get_capvalue(drv_info->recv_pwm_id)/26, drv_info->tdl_hdl);
    }
}
#endif

/**
 * @brief pwm capture callback
 *
 * @param[in] drv_info: driver info structure
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
static int __tdd_ir_recv_capture_init(IR_DRV_INFO_T *drv_info)
{
    OPERATE_RET op_ret = OPRT_OK;

#if 0
    if (IR_INPUT_INVALID == drv_info->recv_pwm_id)
    {
        return OPRT_NOT_SUPPORTED;
    }

    op_ret = bk_pwm_capture_initialize(drv_info->recv_pwm_id, PWM_CAP_EDGE_MODE);
    if (kNoErr != op_ret) {
        tkl_log_output("pwm capture init failed\r\n");
        return OPRT_COM_ERROR;
    }

    op_ret = bk_pwm_set_isr_callback(drv_info->recv_pwm_id, __tdd_ir_capture_callback);
    if (kNoErr != op_ret) {
        tkl_log_output("pwm capture set isr failed\r\n");
        return OPRT_COM_ERROR;
    }

    op_ret = bk_pwm_en_isr_callback(drv_info->recv_pwm_id);
    if (kNoErr != op_ret) {
        tkl_log_output("pwm capture enable isr failed\r\n");
        return OPRT_COM_ERROR;
    }

    op_ret = bk_pwm_start(drv_info->recv_pwm_id);
    if (kNoErr != op_ret) {
        tkl_log_output("pwm capture start failed\r\n");
        return OPRT_COM_ERROR;
    }

    return op_ret;
#endif

    return OPRT_COM_ERROR;
}

/**
 * @brief ir receive hardware init
 *
 * @param[in] drv_info: driver info structure
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
static int __tdd_ir_recv_hw_init(IR_DRV_INFO_T *drv_info)
{
    OPERATE_RET op_ret = OPRT_OK;

    if (NULL == drv_info) {
        return OPRT_INVALID_PARM;
    }

    if (drv_info->user_close_recv_flag) {
        tkl_log_output("user close ir recv, no init\r\n");
        return OPRT_OK;
    }

    if (IR_DRV_CAPTURE == drv_info->driver_type) {
        op_ret = __tdd_ir_recv_capture_init(drv_info);
    } else {
        op_ret = __tdd_ir_recv_irq_timer_init(drv_info);
    }

    return op_ret;
}

/**
 * @brief ir receive IR_DRV_SINGLE_TIMER, IR_DRV_DUAL_TIMER type deinit
 *
 * @param[in] drv_info: driver info structure
 *
 * @return none
 */
static void __tdd_ir_recv_irq_timer_deinit(IR_DRV_INFO_T *drv_info)
{
    tkl_gpio_irq_disable(drv_info->hw_cfg.recv_pin);
    tkl_gpio_deinit(drv_info->hw_cfg.recv_pin);
    tkl_timer_stop(drv_info->hw_cfg.recv_timer);
    tkl_timer_deinit(drv_info->hw_cfg.recv_timer);

    return;
}

/**
 * @brief ir receive capture deinit
 *
 * @param[in] drv_info: driver info structure
 *
 * @return none
 */
static void __tdd_ir_recv_capture_deinit(IR_DRV_INFO_T *drv_info)
{
    int op_ret = 0;

#if 0
    op_ret = bk_pwm_en_isr_callback(drv_info->recv_pwm_id);
    if (kNoErr != op_ret) {
        tkl_log_output("pwm capture disable isr failed, %d\r\n", op_ret);
        return;
    }

    op_ret = bk_pwm_stop(drv_info->recv_pwm_id);
    if (kNoErr != op_ret) {
        tkl_log_output("pwm capture stop failed, %d\r\n", op_ret);
        return;
    }
#endif
    return;
}

/**
 * @brief ir receive hardware deinit
 *
 * @param[in] drv_info: driver info structure
 *
 * @return none
 */
static void __tdd_ir_recv_hw_deinit(IR_DRV_INFO_T *drv_info)
{
    if (NULL == drv_info) {
        tkl_log_output("ir receive hardware deinit error, input invalid\r\n");
        return;
    }

    if (IR_DRV_CAPTURE == drv_info->driver_type) {
        __tdd_ir_recv_capture_deinit(drv_info);
    } else {
        __tdd_ir_recv_irq_timer_deinit(drv_info);
    }

    return;
}

/**
 * @brief ir pre-send process
 *
 * @param[in] drv_info: driver info structure
 *
 * @return none
 */
static int __tdd_ir_pre_send_process(IR_DRV_HANDLE_T drv_hdl)
{
    OPERATE_RET op_ret = OPRT_OK;
    IR_DRV_INFO_T *drv_info = (IR_DRV_INFO_T *)drv_hdl;

    if (IR_DRV_SINGLE_TIMER == drv_info->driver_type && IR_MODE_SEND_RECV == drv_info->driver_mode) {
        __tdd_ir_recv_hw_deinit(drv_info);
    }

    op_ret = __tdd_ir_send_hw_init(drv_info);
    if (OPRT_OK != op_ret) {
        return op_ret;
    }

    __tdd_bk7231n_isr_disable(&reg_val);

    /* close fiq */
    __tdd_bk7231n_fiq_disable();

    op_ret = __tdd_ir_send_start(drv_info);
    if (OPRT_OK != op_ret) {
        tkl_log_output("tdd send start error\r\n");
        __tdd_ir_send_finish_process(drv_hdl);
        return op_ret;
    }

    return OPRT_OK;
}

/**
 * @brief ir send finish process
 *
 * @param[in] drv_info: driver info structure
 * @param[in] is_success: 1: send success, 0: send fail
 *
 * @return none
 */
static int __tdd_ir_send_finish_process(IR_DRV_HANDLE_T drv_hdl)
{
    if (NULL == drv_hdl) {
        tkl_log_output("send finish process error, input invalid\r\n");
        return OPRT_INVALID_PARM;
    }

    IR_DRV_INFO_T *drv_info = (IR_DRV_INFO_T *)drv_hdl;

    __tdd_bk7231n_isr_enable(&reg_val);

    /* open fiq */
    __tdd_bk7231n_fiq_enable();

    __tdd_ir_send_hw_deinit(drv_info);
    if (IR_DRV_SINGLE_TIMER == drv_info->driver_type && IR_MODE_SEND_RECV == drv_info->driver_mode) {
        __tdd_ir_recv_hw_init(drv_info);
    }

    return OPRT_OK;
}

/**
 * @brief ir pre-receive process
 *
 * @param[in] drv_info: driver info struct
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
static int __tdd_ir_pre_recv_process(IR_DRV_HANDLE_T drv_hdl)
{
    __tdd_bk7231n_isr_disable(&reg_val);

    /* close fiq */
    __tdd_bk7231n_fiq_disable();

    if (NULL == drv_hdl) {
        return OPRT_INVALID_PARM;
    }

    return OPRT_OK;
}

/**
 * @brief ir receive finish process
 *
 * @param[in] drv_info: driver info struct
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
static int __tdd_ir_recv_finish_process(IR_DRV_HANDLE_T drv_hdl)
{
    IR_DRV_INFO_T *drv_info = (IR_DRV_INFO_T *)drv_hdl;

    __tdd_bk7231n_isr_enable(&reg_val);

    /* open fiq */
    __tdd_bk7231n_fiq_enable();

    if (NULL == drv_info) {
        return OPRT_INVALID_PARM;
    }

    drv_info->last_recv_time = 0;

    return OPRT_OK;
}

/**
 * @brief start send callback
 *
 * @param[in] drv_info: driver info structure
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
static int __tdd_ir_send_start(IR_DRV_INFO_T *drv_info)
{
    OPERATE_RET op_ret = OPRT_OK;
    UINT_T  delay_time = 0;

    if (NULL == drv_info) {
        return OPRT_INVALID_PARM;
    }

    op_ret = __tdd_ir_pwm_start(drv_info->send_pwm_id, IR_SEND_FREQ_DEFAULT, 0);
    if (OPRT_OK != op_ret) {
        return op_ret;
    }

    if (IR_SEND_INTER_CODE_DELAY == 0) {
        delay_time = 300; //us
    } else {
        delay_time = IR_SEND_INTER_CODE_DELAY;
    }

    op_ret = tkl_timer_start(drv_info->hw_cfg.send_timer, delay_time);
    if (OPRT_OK != op_ret) {
        return op_ret;
    }

    return op_ret;
}

/**
 * @brief tdd open interface 
 *
 * @param[in] drv_hdl: ir driver info handle
 * @param[in] mode: ir device open mode
 * @param[in] ir_tdl_cb: ir device callback struct
 * @param[in] tdl_hdl: 
 * 
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
static int __tdd_ir_drv_open(IR_DRV_HANDLE_T drv_hdl, unsigned char mode, IR_TDL_TRANS_CB ir_tdl_cb, void *tdl_hdl)
{
    OPERATE_RET op_ret = OPRT_OK;
    IR_DRV_INFO_T *drv_info = NULL;

    if (NULL==drv_hdl || NULL==tdl_hdl) {
        return OPRT_INVALID_PARM;
    }

    if ((IR_MODE_SEND_ONLY==mode && NULL==ir_tdl_cb.output_finish_cb) || \
        (IR_MODE_RECV_ONLY==mode && NULL==ir_tdl_cb.recv_cb) || \
        (IR_MODE_SEND_RECV==mode && (NULL==ir_tdl_cb.output_finish_cb || NULL==ir_tdl_cb.recv_cb))) {
        return OPRT_INVALID_PARM;
    }

    drv_info = (IR_DRV_INFO_T *)drv_hdl;

    drv_info->output_finish_cb = ir_tdl_cb.output_finish_cb;
    drv_info->recv_value_cb = ir_tdl_cb.recv_cb;
    drv_info->tdl_hdl = tdl_hdl;
    drv_info->driver_mode = mode;

    if (IR_DRV_CAPTURE == drv_info->driver_type) {
        __cap_tdl_hdl_map[drv_info->recv_pwm_id] = drv_hdl;
    }

    if (IR_MODE_SEND_ONLY != mode) {
        drv_info->user_close_recv_flag = 0;
        __tdd_ir_recv_hw_deinit(drv_info);
        op_ret = __tdd_ir_recv_hw_init(drv_info);
        if (OPRT_OK != op_ret) {
            tkl_log_output("tdd recv hardware init failed\r\n");
            return op_ret;
        }
    }

    return OPRT_OK;
}

/**
 * @brief close ir driver
 *
 * @param[in] drv_hdl: ir driver info handle
 * @param[in] mode: ir device open mode
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
static int __tdd_ir_drv_close(IR_DRV_HANDLE_T drv_hdl, unsigned char mode)
{
    IR_DRV_INFO_T *drv_info = NULL;

    if (NULL == drv_hdl) {
        return OPRT_INVALID_PARM;
    }

    drv_info = (IR_DRV_INFO_T *)drv_hdl;

    __tdd_ir_send_hw_deinit(drv_info);
    if (IR_MODE_SEND_ONLY != mode) {
        __tdd_ir_recv_hw_deinit(drv_info);
    }

    return OPRT_OK;
}

/**
 * @brief ir device tdl send interface
 *
 * @param[in] drv_hdl: tdd driver handle
 * @param[in] freq: send carrier frequency
 * @param[in] is_active: 1: pwm, 0: low level
 * @param[in] time_us: time
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
static int __tdd_ir_drv_output(IR_DRV_HANDLE_T drv_hdl, unsigned int freq, unsigned char is_active, unsigned int time_us)
{
    OPERATE_RET op_ret = OPRT_OK;
    unsigned int delay_time = 0;

    if (NULL == drv_hdl) {
        return OPRT_INVALID_PARM;
    }

    IR_DRV_INFO_T *drv_info = (IR_DRV_INFO_T *)drv_hdl;

    if (is_active) {
        delay_time = ((time_us + IR_SEND_TIM_HIGH_ERR_VALUE)<=0 ? time_us : (time_us + IR_SEND_TIM_HIGH_ERR_VALUE));
        __tdd_ir_pwm_start(drv_info->send_pwm_id, freq, drv_info->hw_cfg.send_duty);
    } else {
        delay_time = ((time_us + IR_SEND_TIM_LOW_ERR_VALUE)<=0 ? time_us : (time_us + IR_SEND_TIM_LOW_ERR_VALUE));
        __tdd_ir_pwm_start(drv_info->send_pwm_id, freq, 0);
    }

    if (delay_time<50 || delay_time>IR_SEND_TIMEOUT_US) {
        op_ret = OPRT_COM_ERROR;
    } else {
        op_ret = tkl_timer_start(drv_info->hw_cfg.send_timer, delay_time);
    }

    return op_ret;
}

/**
 * @brief Convert gpio id to pwm id
 *
 * @param[in] gpio_id: gpio id, reference "tuya_cloud_types.h" file TUYA_GPIO_NUM_E
 * @param[out] pwm_id: pwm id, reference "tuya_cloud_types.h" file TUYA_PWM_NUM_E
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
static int __tdd_get_pwm_id(unsigned int gpio_id, TUYA_PWM_NUM_E *pwm_id)
{
    unsigned char i;
    const unsigned int gpio_pwm_map[6][2] = {
        {GPIO_NUM_6,  PWM_NUM_0},
        {GPIO_NUM_7,  PWM_NUM_1},
        {GPIO_NUM_8,  PWM_NUM_2},
        {GPIO_NUM_9,  PWM_NUM_3},
        {GPIO_NUM_24, PWM_NUM_4},
        {GPIO_NUM_26, PWM_NUM_5}
    };

    if (NULL == pwm_id) {
        return OPRT_INVALID_PARM;
    }

    for (i = 0; i < 6; i++) {
        if (gpio_pwm_map[i][0] == gpio_id) {
            *pwm_id = (TUYA_PWM_NUM_E)gpio_pwm_map[i][1];
            break;
        }
    }

    if (i >= 6) {
        return OPRT_NOT_SUPPORTED;
    }

    return OPRT_OK;
}

/**
 * @brief register ir driver
 *
 * @param[in] dev_name: device name, maximum 16 bytes
 * @param[in] drv_cfg: driver config params
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
int tdd_ir_driver_register(const char *dev_name, IR_DRIVER_TYPE_E driver_type, IR_DRV_CFG_T drv_cfg)
{
    OPERATE_RET rt = OPRT_OK;
    unsigned int recv_pwm_id = 0;

    IR_DRV_INFO_T *tdd_drv_info = NULL;

    if (NULL == dev_name || IR_DRV_TYPE_MAX <= driver_type) {
        return OPRT_INVALID_PARM ;
    }

    if (IR_DRV_CAPTURE == driver_type) {
#if 0
        rt = __tdd_get_pwm_id(drv_cfg.recv_pin, &recv_pwm_id);
        if (OPRT_OK != rt) {
            return rt;
        }
        if (recv_pwm_id > 1) {
            TAL_PR_ERR("%d no support capture mode", drv_cfg.recv_pin);
            rt = OPRT_NOT_SUPPORTED;
            return rt;
        }
#else
        return OPRT_NOT_SUPPORTED;
#endif
    }

    tdd_drv_info = (IR_DRV_INFO_T *)tal_malloc(SIZEOF(IR_DRV_INFO_T));
    if (NULL == tdd_drv_info) {
        return OPRT_MALLOC_FAILED;
    }
    memset(tdd_drv_info, 0, SIZEOF(IR_DRV_INFO_T));

    if (drv_cfg.send_pin != IR_INPUT_INVALID) {
        TUYA_CALL_ERR_RETURN(__tdd_get_pwm_id(drv_cfg.send_pin, &tdd_drv_info->send_pwm_id));
    } else {
        tdd_drv_info->send_pwm_id = IR_INPUT_INVALID;
    }

    if (IR_DRV_SINGLE_TIMER == driver_type) {
        drv_cfg.recv_timer = drv_cfg.send_timer;
    }
    tdd_drv_info->driver_type = driver_type;

    memcpy(&tdd_drv_info->hw_cfg, &drv_cfg, sizeof(IR_DRV_CFG_T));

#if 0
    TAL_PR_DEBUG("************* ir use hw **********");
    TAL_PR_DEBUG("* send: pin:%d ---> PWM%d, TIMER%d", tdd_drv_info->send_pwm_id, tdd_drv_info->send_pwm_id, tdd_drv_info->hw_cfg.send_timer);
    if (IR_DRV_CAPTURE == driver_type) {
        TAL_PR_DEBUG("* recv: pin:%d ---> PWM%d", tdd_drv_info->hw_cfg.recv_pin, tdd_drv_info->recv_pwm_id);
    } else {
        TAL_PR_DEBUG("* recv: pin:%d, TIMER%d", tdd_drv_info->hw_cfg.recv_pin, tdd_drv_info->hw_cfg.recv_timer);
    }
    TAL_PR_DEBUG("**********************************");
#endif

    rt = tdl_ir_dev_register(dev_name, (IR_DRV_HANDLE_T)tdd_drv_info, &tdd_ir_intfs);

    return rt;
}
