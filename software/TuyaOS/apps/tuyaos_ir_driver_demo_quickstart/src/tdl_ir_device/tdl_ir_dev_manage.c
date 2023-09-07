/**
 * @file tdl_ir_dev_manage.c
 * @author www.tuya.com
 * @brief tdl_ir_dev_manage module is used to 
 * @version 0.1
 * @date 2022-06-06
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include <string.h>

#include "tal_log.h"
#include "tal_sleep.h"
#include "tal_system.h"
#include "tal_memory.h"
#include "tal_wifi.h"
#include "tal_queue.h"
#include "tal_thread.h"
#include "tkl_output.h"
#include "tal_semaphore.h"

#include "tdl_ir_dev_manage.h"

/***********************************************************
************************macro define************************
***********************************************************/

#define IR_RECV_THREAD_STACK    1024

#define IR_RECV_TIMEOUT_MS      300
#define OVERFLOW_TIME_MS        (0xffffffffUL)
#define IR_RECV_POST_TIMEOUT_MS (3*1000)
#define IR_RECV_VALID_LEN_MIN   20

#define IR_DEVICE_NUM_MAX       5

#ifndef QUEUE_WAIT_FOREVER
#define QUEUE_WAIT_FOREVER      0xFFFFFFFF
#endif

/***********************************************************
***********************typedef define***********************
***********************************************************/

typedef struct {
    UINT32_T                buf_len;
    volatile UINT32_T       write_idx;
    volatile UINT32_T       read_idx;
    UINT32_T                *p_buf;
}IR_RING_BUF_T;

typedef struct {
    UINT32_T                *send_data;
    UINT32_T                send_len;
    volatile UINT32_T       send_idx;
    UINT32_T                send_freq;
    UINT32_T                send_cnt;
    SEM_HANDLE              send_finish_sem_hdl;
}IR_DEV_SEND_T;

typedef struct {
    UINT8_T                 is_run;

    IR_RING_BUF_T           *ring_buf;

    volatile UINT32_T       last_time; /* tdl last receive data time, unit: ms */
    void                    *recv_err_val;
    QUEUE_HANDLE            recv_queue_hdl;
    IR_APP_RECV_CB          app_recv_cb;
}IR_DEV_RECV_T;

typedef struct ir_dev_node {
    struct ir_dev_node      *next;

    BOOL_T                  is_open;
    CHAR_T                  name[IR_DEV_NAME_MAX_LEN+1];

    /* ir driver info */
    IR_DRV_HANDLE_T         ir_drv_hdl;
    TDD_IR_INTFS_T          *drv_intfs; /* driver interface */

    IR_DEV_SEND_T           send_info;
    IR_DEV_RECV_T           recv_info;

    volatile IR_SEND_STATUS send_status;
    volatile IR_RECV_STATUS recv_status;

    /* ir device config params */
    IR_DEV_CFG_T            ir_dev_cfg;
}IR_DEV_NODE_T;

typedef struct {
    UINT16_T                recv_dev_num;
    UINT16_T                recv_dev_run_num;
    struct ir_dev_node      *next;
    THREAD_HANDLE           dev_recv_thrd_hdl;
    QUEUE_HANDLE            dev_notif_queue_hdl;
} IR_DEV_LIST_T;

/***********************************************************
********************function declaration********************
***********************************************************/

extern OPERATE_RET tdl_ir_nec_build(BOOL_T is_msb, IR_DATA_NEC_T ir_nec_data, IR_DATA_TIMECODE_T **timecode);
extern void tdl_ir_nec_build_release(IR_DATA_TIMECODE_T *timecode);
extern OPERATE_RET tdl_ir_nec_err_val_init(void **nec_err_val, IR_NEC_CFG_T *nec_cfg);
extern void tdl_ir_nec_err_val_init_release(void *nec_err_val);
extern OPERATE_RET tdl_nec_get_frame_head(UINT32_T *data, UINT16_T len, void *nec_err_val, UINT16_T *head_idx);
extern UINT16_T tdl_ir_nec_parser_single(unsigned int *data, unsigned int len, IR_DATA_NEC_T *nec_code, void *nec_err_val, unsigned char is_msb);

static IR_DEV_NODE_T *__tdl_ir_dev_find(IR_DEV_LIST_T *list, CHAR_T *dev_name);
static OPERATE_RET __tdl_ir_dev_register(CHAR_T *dev_name, IR_DRV_HANDLE_T ir_drv_hdl, TDD_IR_INTFS_T *ir_intfs);

static int __tdl_ir_output_finish_cb(void *args);
static int __tdl_ir_recv_cb(IR_DRV_HANDLE_T drv_hdl, unsigned int raw_data, void *arg);

/***********************************************************
***********************variable define**********************
***********************************************************/

/* ir device list head */
STATIC IR_DEV_LIST_T sg_list_head = {
    .recv_dev_num = 0,
    .recv_dev_run_num = 0,
    .dev_recv_thrd_hdl = NULL,
    .dev_notif_queue_hdl = NULL,
    .next = NULL,
};

/***********************************************************
***********************function define**********************
***********************************************************/

static OPERATE_RET __tdl_ir_recv_init(IR_DEV_NODE_T *ir_device);
static OPERATE_RET __tdl_ir_recv_deinit(IR_DEV_NODE_T *ir_device);

/**
 * @brief ring buffer init
 *
 * @param[inout] ir_buf: ring buffer malloc memory space
 * @param[in] buf_size: ring buffer size
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
static OPERATE_RET __tdl_ir_ring_buf_init(IR_RING_BUF_T **ir_buf, UINT32_T buf_size)
{
    (*ir_buf) = (IR_RING_BUF_T *)tal_malloc(sizeof(IR_RING_BUF_T) + buf_size);
    if (NULL == (*ir_buf)) {
        return OPRT_MALLOC_FAILED;
    }
    memset((*ir_buf), 0, sizeof(IR_RING_BUF_T) + buf_size);

    (*ir_buf)->buf_len = buf_size/sizeof(UINT32_T);
    (*ir_buf)->read_idx = (*ir_buf)->write_idx = 0;

    (*ir_buf)->p_buf = (UINT32_T *)((*ir_buf)+1);

    return OPRT_OK;
}

/**
 * @brief ring buffer deinit
 *
 * @param[in] ir_buf: ring buffer
 *
 * @return none
 */
