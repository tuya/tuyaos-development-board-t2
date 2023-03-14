#!/usr/bin/python3
# -*- coding: utf-8 -*-

import os
import sys
from ruamel import yaml  # pip install ruamel.yaml

# make.yaml
MAKE_YAML = "./make.yaml" if len(sys.argv)<2 else sys.argv[1]

def rebuild_make_yaml(make_yaml):
    f = open(make_yaml, "r")
    yaml_res = yaml.load(f.read(), yaml.RoundTripLoader)
    f.close()
    #print(yaml_res)

    if ('dependencies' not in yaml_res.keys()) or ('components' not in yaml_res['dependencies'].keys()):
        return

    #yaml_comp_list = yaml_res['dependencies']['components']
    #yaml_vend_list = yaml_res['dependencies']['toolchains']

    '''
    ruamel.yaml这个库本身存在问题，['key': {'name': 'hello'}]会被解析成
    [{'key': None, 'name': 'hello'}]
    只能在数据处理结束后，再处理一次格式。
    （下面的处理虽然很丑，但是有效）
    '''
    yaml_res_copy = yaml_res.copy()
    for key in yaml_res['dependencies'].keys():

        # move components according the import source
        components_list = []
        if key == 'components':
            items = yaml_res['dependencies'][key]
            for i in items:
                #print(i.keys())
                item_key = ''
                for j in i.keys():
                    #print(j)
                    item_key = j
                    break
                i.pop(item_key)
                if 'kernel' in i['import']:
                    i['locater'] = 'adapter'
                else:
                    i['locater'] = 'components'
                components_list.append({item_key: i})
            yaml_res_copy['dependencies'][key] = components_list

        # move toolchains locater to vendor
        vendors_list = []
        if key == 'toolchains':
            items = yaml_res['dependencies'][key]
            for i in items:
                item_key = ''
                for j in i.keys():
                    item_key = j
                    break
                i.pop(item_key)
                i['locater'] = 'vendor'
                vendors_list.append({item_key: i})
            yaml_res_copy['dependencies'][key] = vendors_list

    # update make.yaml
    yaml_str = yaml.dump(yaml_res_copy, Dumper=yaml.RoundTripDumper, indent=2, block_seq_indent=2)
    #print(yaml_str)
    with open(make_yaml, "w") as w:
        w.write(yaml_str)
    pass


def main():
    if not os.path.exists(MAKE_YAML):
        print(f'path error: {MAKE_YAML}')
        exit()

    rebuild_make_yaml(MAKE_YAML)
    print("-----change yaml success-----")
    pass


if __name__ == '__main__':
    main()
    pass
