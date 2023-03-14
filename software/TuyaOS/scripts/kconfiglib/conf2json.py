##
# @file conf2json.py
# @brief 输入.config文件，输出json格式的特征值文件
# @author huatuo
# @version 1.0.0
# @date 2021-05-28
# @note 过程中需要用到json格式的特征值模板

#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import os
import json

MYDIR = os.path.abspath(os.path.dirname(__file__))

def conf2json(conf, json_file, template):
    if not os.path.exists(conf):
        print("error path: ", conf)
        return -1
    template = template if len(template) else os.path.join(MYDIR, 'feature_template.json')
    if not os.path.exists(template):
        print("error path: ", template)
        return -1

    def_data = {}
    conf_f = open(conf, 'r', encoding="utf-8")
    conf_lines = conf_f.readlines()
    for l in conf_lines:
        if not l.startswith("CONFIG_"):
            continue
        ori_key = l.split('=', 1)[0]
        ori_value = l.split('=', 1)[1].rstrip('\n')
        def_key = ori_key.replace("CONFIG_", '', 1)
        def_value = ori_value if ori_value[0] != 'y' else ori_value.replace('y', '1', 1)
        def_data[def_key] = def_value

    json_data = json.load(open(template, 'r', encoding='utf8'))
    for f in json_data:
        if not f['tag'] in def_data.keys():
            continue
        f['value'] = def_data[f['tag']]

    json_str = json.dumps(json_data, indent=4, ensure_ascii=False)
    with open(json_file, 'w', encoding="utf-8") as f:
        f.write(json_str)


if __name__ == '__main__':
    conf_file=sys.argv[1]
    json_file=sys.argv[2]
    template = ""
    if len(sys.argv) > 3:
        template = sys.argv[3]

    conf2json(conf_file, json_file, template)
    pass
