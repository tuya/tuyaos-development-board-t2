#!/usr/bin/python3
# -*- coding: utf-8 -*-


import sys
import os
import json
import quality_check_base as BASE
import quality_check_warning as WARN
import quality_check_api as API
import quality_check_utcov as UTCOV
import quality_check_utapi as UTAPI


OUTPUT_PATH       = "" if len(sys.argv)<2 else os.path.join(sys.argv[1], 'quality_check')
PROJECT_JSON_NAME = "quality_check_project.json"
PERSON_JSON_NAME  = "quality_check_person.json"
PROJECT_JSON      = os.path.join(OUTPUT_PATH, PROJECT_JSON_NAME)
PERSON_JSON       = os.path.join(OUTPUT_PATH, PERSON_JSON_NAME)


def check_api():
    output_dir = BASE.get_value('output_path')
    api_dir = os.path.join(output_dir, '..', '.api')
    API.make_doc(api_dir)
    API.quality_check_api(api_dir, PROJECT_JSON)

def check_utcov():
    output_dir = BASE.get_value('output_path')
    lcov_file = os.path.join(output_dir, '..', 'ut', 'lcov', 'index.html')
    UTCOV.make_ut(lcov_file)
    UTCOV.quality_check_utcov(lcov_file, BASE.get_value('project_json'))

def check_utapi():
    output_dir = BASE.get_value('output_path')
    lcov_dir = os.path.join(output_dir, '..', 'ut', 'lcov')
    api_json = os.path.join(output_dir, 'api.json')
    utapi_json = os.path.join(output_dir, 'utapi.json')
    project_json = BASE.get_value('project_json')
    person_json = BASE.get_value('person_json')

    UTAPI.make_ut(lcov_dir)
    UTAPI.quality_check_utapi(lcov_dir, api_json, utapi_json, project_json, person_json)


def check_warning():
    output_dir = BASE.get_value('output_path')
    make_log = os.path.join(output_dir, 'make.log')
    person_json = os.path.join(output_dir, 'quality_check_person.json')
    project_json = os.path.join(output_dir, 'quality_check_project.json')

    WARN.make_sdk(make_log)
    WARN.quality_check_warning(make_log, person_json, project_json)


def calc_quality_scort():
    f = open(PROJECT_JSON, 'r', encoding='utf8')
    quality_data = json.load(f)
    f.close()

    if 'SUMMARY' not in quality_data.keys():
        quality_data['SUMMARY'] = {}

    total_score = 0
    total_comp = 0
    for key in quality_data.keys():
        if key == 'SUMMARY':
            continue
        total_score += quality_data[key]['SUMMARY']['score']
        total_comp += 1
    score = round(total_score / total_comp, 2)
    quality_data['SUMMARY']['score'] = score
    print("\033[1;32m%s\033[0m" % "The finally score is %s" % score)
    BASE.quality_report_append(PROJECT_JSON, 'SUMMARY', quality_data['SUMMARY'])


def main():
    BASE._init()
    if len(OUTPUT_PATH):
        BASE.set_value('output_path', OUTPUT_PATH)
        BASE.set_value('project_json_name', PROJECT_JSON_NAME)
        BASE.set_value('person_json_name', PERSON_JSON_NAME)
        BASE.set_value('project_json', PROJECT_JSON)
        BASE.set_value('person_json', PERSON_JSON)

    if not os.path.exists(OUTPUT_PATH):
        os.makedirs(OUTPUT_PATH)

    check_warning()
    check_api()
    check_utcov()
    check_utapi()
    calc_quality_scort()


if __name__ == "__main__":
    main()
