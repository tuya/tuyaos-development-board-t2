##
# @author huatuo

# -*- coding:utf-8 -*-
import sys
import os


def conf2h(conf, header):
    header_f = open(header, 'w', encoding="utf-8")
    conf_lines = []
    for f in conf:
        conf_f = open(f, 'r', encoding="utf-8")
        conf_lines += conf_f.readlines()

    for l in conf_lines:
        l = l.strip()
        ans = ""
        if l.startswith("CONFIG_"):
            ori_key = l.split('=', 1)[0]
            ori_value = l.split('=', 1)[1]

            def_head = "#define "
            def_key = ori_key.replace("CONFIG_", '', 1) + ' '
            def_value = ori_value if ori_value != 'y' else "1"

            ans = def_head + def_key + def_value
        elif l.startswith("#"):
            ans = l.replace('#', "//", 1)
        else:
            ans = l
        header_f.write(ans+'\n')
    header_f.close()
    conf_f.close()


if __name__ == "__main__":
    conf_file = sys.argv[1]
    h_file = sys.argv[2]

    conf_file_sub = conf_file.split()
    conf_file_list = []
    for f in conf_file_sub:
        if os.path.exists(f):
            conf_file_list.append(f)
        else:
            print("can't find file: ", f)

    if not conf_file_list:
        print("No files !", conf_file)
        exit(1)

    conf2h(conf_file_list, h_file)
    pass
