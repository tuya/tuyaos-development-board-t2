#!/usr/bin/env python3
# -*- coding: utf-8 -*-
##
# @file set_app_config.py
# @brief 解析yaml文件，查找组件和工具链目录下是否存在APPconfig文件，
#        如果存在则source到一个总APPconfig文件中
# @author huatuo
# @version 1.0.0
# @date 2022-07-28


import os
import sys
import shutil
from ruamel import yaml  # pip3 install ruamel.yaml

APP_NAME = "eswin_std_demo_340" if len(sys.argv)<2 else sys.argv[1]
PLATFORM = None if len(sys.argv)<3 else sys.argv[2]
APP_PATH = os.path.join("apps", APP_NAME)
MAKE_YAML = os.path.join(APP_PATH, "app.yaml")
APPCONFIG = os.path.join(APP_PATH, "build", "APPconfig")
DEFAULT_CONFIG = os.path.join(APP_PATH, "build", "APPconfig.default")

APP_CONTEXT_PREFIX = '''
menu "APPLICATION"
'''
COMP_CONTEXT_PREFIX = '''
menu "APP_COMPONENT"
'''
DRIVER_CONTEXT_PREFIX = '''
menu "APP_DIRVER"
'''
VENDOR_CONTEXT_PREFIX = '''
menu "VENDOR"
'''
SUFFIX = '''endmenu
'''

def use_default_config(default, config):
    if os.path.exists(default):
        shutil.copyfile(default, config)
        return True

    return False


def set_iotosconfig(make_yaml, osconfig, platform):
    f = open(make_yaml, "r")
    yaml_res = yaml.load(f.read(), yaml.RoundTripLoader)
    f.close()

    if 'dependencies' not in yaml_res.keys():
        return False

    dependencies = yaml_res['dependencies']

    app_context = ""
    config_path = os.path.join(APP_PATH, 'APPconfig')
    if os.path.exists(config_path):
        app_context += ('    rsource '+'../APPconfig'+'\n')
    if app_context:
        app_context = APP_CONTEXT_PREFIX + app_context + SUFFIX

    comp_context = ""
    comp_deps = dependencies.get("application_components", [])
    for block in comp_deps:
        if "Component" in block.keys():
            d = block['Component']
            if ('name' not in d.keys()) or ('locater' not in d.keys()):
                continue
            name = d['name']
            locater = d['locater']
            config_path = os.path.join(locater, name, 'APPconfig')
            ex_config_path = os.path.join("apps/"+APP_NAME, locater, name, 'APPconfig')
            if os.path.exists(ex_config_path):
                comp_context += ('    rsource '+'../'+config_path+'\n')
            elif os.path.exists(config_path):
                comp_context += ('    rsource '+'../../../'+config_path+'\n')
    if comp_context:
        comp_context = COMP_CONTEXT_PREFIX + comp_context + SUFFIX

    driver_context = ""
    driver_deps = dependencies.get("application_drivers", [])
    for block in driver_deps:
        if "Component" in block.keys():
            d = block['Component']
            if ('name' not in d.keys()) or ('locater' not in d.keys()):
                continue
            name = d['name']
            locater = d['locater']
            config_path = os.path.join(locater, name, 'APPconfig')
            ex_config_path = os.path.join("apps/"+APP_NAME, locater, name, 'APPconfig')
            if os.path.exists(ex_config_path):
                driver_context += ('    rsource '+'../'+config_path+'\n')
            elif os.path.exists(config_path):
                driver_context += ('    rsource '+'../../../'+config_path+'\n')
    if driver_context:
        driver_context = DRIVER_CONTEXT_PREFIX + driver_context + SUFFIX

    vendor_context = ""
    if platform == None:
        sdk_deps = dependencies.get("sdkproducts", [])
        for block in sdk_deps:
            if "SDKProduct" in block.keys():
                d = block['SDKProduct']
                if ('toolchain' not in d.keys()):
                    continue
                t = d['toolchain']
                platform = t['name']
    if platform:
        config_path = os.path.join("vendor", platform, 'APPconfig')
        if os.path.exists(config_path):
            vendor_context += ('    rsource '+'../../../'+config_path+'\n')
    if vendor_context:
        vendor_context = VENDOR_CONTEXT_PREFIX + vendor_context + SUFFIX

    context = 'mainmenu "Tuya Application Configuration"\n'
    context += app_context + comp_context + driver_context + vendor_context

    with open(osconfig, 'w') as f:
        f.write(context)

    return True


def main():
    if not os.path.exists(MAKE_YAML):
        print("path error: ", MAKE_YAML)
        exit()
    config_dir = os.path.dirname(APPCONFIG)
    if not os.path.exists(config_dir):
        os.makedirs(config_dir)

    if use_default_config(DEFAULT_CONFIG, APPCONFIG):
        return
    set_iotosconfig(MAKE_YAML, APPCONFIG, PLATFORM)
    pass


if __name__ == '__main__':
    main()
    pass

