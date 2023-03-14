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

#include "tuya_cloud_types.h"

#include "tal_log.h"
#include "tal_thread.h"
#include "tal_system.h"
#include "tkl_spi.h"
/***********************************************************
*************************micro define***********************
***********************************************************/
#define SPI_ID      SPI_NUM_0

#define SPI_FREQ    10000

/***********************************************************
***********************typedef define***********************
***********************************************************/


/***********************************************************
***********************variable define**********************
***********************************************************/


/***********************************************************
***********************function define**********************
***********************************************************/

/**
* @brief spi task
*
* @param[in] param:Task parameters
* @return none
*/
VOID example_spi(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET rt = OPRT_OK;
    UCHAR_T send_buff[] = {"Hello Tuya"};

    /*spi init*/
    TUYA_SPI_BASE_CFG_T spi_cfg = {
        .mode = TUYA_SPI_MODE0,
        .freq_hz = SPI_FREQ,
        .databits = TUYA_SPI_DATA_BIT8,
        .bitorder = TUYA_SPI_ORDER_LSB2MSB,
        .role = TUYA_SPI_ROLE_MASTER,
        .type = TUYA_SPI_AUTO_TYPE
    };
    TUYA_CALL_ERR_GOTO(tkl_spi_init(SPI_ID, &spi_cfg), __EXIT);

    TUYA_CALL_ERR_LOG(tkl_spi_send(SPI_ID, send_buff, CNTSOF(send_buff)));
    TAL_PR_NOTICE("spi send \"%s\" finish", send_buff);

    TUYA_CALL_ERR_LOG(tkl_spi_deinit(SPI_ID));

__EXIT:
    return;
}
