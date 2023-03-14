/**
 * @file tdd_dimmer_bp1658cj.c
 * @author www.tuya.com
 * @brief tdd_dimmer_bp1658cj module is used to drive BP1658CJ chip
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

#include "tdd_dimmer_bp1658cj.h"
/***********************************************************
************************macro define************************
***********************************************************/
#define BP1658CJ_OUTPUT_CH_MAX      5u

/**
 * @brief frame format
 */
#define BP1658CJ_SETTING_TAG        0x02 

/**
 * @brief address
 */
#define BP1658CJ_ADDR_MAX_CUR       0x0     // address to set max current

/**
 * @brief max current range (mA)
 */
#define BP1658CJ_RGB_CUR_MAX        150     // RGB channel upper limit
#define BP1658CJ_CW_CUR_MAX         75      // CW channel upper limit

/**
 * @brief max output gray value
 */
#define BP1658CJ_GRAY_VAL_MAX         1023    // 1024 gray level

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef UCHAR_T BP1658CJ_WORK_MODE_E;
#define BP1658CJ_MODE_SLEEP         0x00     // sleep mode
#define BP1658CJ_MODE_RGB           0x01     // OUT1~3 channel work mode
#define BP1658CJ_MODE_CW            0x02     // OUT4~5 channel work mode
#define BP1658CJ_MODE_RGBCW         0x03     // OUT1~5 channel work mode

#pragma pack(1)
typedef struct {
    UCHAR_T    addr:        4;   //下一个byte的地址
    UCHAR_T    mode:        2;   //BP1658CJ_WORK_MODE_E
    UCHAR_T    tag:         2;   //固定标识
}SETTING_T;

typedef struct{
    USHORT_T    low:         5;     //颜色数值低 5 位
    USHORT_T    res1:        3; 
    USHORT_T    high:        5;     //颜色数值高 5 位
    USHORT_T    res2:        3;
}OUTPUT_T;

typedef struct {
    SETTING_T                setting;
    UCHAR_T                  max_current;
    OUTPUT_T                 out[BP1658CJ_OUTPUT_CH_MAX];
}IIC_PROT_T;

#pragma pack()