static void __tdl_ir_ring_buf_deinit(IR_RING_BUF_T *ir_buf)
{
    tal_free(ir_buf);
    return;
}

/**
 * @brief judge if the buffer is full
 *
 * @param[in] ring_buf: ring buffer
 *
 * @return 1: buffer is full, 0: buffer is not full
 */
#define RING_BUFFER_IS_FULL(ring_buf)   \
    ((ring_buf->write_idx+1) % (ring_buf->buf_len) == ring_buf->read_idx) ? (1) : (0)

/**
 * @brief get ring buffer usage length
 *
 * @param[in] ring_buf: ring buffer
 *
 * @return ring buffer usage length
 */
#define RING_BUFFER_LENGTH_GET(ring_buf) \
    ((ring_buf->buf_len + ring_buf->write_idx - ring_buf->read_idx) % ring_buf->buf_len)

/**
 * @brief ring buffer write one word
 *
 * @param[in] ir_buf: ring buffer
 * @param[in] write_value: write value
 *
 * @return none
 */
static inline void __tdl_ir_ring_buf_write_word(IR_RING_BUF_T *ir_buf, UINT32_T write_value)
{
    ir_buf->p_buf[ir_buf->write_idx] = write_value;

    if (ir_buf->write_idx+1 < ir_buf->buf_len) {
        ir_buf->write_idx++;
    } else {
        ir_buf->write_idx = 0;
    }

    return;
}

/**
 * @brief read ring buffer and update ring buffer space
 *
 * @param[in] ir_buf: ring buffer
 * @param[out] read_data: output data
 * @param[in] read_len: read data length
 *
 * @return none
 */
static void __tdl_ir_ring_buf_read(IR_RING_BUF_T *ir_buf, UINT32_T *read_data, UINT16_T read_len)
{
    UINT16_T tmp_idx = 0;

    if (NULL == read_data) {
        return;
    }

    if (ir_buf->read_idx < ir_buf->write_idx) {
        memcpy(read_data, &ir_buf->p_buf[ir_buf->read_idx], read_len*sizeof(UINT32_T));
        ir_buf->read_idx += read_len;
    } else {
        tmp_idx = ir_buf->buf_len-ir_buf->read_idx;
        memcpy(read_data, &ir_buf->p_buf[ir_buf->read_idx], tmp_idx*sizeof(UINT32_T));
        ir_buf->read_idx = 0;
        memcpy(read_data+tmp_idx, &ir_buf->p_buf[ir_buf->read_idx], (read_len-tmp_idx)*sizeof(UINT32_T));
        ir_buf->read_idx += read_len-tmp_idx;
    }

    return;
}

/**
 * @brief read ring buffer and no update ring buffer space
 *
 * @param[in] ir_buf: ring buffer
 * @param[out] read_data: output data
 * @param[in] read_len: read data length
 *
 * @return none
 */
static void __tdl_ir_ring_buf_read_no_update(IR_RING_BUF_T *ir_buf, UINT32_T *read_data, UINT16_T read_len)
{
    UINT16_T tmp_idx = 0;

    if (NULL == read_data) {
        return;
    }

    if (ir_buf->read_idx < ir_buf->write_idx) {
        memcpy(read_data, &ir_buf->p_buf[ir_buf->read_idx], read_len*sizeof(UINT32_T));
    } else {
        tmp_idx = ir_buf->buf_len-ir_buf->read_idx;
        memcpy(read_data, &ir_buf->p_buf[ir_buf->read_idx], tmp_idx*sizeof(UINT32_T));
        memcpy(read_data+tmp_idx, &ir_buf->p_buf[0], (read_len-tmp_idx)*sizeof(UINT32_T));
    }

    return;
}

static void __tdl_ir_ring_buf_data_discard(IR_RING_BUF_T *ir_buf, UINT16_T discard_len)
{
    UINT32_T cur_data_len = RING_BUFFER_LENGTH_GET(ir_buf);
    if (cur_data_len < discard_len) {
        ir_buf->read_idx = ir_buf->write_idx;
        return;
    }

    if (ir_buf->read_idx+discard_len < ir_buf->buf_len) {
        ir_buf->read_idx += discard_len;
    } else {
        ir_buf->read_idx = (ir_buf->read_idx + discard_len) % ir_buf->buf_len;
    }

    return;
}

/**
 * @brief register ir device
 *
 * @param[in] dev_name: device name, Maximum 16 bytes
 * @param[in] ir_drv_hdl: ir driver handle
 * @param[in] ir_intfs: ir driver interface
 * 
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
static OPERATE_RET __tdl_ir_dev_register(CHAR_T *dev_name, IR_DRV_HANDLE_T ir_drv_hdl, TDD_IR_INTFS_T *ir_intfs)
{
    IR_DEV_NODE_T *dev_node = NULL;
    IR_DEV_NODE_T *cur_node = NULL;
    unsigned char name_len = 0;

    /* check device name length */
    name_len = strlen((const char *)dev_name);
    if (name_len>IR_DEV_NAME_MAX_LEN) {
        TAL_PR_ERR("device name too long, max length 16 bytes");
        return OPRT_COM_ERROR;
    }

    /* check if the device is registered */
    if (NULL != __tdl_ir_dev_find(&sg_list_head, dev_name)) {
        TAL_PR_ERR("ir device name %s already there.", dev_name);
        return OPRT_COM_ERROR;
    }

    /* malloc device node */
    dev_node = (IR_DEV_NODE_T *)tal_malloc(SIZEOF(IR_DEV_NODE_T));
    if (NULL==dev_node) {
        TAL_PR_ERR("malloc for %s fail", dev_name);
        return OPRT_MALLOC_FAILED;
    }
    memset(dev_node, 0, SIZEOF(IR_DEV_NODE_T));

    /* copy driver info */
    strncpy(dev_node->name, (const char *)dev_name, name_len);
    dev_node->name[name_len] = '\0';

    dev_node->drv_intfs = ir_intfs;
    dev_node->ir_drv_hdl = ir_drv_hdl;

    /* init device status */
    dev_node->send_status = IR_STA_SEND_IDLE;
    dev_node->recv_status = IR_STA_RECV_IDLE;
    dev_node->next = NULL;

    /* find the last node and add this new device */
    if (NULL == sg_list_head.next) {
        sg_list_head.next = dev_node;
    } else {
        cur_node = sg_list_head.next;
        while (NULL != cur_node->next) {
            cur_node = cur_node->next;
        }
        cur_node->next = dev_node;
    }

    return OPRT_OK;
}

