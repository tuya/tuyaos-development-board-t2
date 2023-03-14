#!/usr/bin/python3
# -*- coding: utf-8 -*-

import sys
import os
import json
import quality_check_base as BASE


def calc_quality_scort(proj_json):
    f = open(proj_json, 'r', encoding='utf8')
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
    BASE.quality_report_append(proj_json, 'SUMMARY', quality_data['SUMMARY'])
    print("\033[1;32m%s\033[0m" % "----- calc quality check score ... ----- ")
    print("\033[1;32m%s\033[0m" % "The finally score is %s" % score)
    pass


def main():
    BASE._init()
    output_dir = BASE.get_value("output_path")
    project_json = BASE.get_value("project_json")
    if os.path.exists(project_json):
        calc_quality_scort(project_json)
    else:
        print("not fuond project json: ", project_json)
    
    pass


if __name__ == "__main__":
    main()
