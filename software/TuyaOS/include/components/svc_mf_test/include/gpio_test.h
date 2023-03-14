/**
* @file gpio_test.h
* @brief Common process - mf test of gpio
* @version 0.1
* @date 2018-05-02
*
* @copyright Copyright 2018-2021 Tuya Inc. All Rights Reserved.
*
*/
#ifndef _GPIO_TEST_H
#define _GPIO_TEST_H

#include "tuya_pin.h"
#include "tuya_iot_config.h"

/**
 * @brief Definition group of control
 */
typedef struct {
    INT_T   ionum;
    tuya_pin_name_t iopin[8];
} CTRL_GROUP;

/**
 * @brief Definition gpio test table
 *
 */
typedef struct {
    INT_T group_num;
    CTRL_GROUP group[20];
} GPIO_TEST_TABLE;

/**
 * @brief enable gpio test
 *
 * @param[in] enable Flag to enable
 *
 */
VOID gpio_test_enabe(BOOL_T enable);

/**
 * @brief callback for gpio test
 *
 * @param[in] table Test content
 *
 * @return TRUE/FLASE
 */
BOOL_T gpio_test_cb(GPIO_TEST_TABLE * table);

/**
 * @brief gpio test function
 *
 * @param[in] in Test content in
 * @param[out] out Test content out
 *
 * @return TRUE/FLASE
 */
BOOL_T gpio_test_all(IN CONST CHAR_T *in, OUT CHAR_T *out);

#endif
