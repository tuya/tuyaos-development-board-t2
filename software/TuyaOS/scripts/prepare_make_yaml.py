#!/usr/bin/python3
# -*- coding: utf-8 -*-

import os
import sys
from ruamel import yaml  # pip install ruamel.yaml

MAKE_YAML = "./make.yaml" if len(sys.argv)<2 else sys.argv[1]
CONFIG = "./build/tuya_iot.config" if len(sys.argv)<3 else sys.argv[2]

def get_enable_list(config_file):
    conf_handle = open(config_file, 'r')
    conf_lines = conf_handle.readlines()

    ans = []
    prefix = "CONFIG_ENABLE_"
    len_prefix = len(prefix)
    for l in conf_lines:
        l = l.strip()
        if not l.startswith(prefix):
            continue
        key = l.split('=', 1)[0]
        value = l.split('=', 1)[1]
        if value == 'y' or value == '1':
            comp = key[len_prefix:]
            ans.append(comp.lower())
    return ans

def rebuild_make_yaml(enable_list, make_yaml):
    f = open(make_yaml, "r")
    yaml_res = yaml.load(f.read(), yaml.RoundTripLoader)
    f.close()
    #  print(yaml_res)

    if ('dependencies' not in yaml_res.keys()) or ('components' not in yaml_res['dependencies'].keys()):
        return

    yaml_comp_list = yaml_res['dependencies']['components']
    comp_list = yaml_comp_list.copy()

    '''
    只有明确使能的组件才去下载
    '''
    for c in yaml_comp_list:
        if c['name'] in enable_list:
            continue
        comp_list.remove(c)

    yaml_res['dependencies']['components'] = comp_list

    '''
    ruamel.yaml这个库本身存在问题，['key': {'name': 'hello'}]会被解析成
    [{'key': None, 'name': 'hello'}]
    只能在数据处理结束后，再处理一次格式。
    （下面的处理虽然很丑，但是有效）
    '''
    yaml_res_copy = yaml_res.copy()
    for key in yaml_res['dependencies'].keys():
        items = yaml_res['dependencies'][key]
        item_list = []
        for i in items:
            item_key = ''
            for j in i.keys():
                item_key = j
                break
            i.pop(item_key)
            item_list.append({item_key: i})
        yaml_res_copy['dependencies'][key] = item_list

    yaml_str = yaml.dump(yaml_res_copy, Dumper=yaml.RoundTripDumper, indent=2, block_seq_indent=2)
    with open(make_yaml, "w") as w:
        w.write(yaml_str)
    pass


def main():
    if not os.path.exists(MAKE_YAML):
        print(f'path error: {MAKE_YAML}')
        exit()
    if not os.path.exists(CONFIG):
        print(f'path error: {CONFIG}')
        exit()
    enable_list = get_enable_list(CONFIG)
    rebuild_make_yaml(enable_list, MAKE_YAML)
    print("-----change yaml success-----")
    pass


if __name__ == '__main__':
    main()
    pass