/**
 * @brief find device
 *
 * @param[in] list: Link table header
 * @param[in] dev_name: device name
 * 
 * @return If the device is found return the node, if not return null
 */
static IR_DEV_NODE_T *__tdl_ir_dev_find(IR_DEV_LIST_T *list, CHAR_T *dev_name)
{
    IR_DEV_NODE_T *node = NULL;

    if (NULL==list || NULL==dev_name) {
        TAL_PR_ERR("tdl ir manage input param error");
        return NULL;
    }

    node = list->next;
    while (NULL != node) {
        if (0 == strcmp((const char *)dev_name, (const char *)node->name)) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

/**
 * @brief Request memory space for receive data
 *
 * @param[in] protocol_type: ir protocol type
 * @param[in] data_size: request memory space size
 * 
 * @return malloc memory address
 */
static IR_DATA_U *__tdl_ir_recv_buf_malloc(IR_PROT_E protocol_type, UINT16_T data_size)
{
    IR_DATA_U *ir_data = NULL;

    if (protocol_type >= IR_PROT_MAX) {
        return NULL;
    }

    ir_data = tal_malloc(SIZEOF(IR_DATA_U) + data_size);
    if (NULL == ir_data) {
        TAL_PR_ERR("recv buf malloc fail");
        return NULL;
    }
    memset(ir_data, 0, SIZEOF(IR_DATA_U) + data_size);

    if (IR_PROT_TIMECODE == protocol_type) {
        ir_data->timecode.data = (UINT32_T *)(ir_data+1);
    }

    return ir_data;
}

/**
 * @brief free ir receive data memory space
 *
 * @param[in] ir_data : memory address
 *
 * @return none
 */
static void __tdl_ir_recv_buf_free(IR_DATA_U *ir_data)
{
    if (NULL != ir_data) {
        tal_free(ir_data);
    }
    return;
}

/**
 * @brief send finish, free memory
 *
 * @param[in] dev_node: device node
 *
 * @return none
 */
static void __tdl_ir_send_finish(IR_DEV_NODE_T *dev_node)
{
    if (NULL == dev_node) {
        return;
    }

    dev_node->send_status = IR_STA_SEND_FINISH;
    dev_node->drv_intfs->status_notif(dev_node->ir_drv_hdl, IR_DRV_SEND_FINISH_STATE, NULL);

    dev_node->send_info.send_idx = 0;
    dev_node->send_info.send_len = 0;
    dev_node->send_info.send_cnt = 0;

    if (NULL != dev_node->send_info.send_finish_sem_hdl) {
        tal_semaphore_post(dev_node->send_info.send_finish_sem_hdl);
    }

    return;
}

/**
 * @brief ir send callback
 *
 * @param[in] args: device handle
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
static int __tdl_ir_output_finish_cb(void *args)
{
    OPERATE_RET op_ret = OPRT_OK;
    IR_DEV_NODE_T *dev_info = NULL;
    UINT8_T is_active = 0;
    UINT32_T timeout_us = 0;

    dev_info = (IR_DEV_NODE_T *)args;

    if (NULL == dev_info || NULL == dev_info->drv_intfs->output) {
        __tdl_ir_send_finish(dev_info);
        return OPRT_INVALID_PARM;
    }

    if (dev_info->send_info.send_idx >= dev_info->send_info.send_len && dev_info->send_info.send_cnt == 0) { /* send over */
        __tdl_ir_send_finish(dev_info);
        return OPRT_OK;
    } else if (dev_info->send_info.send_idx >= dev_info->send_info.send_len && dev_info->send_info.send_cnt > 0){ /* send count is not 0, update send information */
        dev_info->send_info.send_idx = 0;
        dev_info->send_info.send_cnt--;
    }

    timeout_us = dev_info->send_info.send_data[dev_info->send_info.send_idx];
    if (dev_info->send_info.send_idx % 2 == 0) {
        is_active = 1;
    } else {
        is_active = 0;
    }
    dev_info->send_info.send_idx++;

    op_ret = dev_info->drv_intfs->output(dev_info->ir_drv_hdl, dev_info->send_info.send_freq, is_active, timeout_us);
    if (OPRT_OK != op_ret) {
        __tdl_ir_send_finish(dev_info);
    }

    return op_ret;
}

/**
 * @brief ir receive callback
 *
 * @param[in] drv_hdl: ir driver handle
 * @param[in] raw_data: receive raw data
 * @param[in] args: device handle
 * 
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
static int __tdl_ir_recv_cb(IR_DRV_HANDLE_T drv_hdl, unsigned int raw_data, void *args)
{
    IR_DEV_NODE_T *dev_info = NULL;
    int temp_data = raw_data;

    if (NULL == args) {
        /* Interrupt callbacks, can't use TAL_PR_ */
        tkl_log_output("tdl ir receive input arg is null\r\n");
        return 0;
    }

    raw_data = (temp_data>0)?(temp_data):(-temp_data);

    dev_info = (IR_DEV_NODE_T *)args;

    dev_info->recv_info.last_time = tal_system_get_millisecond(); // update receive time

    if (RING_BUFFER_IS_FULL(dev_info->recv_info.ring_buf)) {
        if (IR_STA_RECVING == dev_info->recv_status) {
            dev_info->recv_status = IR_STA_RECV_OVERFLOW;
            dev_info->drv_intfs->status_notif(dev_info->ir_drv_hdl, IR_DRV_RECV_FINISH_STATE, NULL);
        }
        return 0;
    }

    if (IR_STA_RECV_IDLE == dev_info->recv_status) {
        dev_info->recv_status = IR_STA_RECVING;
        dev_info->drv_intfs->status_notif(dev_info->ir_drv_hdl, IR_DRV_PRE_RECV_STATE, NULL);
        /* disable low-power mode */
        if (TRUE == tal_cpu_get_lp_mode()) {
            tal_cpu_lp_disable();
        }

        /* receive start, post queue */
        if (NULL != sg_list_head.dev_notif_queue_hdl) {
            tal_queue_post(sg_list_head.dev_notif_queue_hdl, &dev_info, QUEUE_WAIT_FOREVER);
        }

    } else if (IR_STA_RECVING == dev_info->recv_status) {
        __tdl_ir_ring_buf_write_word(dev_info->recv_info.ring_buf, raw_data);
    }

    return 0;
}

