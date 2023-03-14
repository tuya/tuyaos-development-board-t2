/**
 * @file dp_process.h
 * @author www.tuya.com
 * @brief dp_process module is used to 
 * @version 0.1
 * @date 2022-10-28
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#ifndef __DP_PROCESS_H__
#define __DP_PROCESS_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************macro define************************
***********************************************************/
#define DPID_SWITCH   20

/***********************************************************
***********************typedef define***********************
***********************************************************/


/***********************************************************
********************function declaration********************
***********************************************************/

/**
 * @brief upload all dp data
 *
 * @param[in] none
 *
 * @return none
 */
VOID_T update_all_dp(VOID_T);

/**
 * @brief handle dp commands from the cloud
 *
 * @param[in] root: pointer header for dp data
 *
 * @return none
 */
VOID_T deal_dp_proc(IN CONST TY_OBJ_DP_S *root);

#ifdef __cplusplus
}
#endif

#endif /* __DP_PROCESS_H__ */
