
#ifndef __TUYA_OS_ADAPT_WIFI_H__
#define __TUYA_OS_ADAPT_WIFI_H__

#include "tal_sleep.h"
#include "tuya_os_adapter.h"

#define TY_WF_EVENT                 WF_EVENT_E

#define TY_WIFI_CONNECTED           WFE_CONNECTED
#define TY_WIFI_CONNECT_FAILED      WFE_CONNECT_FAILED
#define TY_WIFI_DISCONNECTED        WFE_DISCONNECTED
#define tuya_hal_wifi_init          tal_wifi_init

#endif