/**
 * @brief ir receive data process
 *
 * @param[in] dev_info: ir device structure
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
static OPERATE_RET __tdl_ir_recv_data_process(IR_DEV_NODE_T *dev_info)
{
    OPERATE_RET op_ret = OPRT_OK;
    UINT16_T data_len = 0;
    UINT16_T parser_head = 0, parser_offset = 0, parser_len = 0;
    int ret_len = 0;
    UINT32_T *tmp_buf = NULL;
    IR_DATA_U *out_data = NULL;
    IR_RING_BUF_T *tmp_rb = NULL;


    if (NULL == dev_info) {
        TAL_PR_ERR("__tdl_ir_recv_data_process is null");
        return OPRT_INVALID_PARM;
    }
    tmp_rb = dev_info->recv_info.ring_buf;

    data_len = RING_BUFFER_LENGTH_GET(tmp_rb);
    if (IR_PROT_NEC == dev_info->ir_dev_cfg.prot_opt) {
        tmp_buf = tal_malloc(data_len * SIZEOF(UINT32_T));
        if (NULL == tmp_buf) {
            /* clear ring buffer */
            tmp_rb->read_idx = tmp_rb->write_idx;
            return OPRT_MALLOC_FAILED;
        }
        memset(tmp_buf, 0, data_len * SIZEOF(UINT32_T));
        __tdl_ir_ring_buf_read_no_update(tmp_rb, tmp_buf, data_len);

        parser_offset = 0;
        do {
            parser_len = data_len-parser_offset;
            op_ret = tdl_nec_get_frame_head(&tmp_buf[parser_offset], parser_len, dev_info->recv_info.recv_err_val, &parser_head);
            if (OPRT_OK != op_ret) {
                break;
            }

            out_data = __tdl_ir_recv_buf_malloc(IR_PROT_NEC, 0);
            if (NULL == out_data) {
                TAL_PR_ERR("ir recv data process timecode malloc fail");
                return OPRT_MALLOC_FAILED;
            }

            ret_len = tdl_ir_nec_parser_single(&tmp_buf[parser_head], parser_len, &out_data->nec_data, dev_info->recv_info.recv_err_val, \
                                    dev_info->ir_dev_cfg.prot_cfg.nec_cfg.is_nec_msb);
            parser_offset += ret_len;
            parser_len -= ret_len;

            if (ret_len >= 68) {
                op_ret = tal_queue_post(dev_info->recv_info.recv_queue_hdl, &out_data, IR_RECV_POST_TIMEOUT_MS);
                if (OPRT_OK != op_ret) {
                    TAL_PR_ERR("post queue error, %d", op_ret);
                    __tdl_ir_recv_buf_free(out_data);
                    out_data = NULL;
                }
            } else {
                __tdl_ir_recv_buf_free(out_data);
                out_data = NULL;
            }
        }while (parser_offset+68 <= data_len);

        if (NULL != tmp_buf) {
            tal_free(tmp_buf);
            tmp_buf = NULL;
        }
    } else {
        out_data = __tdl_ir_recv_buf_malloc(IR_PROT_TIMECODE, data_len * SIZEOF(UINT32_T));
        if (NULL == out_data) {
            TAL_PR_ERR("ir recv data process timecode malloc fail");
            return OPRT_MALLOC_FAILED;
        }
        __tdl_ir_ring_buf_read(dev_info->recv_info.ring_buf, out_data->timecode.data, data_len);
        out_data->timecode.len = data_len;
        op_ret = tal_queue_post(dev_info->recv_info.recv_queue_hdl, &out_data, IR_RECV_POST_TIMEOUT_MS);
        if (OPRT_OK != op_ret) {
            TAL_PR_ERR("post queue error, %d", op_ret);
            __tdl_ir_recv_buf_free(out_data);
            out_data = NULL;
        }
    }

    return op_ret;
}

/**
 * @brief Receive and decode IR data at the same time
 *
 * @param[in] dev_info: ir device structure
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
static OPERATE_RET __tdl_ir_recv_data_process_sync(IR_DEV_NODE_T *dev_info)
{
    OPERATE_RET op_ret = OPRT_OK;
    UINT32_T *recv_data = NULL;
    IR_RING_BUF_T *recv_rb = NULL;
    IR_DATA_U *out_data = NULL;
    UINT16_T total_len = 0, last_frame_len = 0;
    UINT16_T nec_head_offset = 0, parser_len = 0, parser_offset = 0;
    int ret_len = 0;
    UINT8_T is_frame_finish = 0;
    STATIC UINT16_T last_total_len = 0;
    STATIC INT16_T last_repeat = -1;

    if (NULL == dev_info) {
        TAL_PR_ERR("__tdl_ir_recv_data_process is null");
        return OPRT_INVALID_PARM;
    }
    recv_rb = dev_info->recv_info.ring_buf;

    total_len = RING_BUFFER_LENGTH_GET(recv_rb);
    if ((total_len < 66 || last_total_len+3 > total_len) && (IR_STA_RECVING == dev_info->recv_status)) {
        /* data too short, no process */
        return OPRT_COM_ERROR;
    }

    if (IR_PROT_NEC == dev_info->ir_dev_cfg.prot_opt) {
        last_total_len = total_len;

        recv_data = tal_malloc(total_len * SIZEOF(UINT32_T));
        if (NULL == recv_data) {
            /* clear ring buffer */
            recv_rb->read_idx = recv_rb->write_idx;
            TAL_PR_ERR("malloc fail, return");
            last_repeat = -1;
            last_total_len = 0;
            return OPRT_MALLOC_FAILED;
        }
        memset(recv_data, 0, total_len * SIZEOF(UINT32_T));
        __tdl_ir_ring_buf_read_no_update(recv_rb, recv_data, total_len);

        /* search nec head */
        parser_offset = 0;
        parser_len = total_len;
        op_ret = tdl_nec_get_frame_head(&recv_data[parser_offset], parser_len, dev_info->recv_info.recv_err_val, &nec_head_offset);
        if (OPRT_OK != op_ret) {
            /* no search nec head, discard invalid data */
            __tdl_ir_ring_buf_data_discard(recv_rb, parser_len);

            last_repeat = -1;
            last_total_len = 0;
            TAL_PR_ERR("no head");
            op_ret = OPRT_COM_ERROR;
            goto __EXIT;
        }
        last_frame_len = nec_head_offset;
        parser_offset += nec_head_offset;
        parser_len = total_len-parser_offset;

        out_data = __tdl_ir_recv_buf_malloc(IR_PROT_NEC, 0);
        if (NULL == out_data) {
            TAL_PR_ERR("ir recv data process timecode malloc fail");
            op_ret = OPRT_MALLOC_FAILED;
            goto __EXIT;
        }

        while (parser_len >= 66) {
            memset(out_data, 0, SIZEOF(IR_DATA_U));

            ret_len = tdl_ir_nec_parser_single(&recv_data[parser_offset], parser_len, &out_data->nec_data, dev_info->recv_info.recv_err_val, \
                                    dev_info->ir_dev_cfg.prot_cfg.nec_cfg.is_nec_msb);
            parser_offset += ret_len;
            parser_len -= ret_len;
            last_frame_len += ret_len;

            if (parser_len > 4) {
                op_ret = tdl_nec_get_frame_head(&recv_data[parser_offset], parser_len, dev_info->recv_info.recv_err_val, &nec_head_offset);
                if (OPRT_OK == op_ret) {
                    is_frame_finish = 1;
                    parser_offset += nec_head_offset;
                    parser_len -= nec_head_offset;
                    /* discard last frame data */
                    __tdl_ir_ring_buf_data_discard(recv_rb, last_frame_len);

                    last_frame_len = nec_head_offset;
                } else {
                    is_frame_finish = 0;
                }
            } else if (IR_STA_RECV_FINISH == dev_info->recv_status) {
                is_frame_finish = 1;
            } else {
                is_frame_finish = 0;
            }

            if ((ret_len >= 66 && last_repeat != out_data->nec_data.repeat_cnt) || is_frame_finish) {
                last_repeat = out_data->nec_data.repeat_cnt;
                dev_info->recv_info.app_recv_cb(is_frame_finish, out_data);
            }

            if (1 == is_frame_finish) {
                last_repeat = -1;
                last_total_len = 0;
            }
        }

        __tdl_ir_recv_buf_free(out_data);
        out_data = NULL;
    }

