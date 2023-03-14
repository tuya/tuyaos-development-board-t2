#!/usr/bin/python3
# -*- coding:utf-8 -*-

##
# @file build_resources_doc.py
# @brief 用于生成SDK文档
# @author huatuo
# @version 1.0.0
# @date 2022-01-21
# @note 相对路径为SDK根目录

import os
import sys
import shutil


OUTPUT_DIR = "./output/.doc" if len(sys.argv)<2 else sys.argv[1]
RSC_HEAD = '''# 资源统计

|module|file|text|data|bss|total|
|---|---|---|---|---|---|
'''


def _rsc_sum(arr1, arr2):
    for i in range(len(arr1)):
        arr1[i] += int(arr2[i])

def _rsc_write(f, info):
    f.write("|{}|{}|{}|{}|{}|{}|\n" \
        .format(info[0], info[1], info[2], info[3], info[4], info[5]))

def build_resources_doc():
    cmd = os.popen('find ./output -wholename "*/\.objs/static"').readlines()
    if not len(cmd):
        print("has no .o")
        return
    o_dir = cmd[0].strip('\n')
    o_list = os.popen('find {} -name "*\.c\.o"'.format(o_dir)).readlines()
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    rsc = open(OUTPUT_DIR + "/IOTresources.md", 'w')
    rsc.write(RSC_HEAD)

    total = [0, 0, 0, 0]
    total_m = None
    module_last = ""
    for o in o_list:
        o = o.strip('\n')
        module_split = o.replace(o_dir, '').split('/')
        if len(module_split) < 3:
            continue
        module = module_split[1] + '/' + module_split[2]
        module_name = ""
        o_file = o.replace(o_dir, '').split('/')[-1].rstrip(".o")
        size = os.popen("size {}".format(o)).readlines()[1].split()
        if module_last != module:
            module_last = module
            module_name = module
            if total_m != None:
                _rsc_write(rsc, ["", "**total**"]+total_m[:4])
            total_m = [0, 0, 0, 0]
        module_name_md = '`'+module_name+'`' if len(module_name) else ''
        o_file_md = '`' + o_file + '`'
        _rsc_write(rsc, [module_name_md, o_file_md]+size[:4])
        _rsc_sum(total, size[:4])
        _rsc_sum(total_m, size[:4])
    _rsc_write(rsc, ["", "**total**"]+total_m[:4])
    _rsc_write(rsc, ["**total**", ""]+total[:4])
    rsc.close()



if __name__ == "__main__":
    build_resources_doc()

