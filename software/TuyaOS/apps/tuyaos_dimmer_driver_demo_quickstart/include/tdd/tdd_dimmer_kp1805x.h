/**
 * @file tdd_dimmer_kp18058.h
 * @author www.tuya.com
 * @brief tdd_dimmer_kp18058 module is used to drive KP18058ESP / KP18059ESP chip
 * @version 0.1
 * @date 2022-08-26
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#ifndef __TDD_DIMMER_KP18058_H__
#define __TDD_DIMMER_KP18058_H__

#include "tdd_dimmer_types.h"
#include "tdd_sw_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************macro define************************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/
/**
 * @brief line voltage compensation start voltage
 */
typedef BYTE_T LVC_STA_VOLT_E;
#define LVC_STA_VOLT_140        0x00    // 140V
#define LVC_STA_VOLT_145        0x01    // 145V
#define LVC_STA_VOLT_150        0x02    // 150V
#define LVC_STA_VOLT_155        0x03    // 155V
#define LVC_STA_VOLT_160        0x04    // 160V
#define LVC_STA_VOLT_165        0x05    // 165V
#define LVC_STA_VOLT_170        0x06    // 170V
#define LVC_STA_VOLT_175        0x07    // 175V
#define LVC_STA_VOLT_260        0x08    // 260V
#define LVC_STA_VOLT_270        0x09    // 270V
#define LVC_STA_VOLT_280        0x0A    // 280V
#define LVC_STA_VOLT_290        0x0B    // 290V
#define LVC_STA_VOLT_300        0x0C    // 300V
#define LVC_STA_VOLT_310        0x0D    // 310V
#define LVC_STA_VOLT_320        0x0E    // 320V
#define LVC_STA_VOLT_330        0x0F    // 330V

/**
 * @brief line voltage compensation slope
 *        for every 15V drop in voltage, the current drops by x%
 */
typedef BYTE_T LVC_SLOPE_E;
#define LVC_SLOPE_7P5           0x00    // 7.5%
#define LVC_SLOPE_10            0x01    // 10%
#define LVC_SLOPE_12P5          0x02    // 12.5%
#define LVC_SLOPE_15            0x03    // 15%

/**
 * @brief dimming mode
 */
typedef BYTE_T DIMMING_MODE_E;
#define DIMMING_MODE_CC         0x00    // constant current
#define DIMMING_MODE_PWM        0x01    // pwm

/**
 * @brief frequency of pwm dimming
 */
typedef BYTE_T DIMMING_PWM_FREQ_E;
#define DIMMING_PWM_FREQ_2K     0x00    // 2kHz
#define DIMMING_PWM_FREQ_1K     0x01    // 1kHz
#define DIMMING_PWM_FREQ_500    0x02    // 500Hz
#define DIMMING_PWM_FREQ_250    0x03    // 250Hz

/**
 * @brief line voltage compensation
 */
typedef struct {
    BOOL_T          enable;             // enable or disable line voltage compensation
    LVC_STA_VOLT_E  start_volt;         // start voltage selection
    LVC_SLOPE_E     slope;              // slope selection
    BOOL_T          rc_filter_en;       // enable or disable RC filter
} KP1805X_LVC_T;

/**
 * @brief RGB channels dimming
 */
typedef struct {
    DIMMING_MODE_E      mode;           // dimming mode
    DIMMING_PWM_FREQ_E  freq;           // pwm frequency
} KP1805X_RGB_DIM_T;

/**
 * @brief KP1805X dimmer configuration
 */
typedef struct {
    CURRENT_COLOR_CFG_T  max_curr_code;
    KP1805X_LVC_T        line_volt_comp; // line voltage compensation
    KP1805X_RGB_DIM_T    rgb_dimming;    // rgb channels dimming mode
} DIMMER_KP1805X_CFG_T;

/***********************************************************
********************function declaration********************
***********************************************************/
/**
 * @brief register KP18058 dimmer
 *
 * @param[in] name: dimmer name
 * @param[in] p_i2c: i2c pin config
 * @param[in] p_ch_cfg: color channel id config
 * @param[in] p_chip_cfg: chip config
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdd_dimmer_kp18058_register(CHAR_T *name, TDD_I2C_PIN_T *p_i2c,\
                                        DIMMER_CH_CFG_U *p_ch_cfg, DIMMER_KP1805X_CFG_T *p_chip_cfg);

/**
 * @brief convert rgb maximum current
 *
 * @param[in] cur: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_kp18058_convert_rgb_curr_code(UCHAR_T current_ma);

/**
 * @brief convert cw maximum current
 *
 * @param[in] cur: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_kp18058_convert_cw_curr_code(UCHAR_T current_ma);

/**
 * @brief register KP18059 dimmer
 *
 * @param[in] name: dimmer name
 * @param[in] p_i2c: i2c pin config
 * @param[in] p_ch_cfg: color channel id config
 * @param[in] p_chip_cfg: chip config
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET tdd_dimmer_kp18059_register(CHAR_T *name, TDD_I2C_PIN_T *p_i2c,\
                                        DIMMER_CH_CFG_U *p_ch_cfg, DIMMER_KP1805X_CFG_T *p_chip_cfg);

/**
 * @brief convert rgb maximum current
 *
 * @param[in] cur: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_kp18059_convert_rgb_curr_code(UCHAR_T current_ma);

/**
 * @brief convert cw maximum current
 *
 * @param[in] cur: user-set maximum current mA value
 *
 * @return maximum current code
 */
UCHAR_T tdd_dimmer_kp18059_convert_cw_curr_code(UCHAR_T current_ma);

#ifdef __cplusplus
}
#endif

#endif /* __TDD_DIMMER_KP18058_H__ */
