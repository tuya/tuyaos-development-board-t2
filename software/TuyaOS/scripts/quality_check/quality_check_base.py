#!/usr/bin/python3
# -*- coding: utf-8 -*-

import os
import re
import json


OUTPUT_PATH=os.environ.get('CI_CODECHECK_PACK_DIR')
if not OUTPUT_PATH:
    OUTPUT_PATH="./output/quality_check"

PROJECT_JSON_NAME="code_check_project.json"
PERSON_JSON_NAME="code_check_person.json"
PROJECT_JSON=OUTPUT_PATH + '/' + PROJECT_JSON_NAME
PERSON_JSON=OUTPUT_PATH + '/' + PERSON_JSON_NAME

def _init():
    global _global_dict
    _global_dict = {}
    set_value('output_path', OUTPUT_PATH)
    set_value('project_json_name', PROJECT_JSON_NAME)
    set_value('person_json_name', PERSON_JSON_NAME)
    set_value('project_json', PROJECT_JSON)
    set_value('person_json', PERSON_JSON)

def set_value(key, value):
    _global_dict[key] = value

def get_value(key, defValue=None):
    try:
        return _global_dict[key]
    except KeyError:
        return defValue


def get_user_email(fil, line):
    file_dir = os.path.dirname(fil)
    file_name = os.path.basename(fil)
    #  print(f'cd {file_dir}; git blame {file_name} -L {line},{line} -e')
    ans = os.popen(f'cd {file_dir}; git blame {file_name} -L {line},{line} -e')
    email = re.search(r'(?<=<).*?(?=>)', ans.readline())
    if email:
        return email.group().replace(' ', '')
    return None


def quality_report_append(report, key, value):
    json_data = {}
    if os.path.exists(report):
        fjson = open(report, 'r', encoding='utf8')
        json_data = json.load(fjson)
        fjson.close()

    json_data[key] = value
    json_str = json.dumps(json_data, indent=4, ensure_ascii=False)
    with open(report, 'w') as f:
        f.write(json_str)