__EXIT:
    if (NULL != recv_data) {
        tal_free(recv_data);
        recv_data = NULL;
    }
    return op_ret;
}

/**
 * @brief ir receive task
 *
 * @param[in] args: none
 *
 * @return none
 */
static void __tdl_ir_recv_task(void *args)
{
    OPERATE_RET op_ret = OPRT_OK;
    IR_DEV_NODE_T *dev_node = NULL;
    UINT32_T cur_time = 0;

    TAL_PR_DEBUG("ir recv task start");

    for (;;) {
        if (0 == sg_list_head.recv_dev_run_num) {
            break;
        }

        op_ret = tal_queue_fetch(sg_list_head.dev_notif_queue_hdl, &dev_node, QUEUE_WAIT_FOREVER);
        if (OPRT_OK != op_ret) {
            TAL_PR_ERR("wait semaphore error, %d", op_ret);
        }

        if (NULL == dev_node || IR_STA_RECV_IDLE == dev_node->recv_status) {
            continue;
        }

        while (IR_STA_RECVING == dev_node->recv_status) {
            tal_system_sleep(10);
            cur_time = tal_system_get_millisecond();
            if (cur_time > dev_node->recv_info.last_time) {
                if (cur_time - dev_node->recv_info.last_time >= IR_RECV_TIMEOUT_MS) {
                    dev_node->recv_status = IR_STA_RECV_FINISH;
                }
            } else if (cur_time < dev_node->recv_info.last_time && dev_node->recv_info.last_time-cur_time > OVERFLOW_TIME_MS) { /* time overflow */
                if (OVERFLOW_TIME_MS-dev_node->recv_info.last_time+cur_time >= IR_RECV_TIMEOUT_MS) {
                    dev_node->recv_status = IR_STA_RECV_FINISH;
                }
            }

            if (NULL != dev_node->recv_info.app_recv_cb) {
                __tdl_ir_recv_data_process_sync(dev_node);
            }
        }

        if (IR_STA_RECV_OVERFLOW == dev_node->recv_status) {
            TAL_PR_DEBUG("ir receive overflow");
            dev_node->recv_status = IR_STA_RECV_FINISH;
        }

        if (IR_STA_RECV_FINISH == dev_node->recv_status) {
            if (NULL != dev_node->recv_info.app_recv_cb && IR_PROT_NEC == dev_node->ir_dev_cfg.prot_opt) {
                /* nec callback notify, ir data already sent to application */
                goto __EXIT;
            }

            if (RING_BUFFER_LENGTH_GET(dev_node->recv_info.ring_buf) >= IR_RECV_VALID_LEN_MIN) {
                /* ir receive finish, the tail adds the last low time */
                if (0 == RING_BUFFER_IS_FULL(dev_node->recv_info.ring_buf)) {
                    __tdl_ir_ring_buf_write_word(dev_node->recv_info.ring_buf, dev_node->ir_dev_cfg.recv_timeout * 1000);
                }

                TAL_PR_DEBUG("recv finish decode");
                op_ret = __tdl_ir_recv_data_process(dev_node);
                if (OPRT_OK != op_ret) {
                    TAL_PR_DEBUG("recv data process fail");
                }
            }

__EXIT:
            /* reset receive status */
            dev_node->recv_info.ring_buf->read_idx = dev_node->recv_info.ring_buf->write_idx;
            dev_node->recv_status = IR_STA_RECV_IDLE;

            /* notify tdd driver */
            dev_node->drv_intfs->status_notif(dev_node->ir_drv_hdl, IR_DRV_RECV_FINISH_STATE, NULL);

            if (NULL != dev_node->recv_info.app_recv_cb && TRUE == tal_cpu_get_lp_mode()) {
                tal_cpu_lp_enable();
            }
        }
    }

    /* close ir receive task */
    if (NULL != sg_list_head.dev_notif_queue_hdl) {
        tal_queue_free(sg_list_head.dev_notif_queue_hdl);
        sg_list_head.dev_notif_queue_hdl = NULL;
    }

    if (NULL != sg_list_head.dev_recv_thrd_hdl) {
        tal_thread_delete(sg_list_head.dev_recv_thrd_hdl);
        sg_list_head.dev_recv_thrd_hdl = NULL;
    }

    return;
}

