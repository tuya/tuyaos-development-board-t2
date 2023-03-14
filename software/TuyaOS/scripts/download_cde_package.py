##
# @file download_cde_package.py
# @brief 下载CDE平台上的SDK产物
# @author huatuo
# @version 1.0.0
# @date 2021-11-12

#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import json
import requests
import tarfile
from ruamel import yaml  # pip3 install ruamel.yaml


def get_proj_info(make_yaml, toolchain_name):
    f = open(make_yaml, "r", encoding="utf-8")
    yaml_res = yaml.load(f.read(), yaml.RoundTripLoader)
    f.close()

    proj_info = yaml_res['info']

    if 'dependencies' not in yaml_res.keys():
        print("[error] make.yaml has no 'dependencies'")
        exit(1)

    dependencies = yaml_res['dependencies']
    toolchain_list = dependencies['vendor']
    for block in toolchain_list:
        if "Toolchain" in block.keys():
            t = block['Toolchain']
            if t['name'] == toolchain_name:
                proj_info['toolchain_ver'] = t['version']
                break
    else:
        print("[error] make.yaml can't find toolchain: {toolchain_name}")
        exit(1)

    return proj_info

def get_package_url(proj_id, package_name, cdetoken):
    cde_api_host = "https://cde-api-cn.tuya-inc.com:7799"  # 线上
    #  cde_api_host = "https://cde-open.wgine-daily.com:7799/cde"  # 日常
    cde_url = cde_api_host + "/projects/" + str(proj_id) + "/assets"
    print("cde_url: ", cde_url)

    headers = {
        "cde-token": cdetoken,
        "Content-Type": "application/json",
    }
    params = {
        "full_name": package_name,
    }

    response = requests.get(cde_url,
                    headers=headers,
                    params=params,
                    verify=False
                    )
    print("response: ", response)
    print(response.text)

    package_info_list = json.loads(response.text)['result']['data']
    if not package_info_list:
        print("[error] not found package: " + package_name)
        exit(1)

    package_info = package_info_list[0]
    package_url = package_info['url']
    print(package_url)

    return package_url


def download_package(url, path, package_name):
    if not os.path.exists(path):
        os.makedirs(path)

    f = requests.get(url)

    status_code = f.status_code
    print ('request code:', status_code)

    if status_code != 200:
        print("download error !")
        exit(1)

    tar_name = path + '/' + package_name
    with open(tar_name, "wb") as code:
        code.write(f.content)

    tar = tarfile.open(tar_name)
    tar.extractall(path)
    tar.close()

    pass


def main():
    toolchain_name = sys.argv[1]
    config_name = sys.argv[2]
    cdetoken = sys.argv[3]

    proj_info = get_proj_info("make.yaml", toolchain_name)

    proj_name = proj_info['name']
    proj_ver = proj_info['version']
    proj_id = proj_info['proj_id']
    toolchain_ver = proj_info['toolchain_ver']

    cfg_name = config_name[:-7]
    if config_name[0:len(toolchain_name)+1] == toolchain_name+'_':
        cfg_name = cfg_name[len(toolchain_name)+1:]
    package_name = proj_name + "_" + proj_ver + "_" \
        + toolchain_name + "_" + cfg_name + "_" + toolchain_ver \
        + ".tar.gz"
    print("package_name: " + package_name)

    package_url = get_package_url(proj_id, package_name, cdetoken)
    download_package(package_url, "./tmp", package_name)

    pass


if __name__ == '__main__':
    main()
    pass