typedef struct {
    TDD_I2C_PIN_T            i2c_pin;
    UCHAR_T                  max_cur_code;
    DIMMER_CH_CFG_U          ch_cfg;
    IIC_PROT_T               send_buf;
}DIMMER_BP1658CJ_DRV_T;
/***********************************************************
********************function declaration********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
STATIC BP1658CJ_WORK_MODE_E __bp1658cj_get_work_mode(DIMMER_CH_CFG_U *p_cf_cfg, LIGHT_RGBCW_U *p_rgbcw)
{
    UCHAR_T i = 0, ch_id = 0;
    BP1658CJ_WORK_MODE_E mode = BP1658CJ_MODE_SLEEP;

    if(NULL == p_cf_cfg || NULL == p_rgbcw) {
        return BP1658CJ_MODE_SLEEP;
    }

    for(i = 0; i<LIGHT_COLOR_CHANNEL_MAX; i++) {
        ch_id = p_cf_cfg->array[i];
        if((DIMMER_CH_ID_INVALID == ch_id) || (p_rgbcw->array[i] == 0)) {
            continue;
        }

        if((ch_id >= DIMMER_RGB_CH_ID_MIN) && (ch_id <= DIMMER_RGB_CH_ID_MAX)) {
            mode |= BP1658CJ_MODE_RGB;
        }else if ((ch_id >= DIMMER_CW_CH_ID_MIN) && (ch_id <= DIMMER_CW_CH_ID_MAX)){
            mode |= BP1658CJ_MODE_CW;
        }
    }

    return mode;
}

/**
 * @brief open BP1658CJ dimmer
 *
 * @param[in] drv_handle: dimmer driver handle
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
STATIC OPERATE_RET __tdd_dimmer_bp1658cj_open(IN DIMMER_DRV_HANDLE drv_handle)
{
    DIMMER_BP1658CJ_DRV_T *p_drv = (DIMMER_BP1658CJ_DRV_T *)drv_handle;

    if(NULL == p_drv) {
        return OPRT_INVALID_PARM;
    }

    return tdd_sw_i2c_init(p_drv->i2c_pin);
}

/**
 * @brief close BP1658CJ dimmer
 *
 * @param[in] drv_handle: dimmer driver handle
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
STATIC OPERATE_RET __tdd_dimmer_bp1658cj_close(IN DIMMER_DRV_HANDLE drv_handle)
{
    DIMMER_BP1658CJ_DRV_T *p_drv = (DIMMER_BP1658CJ_DRV_T *)drv_handle;

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
STATIC OPERATE_RET __tdd_dimmer_bp1658cj_output(DIMMER_DRV_HANDLE drv_handle, LIGHT_RGBCW_U *p_rgbcw)
{
    DIMMER_BP1658CJ_DRV_T *p_drv = (DIMMER_BP1658CJ_DRV_T *)drv_handle;
    IIC_PROT_T *p_prot = NULL;
    UCHAR_T i, ch_id = 0;

    if(NULL == p_drv || NULL == p_rgbcw) {
        return OPRT_INVALID_PARM;
    }

    p_prot = &p_drv->send_buf;
    memset((UCHAR_T *)p_prot, 0x00, SIZEOF(IIC_PROT_T));

    p_prot->setting.addr = BP1658CJ_ADDR_MAX_CUR;
    p_prot->setting.tag  = BP1658CJ_SETTING_TAG;
    p_prot->setting.mode = __bp1658cj_get_work_mode(&p_drv->ch_cfg, p_rgbcw);

    p_prot->max_current = p_drv->max_cur_code;

    for (i = 0; i < LIGHT_COLOR_CHANNEL_MAX; i++) {
        ch_id = p_drv->ch_cfg.array[i];

        if(DIMMER_CH_ID_INVALID == ch_id || ch_id >= BP1658CJ_OUTPUT_CH_MAX) {
            continue;
        }

        p_prot->out[ch_id].low  = (UCHAR_T)(p_rgbcw->array[i] & 0x001F);
        p_prot->out[ch_id].high = (UCHAR_T)(p_rgbcw->array[i] >> 5);
    }

    return tdd_dimmer_i2c_send_data(p_drv->i2c_pin, (UCHAR_T *)p_prot, SIZEOF(IIC_PROT_T));
}

/**
 * @brief register BP1658CJ dimmer
 *
 * @param[in] name: dimmer name
 * @param[in] p_i2c: i2c pin config
 * @param[in] p_ch_cfg: color channel id config
 * @param[in] p_chip_cfg:  chip config (max current code ex.)
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdd_dimmer_bp1658cj_register(CHAR_T *name, TDD_I2C_PIN_T *p_i2c,\
                                         DIMMER_CH_CFG_U *p_ch_cfg, BP1658CJ_CFG_T *p_chip_cfg)
{
    DIMMER_DRV_INTFS_T intfs;

    if(NULL == p_i2c || NULL == p_chip_cfg) {
        return OPRT_INVALID_PARM;
    }

    if(FALSE == tdd_check_channel_cfg_param(p_ch_cfg)) {
        TAL_PR_ERR("channel cfg error");
        return OPRT_INVALID_PARM;
    }

    DIMMER_BP1658CJ_DRV_T *p_drv = tal_malloc(SIZEOF(DIMMER_BP1658CJ_DRV_T));
    if (NULL == p_drv) {
        return OPRT_MALLOC_FAILED;
    }
    memset((UCHAR_T *)p_drv, 0x00, SIZEOF(DIMMER_BP1658CJ_DRV_T));

    memcpy((UCHAR_T *)&p_drv->i2c_pin, (UCHAR_T *)p_i2c,    SIZEOF(TDD_I2C_PIN_T));
    memcpy((UCHAR_T *)&p_drv->ch_cfg,  (UCHAR_T *)p_ch_cfg, SIZEOF(DIMMER_CH_CFG_U));

    p_drv->max_cur_code = ((p_chip_cfg->max_curr_code.color << 4) | (p_chip_cfg->max_curr_code.white & 0x0F));

    intfs.open   = __tdd_dimmer_bp1658cj_open;
    intfs.close  = __tdd_dimmer_bp1658cj_close;
    intfs.output = __tdd_dimmer_bp1658cj_output;

    return tdl_dimmer_register(name, &intfs, BP1658CJ_GRAY_VAL_MAX, DIMMER_DRIVER_IIC, (DIMMER_DRV_HANDLE)p_drv);
}

/**
 * @brief convert maximum rgb current ma to code
 *
 * @param[in] cur: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_bp1658cj_convert_rgb_curr_code(UCHAR_T current_ma)
{
    UCHAR_T cur_tmp_ma = 0;
    UCHAR_T cur_code = 0x00;

    if (current_ma > BP1658CJ_RGB_CUR_MAX) {
        cur_tmp_ma = BP1658CJ_RGB_CUR_MAX;
        TAL_PR_NOTICE("[BP1658CJ] The maximum current setting cannot exceed %d, \
                              the default value has been used.", BP1658CJ_RGB_CUR_MAX);
    } else {
        cur_tmp_ma = current_ma;
    }

    cur_code = cur_tmp_ma / 10;

    return cur_code;
}

/**
 * @brief convert maximum cw current ma to code
 *
 * @param[in] cur: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_bp1658cj_convert_cw_curr_code(UCHAR_T current_ma)
{
    UCHAR_T cur_tmp_ma = 0;
    UCHAR_T cur_code = 0x00;

    if (current_ma > BP1658CJ_CW_CUR_MAX) {
        cur_tmp_ma = BP1658CJ_CW_CUR_MAX;
        TAL_PR_NOTICE("[BP1658CJ] The maximum current setting cannot exceed %d, \
                              the default value has been used.", BP1658CJ_CW_CUR_MAX);
    } else {
        cur_tmp_ma = current_ma;
    }

    cur_code = (cur_tmp_ma / 5);

    return cur_code;
}