/**
 * @brief register ir device
 *
 * @param[in] dev_name: device name
 * @param[in] ir_drv_hdl: ir driver handle
 * @param[in] ir_intfs: ir driver interface
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_ir_dev_register(CHAR_T *dev_name, IR_DRV_HANDLE_T ir_drv_hdl, TDD_IR_INTFS_T *ir_intfs)
{
    OPERATE_RET op_ret = OPRT_OK;

    if (NULL==dev_name || NULL==ir_intfs) {
        return OPRT_INVALID_PARM;
    }

    if (NULL==ir_intfs->open || NULL==ir_intfs->close) {
        return OPRT_INVALID_PARM;
    }

    op_ret = __tdl_ir_dev_register(dev_name, ir_drv_hdl, ir_intfs);

    return op_ret;
}

/**
 * @brief find ir device
 *
 * @param[in] dev_name: device name
 * @param[out] handle: ir device handle
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_ir_dev_find(CHAR_T *dev_name, IR_HANDLE_T *handle)
{
    IR_DEV_NODE_T *dev_node = NULL;

    if (NULL == dev_name) {
        return OPRT_INVALID_PARM;
    }

    dev_node = __tdl_ir_dev_find(&sg_list_head, dev_name);
    if (NULL == dev_node) {
        TAL_PR_ERR("ir dev no find %s", dev_name);
        return OPRT_COM_ERROR;
    }

    *handle = (IR_HANDLE_T)dev_node;

    return OPRT_OK;
}

/**
 * @brief start ir task
 *
 * @param[in] none
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
static OPERATE_RET __tdl_ir_recv_server_start(IR_HANDLE_T handle)
{
    OPERATE_RET rt = OPRT_OK;
    IR_DEV_NODE_T *node = (IR_DEV_NODE_T *)handle;

    if (NULL == handle) {
        return OPRT_INVALID_PARM;
    }

    TUYA_CALL_ERR_RETURN(__tdl_ir_recv_init(node));

    /* queue init */
    if (NULL == sg_list_head.dev_notif_queue_hdl) {
        TUYA_CALL_ERR_RETURN(tal_queue_create_init(&sg_list_head.dev_notif_queue_hdl, SIZEOF(IR_DEV_NODE_T *), IR_DEVICE_NUM_MAX*2));
    }

    /* ir task init */
    if (NULL == sg_list_head.dev_recv_thrd_hdl) {
        THREAD_CFG_T ir_thrd_param = {IR_RECV_THREAD_STACK, 4, "ir_recv_task"};
        TUYA_CALL_ERR_RETURN(tal_thread_create_and_start(&sg_list_head.dev_recv_thrd_hdl, NULL, NULL, __tdl_ir_recv_task, NULL, &ir_thrd_param));
    }

    return OPRT_OK;
}

/**
 * @brief stop ir receive server
 *
 * @param[in] none
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
static OPERATE_RET __tdl_ir_recv_server_stop(IR_HANDLE_T handle)
{
    IR_DEV_NODE_T *node = (IR_DEV_NODE_T *)handle;

    if (NULL == handle) {
        return OPRT_INVALID_PARM;
    }
    TAL_PR_DEBUG("__tdl_ir_recv_server_stop");

    __tdl_ir_recv_deinit(node);

    if (0 == sg_list_head.recv_dev_run_num) {
        /* device receive notify queue */
        if (NULL != sg_list_head.dev_notif_queue_hdl) {
            tal_queue_free(sg_list_head.dev_notif_queue_hdl);
            sg_list_head.dev_notif_queue_hdl = NULL;
        }

        if (NULL != sg_list_head.dev_recv_thrd_hdl) {
            tal_thread_delete(sg_list_head.dev_recv_thrd_hdl);
            sg_list_head.dev_recv_thrd_hdl = NULL;
        }
    }

    return OPRT_OK;
}

/**
 * @brief ir device receive deinit
 *
 * @param[in] ir_device: ir device struct
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
static OPERATE_RET __tdl_ir_recv_deinit(IR_DEV_NODE_T *ir_device)
{
    if (NULL == ir_device) {
        return OPRT_INVALID_PARM;
    }

    if (NULL != ir_device->recv_info.ring_buf) {
        __tdl_ir_ring_buf_deinit(ir_device->recv_info.ring_buf);
        ir_device->recv_info.ring_buf = NULL;
    }

    if (NULL != ir_device->recv_info.recv_queue_hdl) {
        tal_queue_free(ir_device->recv_info.recv_queue_hdl);
        ir_device->recv_info.recv_queue_hdl = NULL;
        sg_list_head.recv_dev_run_num--;
    }

    ir_device->recv_info.is_run = 0;

    return OPRT_OK;
}

/**
 * @brief ir device receive init
 *
 * @param[in] ir_device: ir device struct
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
static OPERATE_RET __tdl_ir_recv_init(IR_DEV_NODE_T *ir_device)
{
    OPERATE_RET op_ret = OPRT_NOT_SUPPORTED;

    if (NULL == ir_device) {
        return OPRT_INVALID_PARM;
    }

    if (1 == ir_device->recv_info.is_run) {
        /* nothing to do */
        return OPRT_OK;
    }

    /* ring buffer init */
    if (NULL == ir_device->recv_info.ring_buf) {
        op_ret = __tdl_ir_ring_buf_init(&ir_device->recv_info.ring_buf, ir_device->ir_dev_cfg.recv_buf_size);
        if (OPRT_OK != op_ret) {
            goto __EXIT;
        }
    }

    if (NULL == ir_device->recv_info.recv_queue_hdl) {
        op_ret = tal_queue_create_init(&ir_device->recv_info.recv_queue_hdl, SIZEOF(IR_DATA_U *), ir_device->ir_dev_cfg.recv_queue_num);
        if (OPRT_OK != op_ret) {
            TAL_PR_ERR("creat queue error:%d, queue sizeof: %d", op_ret, ir_device->ir_dev_cfg.recv_queue_num*SIZEOF(IR_DATA_U *));
            goto __EXIT;
        }
    }

