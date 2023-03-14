#!/usr/bin/env python3
# -*- coding: utf-8 -*-
##
# @file set_iotosconfig.py
# @brief 解析yaml文件，查找组件和工具链目录下是否存在IoTOSconfig文件，
#        如果存在则source到一个总IoTOSconfig文件中
# @author huatuo
# @version 1.0.0
# @date 2021-10-10


import os
import sys
from ruamel import yaml  # pip3 install ruamel.yaml

MAKE_YAML = "./make.yaml" if len(sys.argv)<2 else sys.argv[1]
OSCONFIG = "./build/IoTOSconfig" if len(sys.argv)<3 else sys.argv[2]
PLATFORM = None if len(sys.argv)<4 else sys.argv[3]

SDK_CONTEXT_PREFIX = '''
menu "SDK"
'''
KERNEL_CONTEXT_PREFIX = '''
menu "KERNEL"
'''
VENDOR_CONTEXT_PREFIX = '''
menu "VENDOR"
'''
SUFFIX = '''endmenu
'''


def set_iotosconfig(make_yaml, osconfig, platform):
    f = open(make_yaml, "r")
    yaml_res = yaml.load(f.read(), yaml.RoundTripLoader)
    f.close()

    if 'dependencies' not in yaml_res.keys():
        return False

    dependencies = yaml_res['dependencies']

    sdk_context = ""
    sdk_deps = dependencies.get("components", [])
    for block in sdk_deps:
        if "Component" in block.keys():
            d = block['Component']
            if ('name' not in d.keys()) or ('locater' not in d.keys()):
                continue
            name = d['name']
            locater = d['locater']
            config_path = os.path.join(locater, name, 'IoTOSconfig')
            if os.path.exists(config_path):
                sdk_context += ('    rsource '+'../'+config_path+'\n')
    if sdk_context:
        sdk_context = SDK_CONTEXT_PREFIX + sdk_context + SUFFIX

    context = 'mainmenu "Tuya IoT SDK Configuration"\n'
    context += sdk_context

    with open(osconfig, 'w') as f:
        f.write(context)

    return True


def main():
    if not os.path.exists(MAKE_YAML):
        print("path error: ", MAKE_YAML)
        exit()
    config_dir = os.path.dirname(OSCONFIG)
    if not os.path.exists(config_dir):
        os.makedirs(config_dir)

    set_iotosconfig(MAKE_YAML, OSCONFIG, PLATFORM)

    pass


if __name__ == '__main__':
    main()
    pass

