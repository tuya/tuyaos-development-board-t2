/**
* @file tuya_device.c
* @author www.tuya.com
* @brief tuya_device module is used to 
* @version 0.1
* @date 2022-05-20
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include "tuya_iot_config.h"

#if 0
#include "tuya_cloud_types.h"
#include "tal_log.h"
#include "tkl_i2c.h"
#include "tal_thread.h"
#include "tal_system.h"

/***********************************************************
*************************micro define***********************
***********************************************************/
#define I2C_NUM_ID                  I2C_NUM_0

#define SHT3X_ADDR                  0x44
#define SHT3X_CMD_R_AL_LIM_H        0xE1  /* read alert limits, high set */
#define SHT3X_CMD_R_AL_LIM_L        0x1F  
#define TASK_IIC_PRIORITY           THREAD_PRIO_1
#define TASK_IIC_SIZE               1024

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC THREAD_CFG_T sg_task = {
    .priority = TASK_IIC_PRIORITY,
    .stackDepth = TASK_IIC_SIZE,
    .thrdname = "i2c"
};
STATIC THREAD_HANDLE sg_i2c_handle;

/*i2c config*/
STATIC TUYA_IIC_BASE_CFG_T sg_i2c_cfg = {
    .addr_width = TUYA_IIC_ADDRESS_10BIT,
    .role = TUYA_IIC_MODE_MASTER,
    .speed = TUYA_IIC_BUS_SPEED_400K
};

/***********************************************************
***********************function define**********************
***********************************************************/

/**
* @brief i2c task
*
* @param[in] param:Task parameters
* @return none
*/
VOID __i2c_task(VOID* param)
{
    OPERATE_RET op_ret = OPRT_OK;
    UCHAR_T rec_buff[3];
    UCHAR_T send_buff[2] = {SHT3X_CMD_R_AL_LIM_H, SHT3X_CMD_R_AL_LIM_L};
    USHORT_T alert_value = 0;

    /*i2c init*/
    op_ret = tkl_i2c_init(I2C_NUM_ID, &sg_i2c_cfg);
    if(OPRT_OK != op_ret) {
        TAL_PR_ERR("err<%d>,i2c init fail!", op_ret);
    }

    while (1) {
        /*IIC read data*/
        tkl_i2c_master_send(I2C_NUM_ID, SHT3X_ADDR, (UINT8_T*)send_buff, 2, TRUE);
        tkl_i2c_master_receive(I2C_NUM_ID, SHT3X_ADDR, rec_buff, 3, TRUE);
        alert_value = (rec_buff[0] & (rec_buff[1] << 8));
        TAL_PR_DEBUG("The alarm value you set is = %d", alert_value);

        tal_system_sleep(2000);
    }   
}

/**
* @brief examples_adc_init_and_read
*
* @return none
*/
VOID example_i2c(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;
    TUYA_CALL_ERR_LOG(tal_thread_create_and_start(&sg_i2c_handle, NULL, NULL, __i2c_task, NULL, &sg_task));
    return;
}

#endif
