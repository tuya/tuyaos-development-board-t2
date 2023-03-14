##
# @file quality_check_utapi.py
# @brief check api with ut
# @author huatuo
# @version 1.0.0
# @date 2021-05-02
# @note 依赖`quality_check_api.py`生成的api记录信息，以及`make ut_run LOG=1 COV=1`生成的lcov报告
#       首先生成UT覆盖API的信息（ut_api.json）然后和项目API（api.json）比较输出覆盖率结果。

#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import json
from bs4 import BeautifulSoup
import quality_check_base as BASE

def _get_ut_func_json(html_path):
    """
    解析有API执行次数信息的html文件
    返回模块名、文件名、API执行次数这些信息
    返回执行API列表
    """
    soup = BeautifulSoup(open(html_path,'r',encoding = 'utf-8'), "html.parser")

    #  body > table:nth-child(1) > tbody > tr:nth-child(3) > td > table > tbody > tr:nth-child(1) > td.headerValue > a:nth-child(2)
    top_level = soup \
                .select("body > table:nth-child(1) > tr:nth-child(3) > td > table > tr:nth-child(1) > td.headerValue > a:nth-child(2)")[0] \
                .get_text()
    module_name = top_level.split('components/')[1].split('/')[0]
    c_file = html_path.split('/')[-1][:-17]

    json_data = {}
    json_data['module'] = module_name
    json_data['file'] = c_file
    json_data['cov'] = []

    pass_list = []

    #  body > center > table > tbody > tr:nth-child(3) > td.coverFn > a
    func_cov_list = soup \
                    .select("body > center > table > tr")
    for f in func_cov_list[2:]:
        func = f.select("td.coverFn > a")[0].get_text()
        num = f.select("td")[1].get_text()
        info = "%s : %s" % (func, num)
        json_data['cov'].append(info)
        if num != '0':
            pass_list.append(func)

    return json_data, pass_list

def build_ut_func(lcov_dir, out_json):
    """
    遍历lcov目录下所有文件，找到有API执行次数的html文件，
    进行解析，
    将单元测试API的情况写入out_json中，
    返回已经覆盖过得api列表
    """
    json_data = {}
    summary = {'total': 0}
    pass_list = []

    for root, dirs, files in os.walk(lcov_dir):
        for fil in files:
            if not fil.endswith(".c.func-sort-c.html"):
                continue
            html_path = os.path.join(root, fil)
            cov_data, pass_subs = _get_ut_func_json(html_path)

            module_name = cov_data['module']
            if module_name not in json_data.keys():
                json_data[module_name] = {'total':0}
            json_data[module_name][cov_data['file']] = cov_data['cov']
            json_data[module_name]['total'] += len(cov_data['cov'])
            summary['total'] += len(cov_data['cov'])
            pass_list.extend(pass_subs)

    json_data['SUMMARY'] = summary
    BASE.quality_report_append(out_json, 'utapi', json_data)
    return pass_list


def process_person_utapi(fil, line, info, data):
    email = BASE.get_user_email(fil, line)
    if not email:
        return
    if email not in data.keys():
        data[email] = {'total': 0, 'list': []}
    data[email]['total'] += 1
    data[email]['list'].append(info)
    return


def score_project_utapi(data):
    total_score = 0
    total_comp = 0
    for k in data.keys():
        if k == 'SUMMARY': continue
        if data[k]['total'] > 20:
            score = 0
        else:
            score = 100 - (5 * data[k]['total'])
        data[k]['score'] = score
        total_score += score
        total_comp += 1
    data['SUMMARY']['total'] = total_comp
    data['SUMMARY']['score'] = round(total_score / total_comp, 2)


def build_zero_api(api_list, pass_list, project_json, person_json):
    """
    比对API和UT未覆盖API，
    查询git提交信息，构建得到负责人信息，
    补充至json文件中。
    """
    person_data = {}
    project_data = api_list.copy()
    summary = {'total': 0}
    components = api_list.keys()
    for c in components:
        if 'list' not in api_list[c].keys():
            continue
        apis = api_list[c]['list']
        for a in apis:
            api_path = a.split(':')[0].strip()
            line_num = a.split(':')[1].strip()
            func_name = a.split(':')[2].strip()
            if func_name not in pass_list:
                file_path = os.path.join("components", api_path)
                process_person_utapi(file_path, line_num, a, person_data)
                summary['total'] += 1
            else:
                project_data[c]['list'].remove(a)
                project_data[c]['total'] -= 1
    person_data['SUMMARY'] = summary
    BASE.quality_report_append(person_json, 'utapi', person_data)
    score_project_utapi(project_data)
    BASE.quality_report_append(project_json, 'utapi', project_data)


##
# @brief quality_check_utapi 
#
# @param lcov_dir 已经生成的lcov目录
# @param api_json 已经生成的api记录文件
# @param utapi_json 将lcov的解析结果输出到这个json文件
# @param project_json 项目报告
# @param person_json 私人报告
#
# @return 
def quality_check_utapi(lcov_dir, api_json, utapi_json, project_json, person_json):
    print("quality_check: utapi")
    f = open(api_json, 'r', encoding='utf8')
    api_data = json.load(f)
    f.close()

    pass_list = build_ut_func(lcov_dir, utapi_json)
    build_zero_api(api_data['api'], pass_list, project_json, person_json)


if __name__ == '__main__':
    BASE._init()
    output_dir = BASE.get_value('output_path')
    lcov_dir = os.path.join(output_dir, 'ut', 'lcov')
    api_json = os.path.join(output_dir, 'api', 'api.json')
    utapi_json = os.path.join(output_dir, 'ut', 'utapi.json')
    person_json = BASE.get_value("person_json")
    project_json = BASE.get_value("project_json")
    quality_check_utapi(lcov_dir, api_json, utapi_json, project_json, person_json)

