#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import os
import json

INPUT_H = "./include/base/include/tuya_iot_config.h" if len(sys.argv)<2 else sys.argv[1]
OUTPUT_JSON = "./build/tuya_iot_ablity.json" if len(sys.argv)<3 else sys.argv[2]

##
# @name 能力值信息表
# @{ 
# 注意：修改此表请阅读：https://wiki.tuya-inc.com:7799/page/1468025033673678859#8504B6
# 仅支持boool类型和枚举类型，含义说明如下：
# key值代表头文件中宏定义的标识符部分
# "lable"字段为多语言平台上的code值
# "enum"中key值代表头文件中宏定义的字符串部分
# "enum"中value值为多语言平台上的code值
# 最终的能力值将以多语言平台的配置结果显示
##  @} 
ABLITIES = {
    "ENABLE_BT_NETCFG": {"lable": "EmbAbt_ENABLE_BT_NETCFG"},
    "ENABLE_WIFI_PEGASUS": {"lable": "EmbAbt_ENABLE_WIFI_PEGASUS"},
    "ENABLE_WIFI_FFS": {"lable": "EmbAbt_ENABLE_WIFI_FFS"},
    "ENABLE_QRCODE_ACTIVE": {"lable": "EmbAbt_ENABLE_QRCODE_ACTIVE"},
    "ENABLE_BT_SERVICE": {"lable": "EmbAbt_ENABLE_BT_SERVICE"},
    "ENABLE_BT_ACTIVE": {"lable": "EmbAbt_ENABLE_BT_ACTIVE"},
    "ENABLE_TUYA_LAN": {"lable": "EmbAbt_ENABLE_TUYA_LAN"},
    "ENABLE_BT_CTRL": {"lable": "EmbAbt_ENABLE_BT_CTRL"},
    "ENABLE_WIFI_FFC": {"lable": "EmbAbt_ENABLE_WIFI_FFC"},
    "ENABLE_BT_REMOTE_CTRL": {"lable": "EmbAbt_ENABLE_BT_REMOTE_CTRL"},
    "ENABLE_ASTRO_TIMER": {"lable": "EmbAbt_ENABLE_ASTRO_TIMER"},
    "ENABLE_CALENDAR_TIMER": {"lable": "EmbAbt_ENABLE_CALENDAR_TIMER"},
    "ENABLE_BT_TIMER": {"lable": "EmbAbt_ENABLE_BT_TIMER"},
    "ENABLE_WIFI_PROTECT": {"lable": "EmbAbt_ENABLE_WIFI_PROTECT"},
    "ENABLE_HOMEKIT": {"lable": "EmbAbt_ENABLE_HOMEKIT"},
    "ENABLE_MATTER": {"lable": "EmbAbt_ENABLE_MATTER"},
    "ENABLE_WEBSOCKET": {"lable": "EmbAbt_ENABLE_WEBSOCKET"},
    "ENABLE_LWIP": {"lable": "EmbAbt_ENABLE_LWIP"},
    "ENABLE_HOSTAPD": {"lable": "EmbAbt_ENABLE_HOSTAPD"},
    "ENABLE_TUYA_BLE_STACK": {"lable": "EmbAbt_ENABLE_TUYA_BLE_STACK"},
    "ENABLE_KV_PROTECTED": {"lable": "EmbAbt_ENABLE_KV_PROTECTED"},
    "ENABLE_RTSP_SERVER": {"lable": "EmbAbt_ENABLE_RTSP_SERVER"},
    "ENABLE_LOCAL_KEY_UPDATE": {"lable": "EmbAbt_ENABLE_LOCAL_KEY_UPDATE"},
    "ENABLE_DEVICE_DATA_COLLECT": {"lable": "EmbAbt_ENABLE_DEVICE_DATA_COLLECT"},
    "ENABLE_COMMUNICATE_PRIORITY": {"lable": "EmbAbt_ENABLE_COMMUNICATE_PRIORITY"},
    "ENABLE_MATOP": {"lable": "EmbAbt_ENABLE_MATOP"},
    "TUYA_SECURITY_LEVEL": {"lable": "EmbAbt_TUYA_SECURITY_LEVEL", "enum": {
        "0": "EmbAbt_TUYA_SECURITY_LEVEL_0",
        "1": "EmbAbt_TUYA_SECURITY_LEVEL_1",
        "2": "EmbAbt_TUYA_SECURITY_LEVEL_2",
        "3": "EmbAbt_TUYA_SECURITY_LEVEL_3",
    }},
}


def h2value(input_h):
    with open(input_h, 'r', encoding='utf8') as file_object :
        lines = file_object.readlines()
    value_out = {}
    for line in lines :
        if not line.startswith("#define "):
            continue
        line_split = line.split()
        ablity = line_split[1]
        if ablity in value_out.keys():  # 有可能存在重复定义
            continue
        if ablity not in ABLITIES.keys():
            continue
        value = line_split[2]
        value_out[ablity] = value

    #  print(f'value_out: {value_out}')
    return value_out


def value2ablity(value_list):
    ablity_out = {}
    ablity_keys = ABLITIES.keys()
    value_keys = value_list.keys()

    for abt in ablity_keys:
        lable = ABLITIES[abt]['lable']
        value = "false"  # Bool值未设定、枚举值未命中
        if abt in value_keys:
            if "enum" in ABLITIES[abt].keys():
                vv = value_list[abt]
                if vv in ABLITIES[abt]['enum'].keys():
                    value = ABLITIES[abt]['enum'][vv]  # 命中枚举值
            else:
                value = "true"  # Bool类型被置1
        ablity_out[lable] = value

    #  print(f'ablity_out: {ablity_out}')
    return ablity_out


def out_json(ablities, out_json):
    json_str = json.dumps(ablities, indent=4, ensure_ascii=False)
    with open(out_json, 'w', encoding='utf8') as f:
        f.write(json_str)
    pass


if __name__ == '__main__':
    if not os.path.exists(INPUT_H):
        print("file not found: ", INPUT_H)
        exit()
    values = h2value(INPUT_H)
    ablities = value2ablity(values)
    out_json(ablities, OUTPUT_JSON)
    pass
