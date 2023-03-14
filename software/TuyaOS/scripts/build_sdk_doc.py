#!/usr/bin/python3
# -*- coding:utf-8 -*-

##
# @file build_sdk_doc.py
# @brief 用于生成SDK文档
# @author huatuo
# @version 1.0.0
# @date 2020-12-30
# @note 相对路径为SDK根目录

import os
import sys
import shutil


OUTPUT_DIR = "./output/.doc" if len(sys.argv)<2 else sys.argv[1]
DOC_OUTPUT_DIR = "./output/tuya_iot_doc" if len(sys.argv)<3 else sys.argv[2]
HEADER_DIR = "include" if len(sys.argv)<4 else sys.argv[3]
RSC_HEAD = '''# IoT SDK 资源统计
---

|module|file|text|data|bss|total|
|---|---|---|---|---|---|
'''

def doc_clean():
    shutil.rmtree(OUTPUT_DIR, ignore_errors=True)
    shutil.rmtree(DOC_OUTPUT_DIR, ignore_errors=True)


def copy_header_files():
    shutil.copytree(HEADER_DIR, OUTPUT_DIR)


def copy_sdk_doc():
    md_dir = "README.md"
    output_dir = OUTPUT_DIR
    if os.path.exists(md_dir):
        shutil.copy(md_dir, output_dir)


def copy_component_doc():
    _, group_list, _ = next(os.walk(OUTPUT_DIR))
    for g in group_list:
        _, comp_list, _ = next(os.walk(os.path.join(OUTPUT_DIR, g)))
        for c in comp_list:
            readme = os.path.join(g, c, "README.md");
            if os.path.exists(readme):
                shutil.copy(readme, os.path.join(OUTPUT_DIR, g, c))


def copy_errcode_doc():
    errcode_dir = "./build/error_code/tuya_error_code.md"
    output_dir = OUTPUT_DIR + '/error_code'
    if os.path.exists(errcode_dir):
        os.makedirs(output_dir, exist_ok=True)
        shutil.copy(errcode_dir, output_dir)


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
    rsc_dir = OUTPUT_DIR + "/资源信息"
    os.makedirs(rsc_dir, exist_ok=True)
    rsc = open(rsc_dir + "/IOTresources.md", 'w')
    rsc.write(RSC_HEAD)

    total = [0, 0, 0, 0]
    total_m = None
    module_last = ""
    for o in o_list:
        o = o.strip('\n')
        module_split = o.replace(o_dir, '').split('/')
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


def embed_action():
    #  os.system("embed doc --json --in {} --out {}".format(OUTPUT_DIR, DOC_OUTPUT_DIR))
    os.system("embcli doc --json --in {} --out {}".format(OUTPUT_DIR, DOC_OUTPUT_DIR))


def rebuild_doc():
    """
    新的doc目录结构，前端显示有调整，
    在一个页面中浏览不同的API文件（html）
    """
    SRC_TEMPLATE_DIR = "./scripts/doc_template"
    DST_TEMPLATE_DIR = "./output/doc_template"
    if os.path.exists(os.path.join(DOC_OUTPUT_DIR, 'All.html')):
        os.remove(os.path.join(DOC_OUTPUT_DIR, 'All.html'))
    if os.path.exists(os.path.join(DOC_OUTPUT_DIR, 'index.html')):
        os.remove(os.path.join(DOC_OUTPUT_DIR, 'index.html'))
    shutil.rmtree(DST_TEMPLATE_DIR, ignore_errors=True)
    shutil.copytree(SRC_TEMPLATE_DIR, DST_TEMPLATE_DIR)
    shutil.move(DOC_OUTPUT_DIR, os.path.join(DST_TEMPLATE_DIR, 'doc'))
    shutil.move(DST_TEMPLATE_DIR, DOC_OUTPUT_DIR)
    os.system("python3 %s" % (os.path.join(DOC_OUTPUT_DIR, 'build_catelog.py')))
    print("Use Command:")
    print("\033[1;32m%s\033[0m" % "python3 %s/openDoc.py" % DOC_OUTPUT_DIR)


def main():
    doc_clean()
    copy_header_files()
    copy_sdk_doc()
    copy_component_doc()
    copy_errcode_doc()
    build_resources_doc()
    embed_action()
    rebuild_doc()


if __name__ == "__main__":
   main()

