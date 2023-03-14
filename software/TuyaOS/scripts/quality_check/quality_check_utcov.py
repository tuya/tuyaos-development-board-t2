##
# @file quality_check_utcov.py
# @brief get ut coverage information
# @author huatuo
# @version 1.0.0
# @date 2021-04-23
# @note 依赖`make ut_run COV=1`生成的lcov报告

#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
from bs4 import BeautifulSoup
import quality_check_base as BASE

def _get_score(line_cov, func_cov):
    line_num = float(line_cov.strip('%')) / 100.0
    func_num = float(func_cov.strip('%')) / 100.0
    ans = line_num*40 + func_num*60
    ans = round(ans, 2)
    return ans


def quality_check_utcov(lcov, o_json):
    print("quality_check: utcov")
    if not os.path.exists(lcov):
        print("not fuond lcov report: ", lcov)
        return

    soup = BeautifulSoup(open(lcov,'r',encoding = 'utf-8'), "html.parser")

    #  body > table:nth-child(1) > tbody > tr:nth-child(3) > td > table > tbody > tr:nth-child(2) > td.headerCovTableEntryLo
    summary_list =  soup.select('body > table > tr:nth-child(3) > td > table > tr')
    total_lint_cov = summary_list[1].select('td:nth-child(7)')[0].get_text().replace('\xa0', '')
    total_func_cov = summary_list[2].select('td:nth-child(7)')[0].get_text().replace('\xa0', '')
    summary = {
        'total_line_cov': total_lint_cov,
        'total_func_cov': total_func_cov,
        'score': 100
    }

    total_score = 0
    total_comp = 0
    utcov_data = {}
    tr_list = soup.select('body > center > table > tr')  # 固定结构
    for tr in tr_list:
        coverFile = tr.select('td.coverFile')
        if not coverFile:  # 过滤表头
            continue
        file_path = coverFile[0].get_text()
        if "/components/" not in file_path:
            continue
        component, item = file_path.split("/components/", maxsplit=1)[1].split('/', maxsplit=1)
        if not component:  # 非严格模式
            continue
        td_list = tr.select('td')
        line_cov = td_list[3].get_text().replace('\xa0', '')
        func_cov = td_list[5].get_text().replace('\xa0', '')
        if component not in utcov_data.keys():
            utcov_data[component] = {'list': [], 'score': 100}
        utcov_data[component]['list'].append(f'{item} : {line_cov} {func_cov}')
        if item.startswith("src"):
            score = _get_score(line_cov, func_cov)
            utcov_data[component]['score'] = score
            total_score += score
            total_comp += 1


    summary['score'] = round(total_score / total_comp, 2)
    utcov_data['SUMMARY'] = summary
    BASE.quality_report_append(o_json, 'utcov', utcov_data)
    return


if __name__ == '__main__':
    BASE._init()
    output_dir = BASE.get_value('output_path')
    lcov_file = os.path.join(output_dir, 'ut', 'lcov', 'index.html')
    quality_check_utcov(lcov_file, BASE.get_value('project_json'))

