/**
 * @file tuya_cli_register.c
 * @author www.tuya.com
 * @brief tuya_cli_register module is used to 
 * @version 0.1
 * @date 2022-09-21
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include "tuya_cloud_types.h"
#include "tuya_cli_register.h"

/***********************************************************
************************macro define************************
***********************************************************/


/***********************************************************
***********************typedef define***********************
***********************************************************/


/***********************************************************
********************function declaration********************
***********************************************************/
extern VOID example_adc(INT_T argc, CHAR_T *argv[]);
extern VOID example_pwm(INT_T argc, CHAR_T *argv[]);
extern VOID example_gpio(INT_T argc, CHAR_T *argv[]);
extern VOID example_spi(INT_T argc, CHAR_T *argv[]);
extern VOID example_timer(INT_T argc, CHAR_T *argv[]);
#if 0
extern VOID example_i2c(INT_T argc, CHAR_T *argv[]);
#endif

extern VOID example_thread(INT_T argc, CHAR_T *argv[]);
extern VOID example_sw_timer(INT_T argc, CHAR_T *argv[]);
extern VOID example_semaphore(INT_T argc, CHAR_T *argv[]);
extern VOID example_semaphore_stop(INT_T argc, CHAR_T *argv[]);
extern VOID example_queue(INT_T argc, CHAR_T *argv[]);
extern VOID example_queue_stop(INT_T argc, CHAR_T *argv[]);
extern VOID example_mutex(INT_T argc, CHAR_T *argv[]);
extern VOID example_mutex_stop(INT_T argc, CHAR_T *argv[]);

extern VOID example_feed_watchdog(INT_T argc, CHAR_T *argv[]);

extern VOID example_ble_central(INT_T argc, CHAR_T *argv[]);
extern VOID example_ble_peripheral(INT_T argc, CHAR_T *argv[]);

extern VOID example_kv_common(INT_T argc, CHAR_T *argv[]);
extern VOID example_kv_fuzzy(INT_T argc, CHAR_T *argv[]);
extern VOID example_kv_protect(INT_T argc, CHAR_T *argv[]);
extern VOID example_kv_tuya_db(INT_T argc, CHAR_T *argv[]);
extern VOID example_uf_db(INT_T argc, CHAR_T *argv[]);
extern VOID example_uf_file(INT_T argc, CHAR_T *argv[]);

extern VOID example_wifi_ap(INT_T argc, CHAR_T *argv[]);
extern VOID example_wifi_sta(INT_T argc, CHAR_T *argv[]);
extern VOID example_wifi_scan(INT_T argc, CHAR_T *argv[]);
extern VOID example_wifi_low_power(INT_T argc, CHAR_T *argv[]);

extern VOID example_ble_remote(INT_T argc, CHAR_T *argv[]);
extern VOID example_ffc_master_init(INT_T argc, CHAR_T *argv[]);
extern VOID example_ffc_master_send(INT_T argc, CHAR_T *argv[]);
extern VOID example_ffc_slave_init(INT_T argc, CHAR_T *argv[]);

extern VOID example_output_free_heap(INT_T argc, CHAR_T *argv[]);
extern VOID example_soc_device_remove(INT_T argc, CHAR_T *argv[]);
extern VOID example_soc_device_init(INT_T argc, CHAR_T *argv[]);
extern VOID example_update_device_status(INT_T argc, CHAR_T *argv[]);
extern VOID example_dp_bool(INT_T argc, CHAR_T *argv[]);
extern VOID example_dp_enum(INT_T argc, CHAR_T *argv[]);
extern VOID example_dp_value(INT_T argc, CHAR_T *argv[]);
extern VOID example_dp_str(INT_T argc, CHAR_T *argv[]);
extern VOID example_dp_raw(INT_T argc, CHAR_T *argv[]);

extern VOID example_tcp_server(INT_T argc, CHAR_T *argv[]);
extern VOID example_tcp_client(INT_T argc, CHAR_T *argv[]);

extern VOID example_health_manage(INT_T argc, CHAR_T *argv[]);

extern VOID example_http(INT_T argc, CHAR_T *argv[]);
extern VOID example_http_download(INT_T argc, CHAR_T *argv[]);

extern VOID command_list_output(INT_T argc, CHAR_T *argv[]);

VOID example_reboot(INT_T argc, CHAR_T *argv[]);