__EXIT:
    if (OPRT_OK != op_ret) {
        if (NULL != ir_device->recv_info.ring_buf) {
            __tdl_ir_ring_buf_deinit(ir_device->recv_info.ring_buf);
            ir_device->recv_info.ring_buf = NULL;
        }

        if (NULL != ir_device->recv_info.recv_queue_hdl) {
            tal_queue_free(ir_device->recv_info.recv_queue_hdl);
            ir_device->recv_info.recv_queue_hdl = NULL;
        }
    } else {
        ir_device->recv_info.is_run = 1;
        sg_list_head.recv_dev_run_num++;
    }

    return op_ret;
}

/**
 * @brief open ir device
 *
 * @param[in] handle: ir device handle
 * @param[in] config: ir device config
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_ir_dev_open(IR_HANDLE_T handle, IR_DEV_CFG_T *config)
{
    OPERATE_RET op_ret = OPRT_OK;
    IR_DRV_HANDLE_T drv_hdl;
    IR_DEV_NODE_T *ir_device = NULL;
    void *err_val = NULL;

    if (NULL==handle || NULL==config) {
        return OPRT_INVALID_PARM;
    }

    if (config->ir_mode >= IR_MODE_MAX || config->prot_opt >= IR_PROT_MAX) {
            TAL_PR_ERR("config params error");
            return OPRT_INVALID_PARM;
    }

    ir_device = (IR_DEV_NODE_T *)handle;
    if (0 != ir_device->is_open) {
        TAL_PR_ERR("%d device already open, please close it and open it again.", ir_device->name);
        return OPRT_NOT_SUPPORTED;
    }

    memcpy(&ir_device->ir_dev_cfg, config, SIZEOF(IR_DEV_CFG_T));

    /* init ir hardware driver */
    drv_hdl = ir_device->ir_drv_hdl;
    IR_TDL_TRANS_CB dev_trans_cb = {
        .recv_cb = __tdl_ir_recv_cb,
        .output_finish_cb = __tdl_ir_output_finish_cb
    };
    op_ret = ir_device->drv_intfs->open(drv_hdl, config->ir_mode, dev_trans_cb, (void *)ir_device);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("tdd open err, %d", op_ret);
        return op_ret;
    }

    /* ir send resources init */
    op_ret = tal_semaphore_create_init(&ir_device->send_info.send_finish_sem_hdl, 0, 1);
    if (OPRT_OK != op_ret) {
        TAL_PR_ERR("semaphore creat err, %d", op_ret);
        return op_ret;
    }

    /* ir recv resources init */
    if (config->ir_mode != IR_MODE_SEND_ONLY) {
        sg_list_head.recv_dev_num++;
        /* ir receive task, queue inited */
        __tdl_ir_recv_server_start(handle);

        /* device receive resources request */
        if (IR_PROT_NEC == config->prot_opt) {
            op_ret = tdl_ir_nec_err_val_init(&err_val, &config->prot_cfg.nec_cfg);
            if (OPRT_OK != op_ret) {
                TAL_PR_ERR("nec error value init failed, ret=%d", op_ret);
                return op_ret;
            }
            ir_device->recv_info.recv_err_val = err_val;
        }
    }

    ir_device->send_status = IR_STA_SEND_IDLE;
    ir_device->recv_status = IR_STA_RECV_IDLE;

    ir_device->is_open = 1;

    return OPRT_OK;
}

/**
 * @brief close ir device
 *
 * @param[in] handle: ir device handle
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_ir_dev_close(IR_HANDLE_T handle)
{
    OPERATE_RET op_ret = OPRT_OK;
    IR_DEV_NODE_T *ir_device = NULL;
    IR_DRV_HANDLE_T drv_hdl;
    IR_DATA_U *ir_data = NULL;

    if (NULL==handle) {
        return OPRT_INVALID_PARM;
    }

    ir_device = (IR_DEV_NODE_T *)handle;

    while (IR_STA_SEND_IDLE != ir_device->send_status) {
        TAL_PR_DEBUG("wait ir send over");
        tal_system_sleep(10);
    }

    while (1) {
        op_ret = tdl_ir_dev_recv(handle, &ir_data, 0);
        if (OPRT_OK != op_ret) {
            break;
        }

        TAL_PR_DEBUG("ir recv queue have data");
        tdl_ir_dev_recv_release(handle, ir_data);
    }

    if (ir_device->ir_dev_cfg.ir_mode != IR_MODE_SEND_ONLY) {
        __tdl_ir_recv_deinit(ir_device);
        tdl_ir_nec_err_val_init_release(ir_device->recv_info.recv_err_val);
    }

    drv_hdl = ir_device->ir_drv_hdl;
    ir_device->drv_intfs->close(drv_hdl, ir_device->ir_dev_cfg.ir_mode);

    ir_device->is_open = 0;

    return OPRT_OK;
}

/**
 * @brief ir data send
 *
 * @param[in] handle: ir device handle
 * @param[in] ir_data: send data
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_ir_dev_send(IR_HANDLE_T handle, UINT32_T freq, IR_DATA_U ir_data, UINT8_T send_cnt)
{
    OPERATE_RET op_ret = OPRT_OK;
    IR_DEV_NODE_T *ir_device = NULL;
    IR_DATA_TIMECODE_T *timecode = NULL;

    if (NULL==handle) {
        return OPRT_INVALID_PARM;
    }

    ir_device = (IR_DEV_NODE_T *)handle;

    if (ir_device->ir_dev_cfg.ir_mode == IR_MODE_RECV_ONLY) {
        return OPRT_NOT_SUPPORTED;
    }

    if (IR_STA_SEND_IDLE != ir_device->send_status || IR_STA_RECVING == ir_device->recv_status || 0 == ir_device->is_open) {
        TAL_PR_ERR("ir device busy, device send sta: %d, recv sta: %d", ir_device->send_status, ir_device->recv_status);
        return OPRT_SEND_ERR;
    }

    switch (ir_device->ir_dev_cfg.prot_opt) {
        case IR_PROT_TIMECODE:
            timecode = &ir_data.timecode;
        break;
        case IR_PROT_NEC:
            ir_device->send_status = IR_STA_SEND_BUILD;
            op_ret = tdl_ir_nec_build(ir_device->ir_dev_cfg.prot_cfg.nec_cfg.is_nec_msb, ir_data.nec_data, &timecode);
            if (OPRT_OK != op_ret) {
                ir_device->send_status = IR_STA_SEND_IDLE;
                TAL_PR_ERR("ir nec build error\r\n");
                return OPRT_SEND_ERR;
            }
        break;
        default:
        return OPRT_NOT_SUPPORTED;
    }

    ir_device->send_info.send_data = timecode->data;
    ir_device->send_info.send_len = timecode->len;
    ir_device->send_info.send_freq = freq;
    ir_device->send_info.send_cnt = send_cnt-1;
    ir_device->send_info.send_idx = 0;

    if (NULL == ir_device->send_info.send_data) {
        return OPRT_INVALID_PARM;
    }

    /* pre-send */
    if (NULL != ir_device->drv_intfs->status_notif) {
        ir_device->send_status = IR_STA_SENDING;
        op_ret = ir_device->drv_intfs->status_notif(ir_device->ir_drv_hdl, IR_DRV_PRE_SEND_STATE, NULL);
        if (OPRT_OK != op_ret) {
            TAL_PR_DEBUG("ir pre-send error: %d", op_ret);
            return op_ret;
        }
    } else {
        ir_device->send_status = IR_STA_SEND_IDLE;
        return OPRT_COM_ERROR;
    }

    op_ret = tal_semaphore_wait(ir_device->send_info.send_finish_sem_hdl, 20*1000);
    if (OPRT_OK != op_ret) {
        ir_device->drv_intfs->status_notif(ir_device->ir_drv_hdl, IR_DRV_SEND_FINISH_STATE, NULL);

        ir_device->send_info.send_idx = 0;
        ir_device->send_info.send_len = 0;
        ir_device->send_info.send_cnt = 0;
    }

    /* free nec build memory space */
    if (IR_PROT_NEC == ir_device->ir_dev_cfg.prot_opt) {
        tdl_ir_nec_build_release(timecode);
    }

    ir_device->send_status = IR_STA_SEND_IDLE;

    return op_ret;
}

