/**
* @file examples_driver_adc.c
* @author www.tuya.com
* @brief 一个简单的tkl adc接口使用演示程序，可以通过命令行执行
* @version 0.1
* @date 2022-05-20
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include "tuya_cloud_types.h"
#include "tkl_adc.h"
#include "tal_log.h"

/***********************************************************
*************************micro define***********************
***********************************************************/
#define ADC_NUM     ADC_NUM_0
#define ADC_CHANNEL 2

/***********************************************************
***********************typedef define***********************
***********************************************************/
STATIC UINT8_T adc_channel[1] = {ADC_CHANNEL};

/***********************************************************
***********************variable define**********************
***********************************************************/


/***********************************************************
***********************function define**********************
***********************************************************/

/**
* @brief a thread int adc port and read the adc very 2 seconds
*
* @param[in] param:Task parameters
* @return none
*/
VOID example_adc(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;
    INT32_T adc_value = 0;

    TUYA_ADC_BASE_CFG_T adc_cfg = {
        .ch_list = adc_channel,
        .ch_nums = 1,    //adc Number of channel lists
        .width = 12,
        .mode = TUYA_ADC_CONTINUOUS,
        .type = TUYA_ADC_INNER_SAMPLE_VOL,
        .conv_cnt = 1,
    };

    /* ADC 0 channel 2 init */
    TUYA_CALL_ERR_GOTO(tkl_adc_init(ADC_NUM, &adc_cfg), __EXIT);

    TUYA_CALL_ERR_LOG(tkl_adc_read_single_channel(ADC_NUM, ADC_CHANNEL, &adc_value));
    TAL_PR_DEBUG("ADC%d value = %d", ADC_NUM, adc_value);

    TUYA_CALL_ERR_LOG(tkl_adc_deinit(ADC_NUM));

__EXIT:
    return;
}
