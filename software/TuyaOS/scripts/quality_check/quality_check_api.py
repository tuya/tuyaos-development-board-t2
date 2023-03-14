##
# @file quality_check_api.py
# @brief collect API information and amount
# @author huatuo
# @version 1.0.0
# @date 2021-04-26
# @note 依赖`make doc`生成的api报告（json）

#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import shutil
import json
import quality_check_base as BASE


def _get_score(api_total):
    if api_total <= 20: return 100
    elif api_total <= 30: return 90
    elif api_total <= 40: return 80
    elif api_total <= 50: return 60
    elif api_total <= 60: return 50
    else: return 0

def get_api_score(api_data):
    total_score = 0
    total_comp = 0
    for key in api_data.keys():
        if key == "SUMMARY":
            continue
        score = _get_score(api_data[key]['total'])
        api_data[key]['score'] = score
        total_score += score
        total_comp += 1
    api_data['SUMMARY']['score'] = round(total_score / total_comp, 2)
    pass


def get_func(api_json, api_dir):
    f = open(api_json, 'r', encoding='utf8')
    api_data = json.load(f)
    func_list = api_data['FuncDef']
    if not isinstance(func_list, list):
        return None
    api_dir_len = len(api_dir) + 1
    ans = {'total': 0, 'funcList': []}
    for func in func_list:
        ans['total'] += 1
        func_info = api_json[api_dir_len:-5] + ' : ' + '1' + ' : ' + func['IdentName']
        #  print(func_info)
        ans['funcList'].append(func_info)
    f.close()
    return ans


##
# @brief quality_check_api 
#
# @param api_dir get api json from `embcli doc`
# @param o_json
#
# @return 
def quality_check_api(api_dir, o_json):
    print("quality check: api")
    if not os.path.exists(api_dir):
        print("not fuond report: ", api_dir)
        return
    api_dir_len = len(api_dir)

    component_list = os.listdir(api_dir)
    api_data = {}
    summary = {'total': 0}
    for root, dirs, files in os.walk(api_dir):  
        for fil in files:
            if fil[-7:] != '.h.json':
                continue
            api_info = get_func(os.path.join(root, fil), api_dir)
            if not api_info:
                continue
            api_root = root[api_dir_len:]
            comp = api_root.split('/')[1]
            if comp not in api_data.keys():
                api_data[comp] = {'total': 0, 'list': []}
            summary['total'] += api_info['total']
            api_data[comp]['total'] += api_info['total']
            api_data[comp]['list'].extend(api_info['funcList'])
    api_data['SUMMARY'] = summary
    get_api_score(api_data)

    BASE.quality_report_append(o_json, 'api', api_data)
    api_json = os.path.join(BASE.get_value('output_path'), 'api', 'api.json')
    BASE.quality_report_append(api_json, 'api', api_data)
    pass


if __name__ == '__main__':
    BASE._init()
    output_dir = BASE.get_value('output_path')
    api_dir = os.path.join(output_dir, 'api', 'tuya_iot_doc', 'doc', 'components')
    project_json = BASE.get_value("project_json")
    quality_check_api(api_dir, project_json)
    pass

