##
# @file xls2json.py
# @brief 输入xls格式的涂鸦能力池，输出json格式的模板
# @author huatuo
# @version 1.0.0
# @date 2021-05-28

#!/usr/bin/python
# -*- coding: utf-8 -*-

import xlrd
import json


##
# @brief xls2json 
# @note 新版(2.0.1)的xlrd竟然只支持xls格式！
def xls2json(xls_file, json_file):
    wb = xlrd.open_workbook(xls_file)
    sheet = wb.sheet_by_index(0)

    group_list = sheet.col_values(0)
    feature_list = sheet.col_values(2)
    range_list = sheet.col_values(3)
    explain_list = sheet.col_values(5)

    group = ""
    json_data = []
    for i in range(1, sheet.nrows):
        group = _get_group(group, group_list[i])
        feature, feature_comm = _get_feature(feature_list[i])
        if None == feature:
            continue
        rang = range_list[i]
        explain = explain_list[i]
        feature_data = {
            "tag": feature,
            "group": group,
            "comment": feature_comm,
            "range": rang,
            "explain": explain
        }
        json_data.append(feature_data)

    json_str = json.dumps(json_data, indent=4, ensure_ascii=False)
    with open(json_file, 'w') as f:
        f.write(json_str)


def _get_group(group, tmp_group):
    if len(tmp_group.strip(' ')):
        return tmp_group.strip(' ')
    else:
        return group


def _get_feature(feature):
    if 0 == len(feature):
        return None, None
    lines = feature.splitlines()
    feature_comm = lines[1] if len(lines) > 1 else ''
    return lines[0], feature_comm.lstrip('(').rstrip(')')


if __name__ == '__main__':
    xls_file = "./power.xls"
    json_file = "./feature_template.json"

    xls2json(xls_file, json_file)
    pass

