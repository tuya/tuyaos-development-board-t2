/**
 * @file tdd_dimmer_sm2135e.c
 * @author www.tuya.com
 * @brief tdd_dimmer_sm2135e module is used to drive SM2135E chip
 * @version 0.1
 * @date 2022-08-26
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */
#include <string.h>

#include "tal_memory.h"
#include "tal_log.h"

#include "tdl_dimmer_driver.h"
#include "tdd_dimmer_common.h"

#include "tdd_dimmer_sm2135e.h"
/***********************************************************
************************macro define************************
***********************************************************/
#define SM2135E_OUTPUT_CH_MAX       5u

/**
 * @brief frame format
 */
#define SM2135E_SETTING_TAG         0x03

/**
 * @brief work mode
 */
#define SM2135E_MODE_RGB            0x00    // RGB channel work mode
#define SM2135E_MODE_CW             0x80    // CW channel work mode

/**
 * @brief address
 */
#define SM2135E_ADDR_MAX_CUR        0x00    // address to set current

/**
 * @brief max current range (mA)
 */
#define SM2135E_RGB_CUR_MAX         45      // RGB channel upper limit
#define SM2135E_RGB_CUR_MIN         10      // RGB channel lower limit
#define SM2135E_CW_CUR_MAX          80      // CW channel upper limit
#define SM2135E_CW_CUR_MIN          10      // CW channel lower limit

/**
 * @brief max output gray value
 */
#define SM2135E_GRAY_VAL_MAX        255    // 256 gray level

/***********************************************************
***********************typedef define***********************
***********************************************************/
#pragma pack(1)
typedef struct {
    UCHAR_T    addr:        4;   //下一个byte的地址
    UCHAR_T    res:         2;  
    UCHAR_T    tag:         2;   //固定标识
}SETTING_T;

typedef struct {
    UCHAR_T    res:         7; 
    UCHAR_T    is_cw:       1;  //0: rgb 1:cw
}OUT_ENABLE_T;

typedef struct {
    SETTING_T                setting;
    UCHAR_T                  max_current;
    OUT_ENABLE_T             out_enable;
    UCHAR_T                  out[SM2135E_OUTPUT_CH_MAX];
}IIC_PROT_T;

#pragma pack()

/**
 * @brief driver information
 */
typedef struct {
    TDD_I2C_PIN_T            i2c_pin;
    UCHAR_T                  max_cur_code;
    DIMMER_CH_CFG_U          ch_cfg;
    IIC_PROT_T               send_buf;
} DIMMER_SM2135E_DRV_T;