/**
 * @brief ir data recv
 *
 * @param[in] handle: ir device handle
 * @param[in] ir_data: recv data
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_ir_dev_recv(IR_HANDLE_T handle, IR_DATA_U **ir_data, UINT32_T timeout_ms)
{
    OPERATE_RET rt = OPRT_OK;
    IR_DEV_NODE_T *ir_device = NULL;

    TUYA_CHECK_NULL_RETURN(handle, OPRT_INVALID_PARM);

    ir_device = (IR_DEV_NODE_T *)handle;

    if (0 == ir_device->is_open || IR_MODE_SEND_ONLY == ir_device->ir_dev_cfg.ir_mode) {
        return OPRT_NOT_SUPPORTED;
    }

    TUYA_CALL_ERR_RETURN(tal_queue_fetch(ir_device->recv_info.recv_queue_hdl, ir_data, timeout_ms));

    return rt;
}

/**
 * @brief release recv data
 *
 * @param[in] handle: ir device handle
 * @param[in] ir_data: recv data
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_ir_dev_recv_release(IR_HANDLE_T handle, IR_DATA_U *ir_data)
{
    if (NULL==handle || NULL==ir_data) {
        return OPRT_INVALID_PARM;
    }

    __tdl_ir_recv_buf_free(ir_data);
    ir_data = NULL;

    if (TRUE == tal_cpu_get_lp_mode()) {
        tal_cpu_lp_enable();
    }

    return OPRT_OK;
}

/**
 * @brief config ir device
 *
 * @param[in] handle: ir device handle
 * @param[in] cmd: ir device command
 * @param[inout] params: config params or output data
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdl_ir_config(IR_HANDLE_T handle, IR_CMD_E cmd, void *params)
{
    OPERATE_RET op_ret = OPRT_OK;
    IR_DEV_NODE_T *ir_device = NULL;
    UINT8_T *ir_status = (UINT8_T *)params;
    IR_DATA_U *ir_data = NULL;

    if (NULL==handle) {
        return OPRT_INVALID_PARM;
    }

    ir_device = (IR_DEV_NODE_T *)handle;

    switch (cmd) {
        case IR_CMD_GET_SEND_STATUS:
            if (NULL==ir_status) {return OPRT_INVALID_PARM;}
            *ir_status = ir_device->send_status;
        break;

        case IR_CMD_GET_RECV_STATUS:
            if (NULL==ir_status) {return OPRT_INVALID_PARM;}
            *ir_status = ir_device->recv_status;
        break;

        case IR_CMD_SEND_HW_RESET:
            ir_device->drv_intfs->status_notif(ir_device->ir_drv_hdl, IR_DRV_SEND_HW_RESET, NULL);
        break;

        case IR_CMD_RECV_HW_INIT:
            ir_device->drv_intfs->status_notif(ir_device->ir_drv_hdl, IR_DRV_RECV_HW_INIT, NULL);
        break;
        case IR_CMD_RECV_HW_DEINIT:
            ir_device->drv_intfs->status_notif(ir_device->ir_drv_hdl, IR_DRV_RECV_HW_DEINIT, NULL);
        break;

        case IR_CMD_RECV_TASK_START:
            op_ret = __tdl_ir_recv_server_start(handle);
            if (OPRT_OK != op_ret) {
                goto __EXIT;
            }
            op_ret = ir_device->drv_intfs->status_notif(ir_device->ir_drv_hdl, IR_DRV_RECV_HW_INIT, NULL);
            if (OPRT_OK != op_ret) {
                goto __EXIT;
            }
        break;

        case IR_CMD_RECV_TASK_STOP:
            op_ret = ir_device->drv_intfs->status_notif(ir_device->ir_drv_hdl, IR_DRV_RECV_HW_DEINIT, NULL);
            if (OPRT_OK != op_ret) {
                goto __EXIT;
            }
            __tdl_ir_recv_deinit(ir_device);
            if (NULL != sg_list_head.dev_notif_queue_hdl) {
                tal_queue_post(sg_list_head.dev_notif_queue_hdl, &ir_device, QUEUE_WAIT_FOREVER);
            }
        break;
        case IR_CMD_RECV_QUEUE_CLEAN:
            while (1) {
                op_ret = tdl_ir_dev_recv(ir_device, &ir_data, 0);
                if (OPRT_OK != op_ret) {
                    op_ret = OPRT_OK;
                    break;
                }

                TAL_PR_DEBUG("ir recv queue have data");
                tdl_ir_dev_recv_release(ir_device, ir_data);
            }
        break;

        case IR_CMD_RECV_CB_REGISTER:
            ir_device->recv_info.app_recv_cb = params;
        break;

        default: break;
    }

__EXIT:
    return op_ret;
}