/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC CLI_CMD_T __cmd_register_list[] = {
#if defined(ENABLE_ADC) && (ENABLE_ADC == 1)
    {"example_adc", "Output Sample value", example_adc},
#endif

#if defined(ENABLE_GPIO) && (ENABLE_GPIO == 1)
    {"example_gpio", "init gpio and read write every 2 seconds", example_gpio},
#endif

#if 0
    {"example_i2c", "i2c example", example_i2c},
#endif

#if defined(ENABLE_PWM) && (ENABLE_PWM == 1)
    {"example_pwm", "init pwm and change the duty and frequency every 2 seconds", example_pwm},
#endif

#if defined(ENABLE_SPI) && (ENABLE_SPI == 1)
    {"example_spi", "init and start spi0", example_spi},
#endif

#if defined(ENABLE_TIMER) && (ENABLE_TIMER == 1)
    {"example_timer", "Init and start timer0, enter callback 5 times to stop the timer", example_timer},
#endif

#if defined(ENABLE_DAC) && (ENABLE_DAC == 1)
#endif

    {"example_thread", "creat a thread, then delete", example_thread},
    {"example_sw_timer", "creat a software timer", example_sw_timer},
    {"example_semaphore", "semaphore example, you can execute the \"example_semaphore_stop\" command to stop it", example_semaphore},
    {"example_semaphore_stop", "stop semaphore example", example_semaphore_stop},
    {"example_queue", "queue example, you can execute the \"example_queue_stop\" command to stop it", example_queue},
    {"example_queue_stop", "stop queue example", example_queue_stop},
    {"example_mutex", "mutex example, you can execute the \"example_mutex_stop\" command to stop it", example_mutex},
    {"example_mutex_stop", "stop mutex example", example_mutex_stop},

#if defined(ENABLE_BT_SERVICE) && (ENABLE_BT_SERVICE == 1)
    {"example_ble_central", "Ble central example", example_ble_central},
    {"example_ble_peripheral", "Ble peripheral example", example_ble_peripheral},
#endif

    /* kv database */
    {"example_kv_common", "Key-value database example", example_kv_common},
    {"example_kv_fuzzy", "Fuzzy key search from key-value database", example_kv_fuzzy},
#if defined(ENABLE_KV_PROTECTED) && (ENABLE_KV_PROTECTED == 1)
    {"example_kv_protect", "Protected operations in kv database", example_kv_protect},
#endif
    {"example_kv_tuya_db", "Read firmware information from kv database", example_kv_tuya_db},

    {"example_uf_db", "uf database example", example_uf_db},
    {"example_uf_file", "uf file example", example_uf_file},

#if defined(ENABLE_WIFI_SERVICE) && (ENABLE_WIFI_SERVICE == 1)
    {"example_wifi_ap", "SoftAP, SSID: \"my-wifi\", password: \"12345678\"", example_wifi_ap},
    {"example_wifi_sta", "Wi-Fi station connect, Please enter the SSID and password in the code.", example_wifi_sta},
    {"example_wifi_scan", "Scan for nearby Wi-Fi", example_wifi_scan},
    {"example_wifi_low_power", "Wi-Fi low power", example_wifi_low_power},
#endif

    {"example_ble_remote", "Can accept ble remote control data.", example_ble_remote},

#if defined(TUYA_WIFI_FFC_MASTER) && (TUYA_WIFI_FFC_MASTER == 1)
    {"example_ffc_master_init", "ffc master init and bind", example_ffc_master_init},
    {"example_ffc_master_send", "ffc master send data, 0x00-0x09", example_ffc_master_send},
#endif

#if defined(TUYA_WIFI_FFC_SLAVER) && (TUYA_WIFI_FFC_SLAVER == 1)
    {"example_ffc_slave_init", "ffc slave init and bind", example_ffc_slave_init},
#endif

    {"example_output_free_heap", "Output free heap every 3 seconds.", example_output_free_heap},
    {"example_soc_init", "After initialization, you can use the Tuya Smart App to operate.", example_soc_device_init},
    {"example_update_device_status", "Report current device status to the cloud.", example_update_device_status},
    {"example_soc_remove", "Remove the device to reconnect it to the network.", example_soc_device_remove},
    {"example_dp_bool", "Report a bool type data to the cloud.", example_dp_bool},
    {"example_dp_enum", "Report a enum type data to the cloud.", example_dp_enum},
    {"example_dp_value", "Report a value type data to the cloud.", example_dp_value},
    {"example_dp_str", "Report a string type data to the cloud.", example_dp_str},
    {"example_dp_raw", "Report a raw type data to the cloud.", example_dp_raw},

    {"example_tcp_server", "tcp server", example_tcp_server},
    {"example_tcp_client", "tcp client", example_tcp_client},

#if defined(ENABLE_WATCHDOG) && (ENABLE_WATCHDOG == 1)
    {"example_feed_watchdog", "Feed watchdog, you need to first call \"example_soc_init\".", example_feed_watchdog},
#endif

    {"example_health_manage", "Health manage add item example, you need to first call \"example_soc_init\".", example_health_manage},

    {"example_http", "http post/get example, please connect to the network first.", example_http},
    {"example_http_download", "Download a file, please connect to the network first.", example_http_download},

    {"reboot", "Reboot device.", example_reboot},
    {"help", "Output examples list.", command_list_output},
};

/***********************************************************
***********************function define**********************
***********************************************************/

VOID examples_cli_register(VOID_T)
{
    tuya_cli_cmd_register(__cmd_register_list, CNTSOF(__cmd_register_list));
}

extern VOID_T tkl_system_reset(VOID_T);
VOID example_reboot(INT_T argc, CHAR_T *argv[])
{
    tkl_system_reset();
}