/***********************************************************
********************function declaration********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief open SM2135E dimmer
 *
 * @param[in] drv_handle: dimmer driver handle
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
STATIC OPERATE_RET __tdd_dimmer_sm2135e_open(IN DIMMER_DRV_HANDLE drv_handle)
{
    DIMMER_SM2135E_DRV_T *p_drv = (DIMMER_SM2135E_DRV_T *)drv_handle;

    if(NULL == p_drv) {
        return OPRT_INVALID_PARM;
    }

    return tdd_sw_i2c_init(p_drv->i2c_pin);
}

/**
 * @brief close SM2135E dimmer
 *
 * @param[in] drv_handle: dimmer driver handle
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
STATIC OPERATE_RET __tdd_dimmer_sm2135e_close(IN DIMMER_DRV_HANDLE drv_handle)
{
    DIMMER_SM2135E_DRV_T *p_drv = (DIMMER_SM2135E_DRV_T *)drv_handle;

    if(NULL == p_drv) {
        return OPRT_INVALID_PARM;
    }

    return tdd_sw_i2c_deinit(p_drv->i2c_pin);
}

/**
 * @brief control dimmer output
 *
 * @param[in] drv_handle: driver handle
 * @param[in] p_rgbcw: the value of the value
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
STATIC OPERATE_RET __tdd_dimmer_sm2135e_output(IN DIMMER_DRV_HANDLE drv_handle, LIGHT_RGBCW_U *p_rgbcw)
{
    DIMMER_SM2135E_DRV_T *p_drv = (DIMMER_SM2135E_DRV_T *)drv_handle;
    IIC_PROT_T *p_prot = NULL;
    UCHAR_T i, ch_id = 0;
    BOOL_T is_rgb_ch = FALSE;

    if(NULL == p_drv || NULL == p_rgbcw) {
        return OPRT_INVALID_PARM;
    }

    p_prot = &p_drv->send_buf;
    memset((UCHAR_T *)p_prot, 0x00, SIZEOF(IIC_PROT_T));

    p_prot->setting.tag  = SM2135E_SETTING_TAG;
    p_prot->setting.addr = SM2135E_ADDR_MAX_CUR;
    p_prot->max_current  = p_drv->max_cur_code;

    for (i = 0; i < LIGHT_COLOR_CHANNEL_MAX; i++) {
        ch_id = p_drv->ch_cfg.array[i];

        if(DIMMER_CH_ID_INVALID == ch_id || ch_id >= SM2135E_OUTPUT_CH_MAX) {
            continue;
        }

        p_prot->out[ch_id] = (UCHAR_T)p_rgbcw->array[i];

        if(ch_id <=  DIMMER_RGB_CH_ID_MAX && ch_id >= DIMMER_RGB_CH_ID_MIN) {
            is_rgb_ch = TRUE;
        }else {
            if(FALSE == is_rgb_ch || p_rgbcw->array[i] > 0) {
                p_prot->out_enable.is_cw = 1;
            }
        }
    }

    return tdd_dimmer_i2c_send_data(p_drv->i2c_pin, (UCHAR_T *)p_prot, SIZEOF(IIC_PROT_T));
}

/**
 * @brief register SM2135E dimmer
 *
 * @param[in] name: dimmer name
 * @param[in] p_i2c: i2c pin config
 * @param[in] p_ch_cfg: color channel id config
 * @param[in] p_chip_cfg:  chip config (max current code ex.)
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdd_dimmer_sm2135e_register(CHAR_T *name, TDD_I2C_PIN_T *p_i2c,\
                                        DIMMER_CH_CFG_U *p_ch_cfg, SM2135E_CFG_T *p_chip_cfg)
{
    DIMMER_DRV_INTFS_T intfs;

    if(NULL == p_i2c || NULL == p_chip_cfg) {
        return OPRT_INVALID_PARM;
    }

    if(FALSE == tdd_check_channel_cfg_param(p_ch_cfg)) {
        TAL_PR_ERR("channel cfg error");
        return OPRT_INVALID_PARM;
    }

    DIMMER_SM2135E_DRV_T *p_drv = tal_malloc(SIZEOF(DIMMER_SM2135E_DRV_T));
    if (NULL == p_drv) {
        return OPRT_MALLOC_FAILED;
    }
    memset((UCHAR_T *)p_drv, 0x00, SIZEOF(DIMMER_SM2135E_DRV_T));

    memcpy((UCHAR_T *)&p_drv->i2c_pin, (UCHAR_T *)p_i2c,    SIZEOF(TDD_I2C_PIN_T));
    memcpy((UCHAR_T *)&p_drv->ch_cfg,  (UCHAR_T *)p_ch_cfg, SIZEOF(DIMMER_CH_CFG_U));

    p_drv->max_cur_code = (((p_chip_cfg->max_curr_code.color & 0x07) << 4) | (p_chip_cfg->max_curr_code.white & 0x0F));

    intfs.open   = __tdd_dimmer_sm2135e_open;
    intfs.close  = __tdd_dimmer_sm2135e_close;
    intfs.output = __tdd_dimmer_sm2135e_output;

    return tdl_dimmer_register(name, &intfs, SM2135E_GRAY_VAL_MAX, DIMMER_DRIVER_IIC, (DIMMER_DRV_HANDLE)p_drv);
}

/**
 * @brief convert rgb maximum current code
 *
 * @param[in] current_ma: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_sm2135e_convert_rgb_curr_code(UCHAR_T current_ma)
{
    UCHAR_T cur_tmp = 0;
    UCHAR_T cur_code = 0x00;

    if (current_ma > SM2135E_RGB_CUR_MAX) {
        cur_tmp = SM2135E_RGB_CUR_MAX;
        TAL_PR_NOTICE("[SM2135E] The maximum current setting cannot exceed %d, \
                             the default value has been used.", SM2135E_RGB_CUR_MAX);
    } else if (current_ma < SM2135E_RGB_CUR_MIN) {
        cur_tmp = SM2135E_RGB_CUR_MIN;
    } else {
        cur_tmp = current_ma;
    }
    cur_code = ((cur_tmp - 10) / 5 << 4);

    return cur_code;
}

/**
 * @brief convert cw maximum current code
 *
 * @param[in] current_ma: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_sm2135e_convert_cw_curr_code(UCHAR_T current_ma)
{
    UCHAR_T cur_tmp = 0;
    UCHAR_T cur_code = 0x00;

    if (current_ma > SM2135E_CW_CUR_MAX) {
        cur_tmp = SM2135E_CW_CUR_MAX;
        TAL_PR_NOTICE("[SM2135E] The maximum current setting cannot exceed %d, \
                             the default value has been used.", SM2135E_CW_CUR_MAX);
    } else if (current_ma < SM2135E_CW_CUR_MIN) {
        cur_tmp = SM2135E_CW_CUR_MIN;
    } else {
        cur_tmp = current_ma;
    }

    if (cur_tmp <= 60) {
        cur_code = ((cur_tmp - 10) / 5);
    } else {
        cur_code = ((cur_tmp - 60) / 4 + 10);
    }

    return cur_code;   
}