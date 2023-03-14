##
# @file quality_check_cppcheck.py
# @brief use cppcheck check code
# @author huatuo
# @version 1.0.0
# @date 2021-11-02
# @note 依赖`make info`输出的编译信息内容

#!/usr/bin/python3
# -*- coding: utf-8 -*-

import sys
import os
import json
import re
import quality_check_base as BASE

PROJECT_DIR=os.getcwd()


# check python version > 3.0
if sys.version_info < (3, 0):
    print("Please use python3")
    eixt(0)


def process_person_cppcheck(fil, line, info, data):
    email = BASE.get_user_email(fil, line)
    if not email:
        return
    if email not in data.keys():
        data[email] = {'total': 0, 'list': []}
    data[email]['total'] += 1
    data[email]['list'].append(f'{fil} : {line} : {info}')
    return


def process_project_cppcheck(fil, line, info, data):
    component = fil.split('/')[1]
    if component not in data.keys():
        data[component] = {'total': 0, 'list': []}
    data[component]['total'] += 1
    data[component]['list'].append(f'{fil} : {line} : {info}')
    return


def score_project_cppcheck(data):
    total_score = 0
    total_comp = 0
    for k in data.keys():
        if k == 'SUMMARY': continue
        if data[k]['total'] > 10:
            score = 0
        else:
            score = 100 - (10 * data[k]['total'])
        data[k]['score'] = score
        total_score += score
        total_comp += 1
    data['SUMMARY']['comp_total'] = total_comp
    data['SUMMARY']['score'] = round(total_score / total_comp, 2)


##
# @brief quality_check_cppcheck
#
# @param cppcheck_log: cppcheck检查日志
# @param pro_json: project output json
# @param per_json: person output json
#
# @return 
def quality_check_cppcheck(cppcheck_log, per_json, pro_json):
    print("quality_check: check cppcheck")

    project_data = {}
    person_data = {}
    summary = {'total': 0}
    with open(cppcheck_log, 'r') as f_log:
        for line in f_log:
            pattern = r'^.*:\d*:[\w{}]*: \w*:.*$'  # 兼容不同cppcheck的输出格式
            re_rst = re.findall(pattern, line)
            if not re_rst:
                continue
            check_msg = re_rst[0]
            fil, line, colu, info = check_msg.split(':', 3)

            process_person_cppcheck(fil, line, info, person_data)
            process_project_cppcheck(fil, line, info, project_data)
            summary['total'] += 1

    person_data['SUMMARY'] = summary
    BASE.quality_report_append(per_json, 'cppcheck', person_data)
    project_data['SUMMARY'] = summary
    score_project_cppcheck(project_data)
    BASE.quality_report_append(pro_json, 'cppcheck', project_data)
    return


def do_cppcheck(make_info, output_dir, log_name):
    if not os.path.exists(make_info):
        print("not fuond log: ", make_info)
        exit(1)

    f = open(make_info, 'r', encoding='utf8')
    info_data = json.load(f)
    f.close()

    # include
    cflags = info_data['CFLAGS']
    cflags_split = cflags.split()
    include_dir_str = ""
    for c in cflags_split:
        if c.startswith("-I"):
            include_dir_str += c[2:]+'\n'

    # include file
    include_file = os.path.join(output_dir, "include_file.txt")
    with open(include_file, 'w') as f:
        f.write(include_dir_str)

    # src
    components = info_data['components']
    src_files_str = ""
    for c in components:
        src_list = c['LOCAL_SRC_FILES'].split()
        for s in src_list:
            src_files_str += s+'\n'

    # src file
    src_file = os.path.join(output_dir, "src_file.txt")
    with open(src_file, 'w') as f:
        f.write(src_files_str)

    # cppcheck
    log_path = os.path.join(output_dir, "cppcheck.log")
    template = "--template='{file}:{line}:{column}: {severity}:{message} [{id}]\\n{code}'"
    cmd = "--force --quiet --suppress=unusedFunction --suppress=missingInclude"
    command = "cppcheck --enable=all %s %s --includes-file=%s --file-list=%s > %s 2>&1" % \
        (cmd, template, include_file, src_file, log_path)
    print(command)
    os.system(command)
    pass


if __name__ == "__main__":
    BASE._init()
    output_dir = BASE.get_value("output_path")
    cppcheck_output_dir = os.path.join(output_dir, "cppcheck")
    make_info = os.path.join(cppcheck_output_dir, 'compile_information.json')
    person_json = BASE.get_value("person_json")
    project_json = BASE.get_value("project_json")

    do_cppcheck(make_info, cppcheck_output_dir, "cppcheck.log")
    cppcheck_log = os.path.join(cppcheck_output_dir, "cppcheck.log")
    quality_check_cppcheck(cppcheck_log, person_json, project_json)

