##
# @file build_compile_info.py
# @brief 生成编译详情
# @author huatuo
# @version 1.0.0
# @date 2021-07-29

#!/usr/bin/python3
# -*- coding: utf-8 -*-

import sys
import os
import json

INFO_OUT_JSON = sys.argv[1]
COMP_INFO_DIR = sys.argv[2]

def main():
    if not os.path.exists(INFO_OUT_JSON):
        print("error json: ", INFO_OUT_JSON)
        return
    if not os.path.exists(COMP_INFO_DIR):
        print("error info dir: ", COMP_INFO_DIR)
        return

    f = open(INFO_OUT_JSON, 'r', encoding='utf8')
    json_data = json.load(f)
    f.close()
    json_data['components'] = []

    comp_info_list = os.listdir(COMP_INFO_DIR)
    for i in comp_info_list:
        filename, filetype = os.path.splitext(i)
        if filetype != '.json':
            continue
        f = open(os.path.join(COMP_INFO_DIR, i), 'r', encoding='utf8')
        comp_data = json.load(f)
        f.close()
        json_data['components'].append(comp_data)

    json_str = json.dumps(json_data, indent=4, ensure_ascii=False)
    with open(INFO_OUT_JSON, 'w') as f:
        f.write(json_str)
        


if __name__ == '__main__':
    main()
    pass
