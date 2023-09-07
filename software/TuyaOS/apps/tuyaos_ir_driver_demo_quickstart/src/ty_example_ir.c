/**
 * @file ty_example_ir.c
 * @author www.tuya.com
 * @brief ty_example_ir module is used to 
 * @version 0.1
 * @date 2023-05-31
 *
 * @copyright Copyright (c) tuya.inc 2023
 *
 */

#include "ty_example_ir.h"
#include "tal_log.h"
#include "tal_thread.h"

#include "tdl_ir_dev_manage.h"

/***********************************************************
************************macro define************************
***********************************************************/
#define USE_NEC_DEMO 1  /* 0:timecode demo, 1:nec demo */

/***********************************************************
***********************typedef define***********************
***********************************************************/


/***********************************************************
********************function declaration********************
***********************************************************/


/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC IR_HANDLE_T ir_hdl = NULL;
STATIC THREAD_HANDLE ir_thrd_hdl = NULL;

/***********************************************************
***********************function define**********************
***********************************************************/

VOID ir_demo_task(PVOID_T args)
{
    OPERATE_RET rt =  OPRT_OK;
    IR_DATA_U ir_send_data;
    IR_DATA_U *ir_recv_data = NULL;
    UINT32_T i = 0;

#if USE_NEC_DEMO
    ir_send_data.nec_data.addr = 0x807F;
    ir_send_data.nec_data.cmd = 0x1DE2;
    ir_send_data.nec_data.repeat_cnt = 1;
#else
    UINT32_T data[] = {560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 1690, 1690, 1690};
    ir_send_data.timecode.data = data;
    ir_send_data.timecode.len = strlen(data);
#endif

    for (;;) {
        TUYA_CALL_ERR_LOG(tdl_ir_dev_send(ir_hdl, 38000, ir_send_data, 2));
        TAL_PR_DEBUG("ir send start");

        rt = tdl_ir_dev_recv(ir_hdl, &ir_recv_data, 3000);
        if (OPRT_OK == rt) {
#if USE_NEC_DEMO
            TAL_PR_DEBUG("demo addr:%04x, cmd:%04x, cnt:%d", ir_recv_data->nec_data.addr, ir_recv_data->nec_data.cmd, ir_recv_data->nec_data.repeat_cnt);
#else
            TAL_PR_DEBUG("demo addr:%p, len:%d", ir_recv_data, ir_recv_data->timecode.len);
            for (i=0; i<ir_recv_data->timecode.len; i++) {
                TAL_PR_DEBUG_RAW("%d ", ir_recv_data->timecode.data[i]);
            }
            TAL_PR_DEBUG_RAW("\r\n");
#endif
            tdl_ir_dev_recv_release(ir_hdl, ir_recv_data);
        }

        tkl_system_sleep(2000);
    }

    return;
}

OPERATE_RET open_ir_device(VOID_T)
{
    OPERATE_RET rt =  OPRT_OK;
    IR_DEV_CFG_T ir_cfg = {0};

    TUYA_CALL_ERR_RETURN(tdl_ir_dev_find(IR_DEV_NAME, &ir_hdl));

    ir_cfg.ir_mode = IR_MODE_SEND_RECV;
    ir_cfg.recv_queue_num = 3;
    ir_cfg.recv_buf_size = 1024;
    ir_cfg.recv_timeout = 1000;
#if USE_NEC_DEMO
    ir_cfg.prot_opt = IR_PROT_NEC;
    ir_cfg.prot_cfg.nec_cfg.is_nec_msb = 0,
    ir_cfg.prot_cfg.nec_cfg.lead_err = 70;
    ir_cfg.prot_cfg.nec_cfg.logics_err = 45;
    ir_cfg.prot_cfg.nec_cfg.logic0_err = 45;
    ir_cfg.prot_cfg.nec_cfg.logic1_err = 70;
    ir_cfg.prot_cfg.nec_cfg.repeat_err = 50;
#else
    ir_cfg.prot_opt = IR_PROT_TIMECODE;
#endif
    TUYA_CALL_ERR_RETURN(tdl_ir_dev_open(ir_hdl, &ir_cfg));

    return OPRT_OK;
}

VOID_T ty_example_ir(VOID_T)
{
    OPERATE_RET rt =  OPRT_OK;

    TUYA_CALL_ERR_RETURN(ir_hardware_register());
    TUYA_CALL_ERR_RETURN(open_ir_device());

    THREAD_CFG_T ir_thrd_param = {2048, 4, "ir_thread"};
    tal_thread_create_and_start(&ir_thrd_hdl, NULL, NULL, ir_demo_task, NULL, &ir_thrd_param);

    return;
}
