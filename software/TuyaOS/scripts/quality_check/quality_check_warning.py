##
# @file quality_check_warning.py
# @brief check the compile warning
# @author huatuo
# @version 1.0.0
# @date 2021-04-12
# @note 依赖`make sdk`的输出日志（log）

#!/usr/bin/python3
# -*- coding: utf-8 -*-

import sys
import os
import quality_check_base as BASE

PROJECT_DIR=os.getcwd()


# check python version > 3.0
if sys.version_info < (3, 0):
    print("Please use python3")
    eixt(0)


def process_person_warning(fil, line, info, data):
    email = BASE.get_user_email(fil, line)
    if not email:
        return
    if email not in data.keys():
        data[email] = {'total': 0, 'list': []}
    data[email]['total'] += 1
    data[email]['list'].append(f'{fil} : {line} : {info}')
    return


def process_project_warning(fil, line, info, data):
    component = fil.split('/')[1]
    if component not in data.keys():
        data[component] = {'total': 0, 'list': []}
    data[component]['total'] += 1
    data[component]['list'].append(f'{fil} : {line} : {info}')
    return


def score_project_warning(data):
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
# @brief quality_check_warning 
#
# @param i_log: (input log) make sdk log
# @param pro_json: project output json
# @param per_json: person output json
# @param w_flag: warning flag
#
# @return 
def quality_check_warning(i_log, per_json, pro_json, w_flag=': warning: '):
    print("quality_check: check warning")
    if not os.path.exists(i_log):
        print("not fuond log: ", i_log)
        return

    project_data = {}
    person_data = {}
    summary = {'total': 0}
    with open(i_log, 'r') as f_log:
        for line in f_log:
            if w_flag not in line:
                continue
            warning_location, warning_info = line.split(w_flag)
            warning_file, warning_line, warning_colu = warning_location.split(':')
            if warning_file.startswith(PROJECT_DIR):
                warning_file = warning_file[len(PROJECT_DIR)+1:]

            process_person_warning(warning_file, warning_line, warning_info, person_data)
            process_project_warning(warning_file, warning_line, warning_info, project_data)
            summary['total'] += 1

    person_data['SUMMARY'] = summary
    BASE.quality_report_append(per_json, 'warning', person_data)
    project_data['SUMMARY'] = summary
    score_project_warning(project_data)
    BASE.quality_report_append(pro_json, 'warning', project_data)
    return


def make_sdk(i_log):
    print("quality_check: make clean config")
    os.system("make clean config > /dev/null 2>&1")
    log_dir = os.path.dirname(i_log)
    if not os.path.exists(log_dir):
        os.makedirs(log_dir)
    print("quality_check: make sdk")
    os.system(f'make > {i_log} 2>&1')


if __name__ == "__main__":
    BASE._init()
    output_dir = BASE.get_value("output_path")
    make_log = os.path.join(output_dir, 'warning', 'makesdk.log')
    person_json = BASE.get_value("person_json")
    project_json = BASE.get_value("project_json")
    make_sdk(make_log)
    quality_check_warning(make_log, person_json, project_json)

