/**
 * @file app_temp_humi.h
 * @brief RH-T sensor application demo
 * @version 0.1
 * @date 2022-09-01
 */

#ifndef __APP_TEMP_HUMI_H__
#define __APP_TEMP_HUMI_H__

#include "tuya_cloud_com_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************macro define************************
***********************************************************/
/*
 *@brief product id define
*/
#define PID "8q46zzdfuofastqr"

/**
 * @brief function define
 */
#define APP_TEMP_HUMI_READ_CYCLE            2  * 1000               // units：ms
#define APP_TEMP_HUMI_REPORT_CYCLE          10 * 1000               // units：ms

/**
 * @brief pin define
 */
#define SHT30_SCL_PIN   GPIO_NUM_20
#define SHT30_SDA_PIN   GPIO_NUM_22
#define SHT30_ALT_PIN   GPIO_NUM_14

/**
 * @brief DP list
 */
#define DP_ID_TEMP      1
#define DP_ID_HUMI      2

#define DP_ID_TEMP_MAX  10
#define DP_ID_TEMP_MIN  11
#define DP_ID_HUMI_MAX  12
#define DP_ID_HUMI_MIN  13

#define DP_ID_TEMP_ALT  14
#define DP_ID_HUMI_ALT  15

#define DP_ID_SWITCH    21

#define MAX_DP_NUM          9

/**
 * @brief Data scale
 */
#define TEMP_SCALE      1           // raw-data * 10
#define HUMI_SCALE      1           // raw-data * 10

/**
 *@brief report interval
 */
#define APP_TEMP_REPORT_INTERVAL        20          // default: 2.0 ℃
#define APP_HUMI_REPORT_INTERVAL        20           // default: 2.0 %

/**
 *@brief kv key name
 */
#define KEY_NAME "temp_humi"

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
********************function declaration********************
***********************************************************/
/**
 * @brief temp_humi main
 *
 * @param none
 *
 * @return OPRT_OK on success. Others on error, please refer to "tuya_error_code.h"
 */
OPERATE_RET app_temp_humi_init(VOID_T);

/**
 * @brief DP processing
 *
 * @param[in] dp: dp data
 *
 * @return none
 */
VOID_T app_temp_humi_dp_proc(CONST TY_OBJ_DP_S *dp);

/**
 * @brief report all DP data
 *
 * @param none
 *
 * @return none
 */
VOID_T app_temp_humi_repo_all_dp(VOID_T);

#ifdef __cplusplus
}
#endif

#endif /* __APP_TEMP_HUMI_H__ */
