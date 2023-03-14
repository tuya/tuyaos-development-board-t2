#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import json

root_path = os.path.split(os.path.realpath(__file__))[0]
os.chdir(root_path)

def build_catelog(i_dir, o_json):
    if not os.path.exists(i_dir):
        print("error path: ", i_dir)
        return None

    ans = {}
    root, dirs, files = next(os.walk(i_dir))

    for i in dirs:
        ans[i] = build_catelog(os.path.join(root, i), None)

    fil = [ f for f in files if f[-5:]=='.html']
    ans['files'] = fil

    if o_json != None:
        json_str = json.dumps(ans, indent=4, ensure_ascii=False)
        with open(o_json, 'w') as f:
            f.write(json_str)

    return ans


if __name__ == '__main__':
    build_catelog("./doc", "./catelog.json")
    pass
