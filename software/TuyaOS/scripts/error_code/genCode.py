#!/usr/bin/env python3
# -*- coding: utf-8 -*-

##
# @file genCode.py
# @brief 根据json文件生成错误码头文件以及文档
#        头文件：./release/tuya_error_code.y
#        文档  ：./release/tuya_error_code.md
# @author huatuo
# @version 1.0.0
# @date 2021-09-27
# @note 支持参数1修改文件名，如："base"，使用默认命名则传递空：""
# @note 支持参数2传入指定模块列表，参数形式支持如下：
#       "compA compB compC"
#       "compA, compB, compC"



import json
import os
import sys
import shutil
import codecs
from datetime import date

# 为了兼容python2和python3
if sys.version_info < (3, 0):
    reload(sys)
    sys.setdefaultencoding('utf8')


file_head_template = """/*******************************************************************
*  File: tuya_###MODULE###_error_code.h
*  Author: auto generate by tuya code gen system
*  Date: ###DATE###
*  Description:this file defined the error code of tuya IOT 
*  you can change it manully if needed
*  Copyright(C),2018-2020, tuya inc, www.tuya.comm
*******************************************************************/

#ifndef TUYA_###MODULE_UPPER###_ERROR_CODE_H
#define TUYA_###MODULE_UPPER###_ERROR_CODE_H

#ifdef __cplusplus
extern "C" {
#endif


"""

file_foot_template = """
#define ERRCODE2STRING(errcode) #errcode
#define TUYA_ERROR_STRING(errcode)  ("[ErrCode: " ERRCODE2STRING(errcode) "]")

#define TUYA_CHECK_NULL_RETURN(x, y)\\
do{\\
    if (NULL == (x)){\\
        TAL_PR_ERR("%s null", #x);\\
        return (y);\\
    }\\
}while(0)


#define TUYA_CHECK_NULL_GOTO(x, label)\\
do{\\
    if (NULL == (x)){\\
        TAL_PR_ERR("%s null", #x);\\
        goto label;\\
    }\\
}while(0)


#define TUYA_CALL_ERR_LOG(func)\\
do{\\
    rt = (func);\\
    if (OPRT_OK != (rt)){\\
        TAL_PR_ERR("ret:%d", rt);\\
    }\\
}while(0)


#define TUYA_CALL_ERR_GOTO(func, label)\\
do{\\
    rt = (func);\\
    if (OPRT_OK != (rt)){\\
        TAL_PR_ERR("ret:%d", rt);\\
        goto label;\\
    }\\
}while(0)


#define TUYA_CALL_ERR_RETURN(func)\\
do{\\
    rt = (func);\\
    if (OPRT_OK != (rt)){\\
       TAL_PR_ERR("ret:%d", rt);\\
       return (rt);\\
    }\\
}while(0)


#define TUYA_CALL_ERR_RETURN_VAL(func, y)\\
do{\\
    rt = (func);\\
    if (OPRT_OK != (rt)){\\
        TAL_PR_ERR("ret:%d", rt);\\
        return (y);\\
    }\\
}while(0)


#define TUYA_CALL_ERR_LOG_SEQ_RETURN_VAL(func, y, point)\\
do{\\
    rt = (func);\\
    if (OPRT_OK != (rt)){\\
        TAL_PR_ERR("ret:%d", rt);\\
        INSERT_ERROR_LOG_SEQ_DEC((point), rt);\\
        return (y);\\
    }\\
}while(0)


#define TUYA_CALL_ERR_LOG_SEQ_RETURN(func, point)\\
do{\\
    rt = (func);\\
    if (OPRT_OK != (rt)){\\
        TAL_PR_ERR("ret:%d", rt);\\
        INSERT_ERROR_LOG_SEQ_DEC((point), rt);\\
        return (rt);\\
    }\\
}while(0)


#define TUYA_CALL_ERR_LOG_SEQ_GOTO(func, label)\\
do{\\
    rt = (func);\\
    if (OPRT_OK != (rt)){\\
        TAL_PR_ERR("ret:%d", rt);\\
        INSERT_ERROR_LOG_SEQ_DEC((point), rt);\\
        goto label;\\
    }\\
}while(0)


#define TUYA_CALL_ERR_LOG_SEQ(func)\\
do{\\
    rt = (func);\\
    if (OPRT_OK != (rt)) {\\
        TAL_PR_ERR("ret:%d", rt);\\
        INSERT_ERROR_LOG_SEQ_DEC((point), rt);\\
    }\\
}while(0)


#define TUYA_CHECK_NULL_LOG_SEQ_RETURN(x, y, point)\\
do{\\
    if (NULL == (x)){\\
        TAL_PR_ERR("%s null", #x);\\
        INSERT_ERROR_LOG_SEQ_DEC((point), y);\\
        return (y);\\
    }\\
}while(0)


#define TUYA_CHECK_NULL_LOG_SEQ_GOTO(x, point, label)\\
do{\\
    if (NULL == (x)){\\
        TAL_PR_ERR("%s null", #x);\\
        INSERT_ERROR_LOG_SEQ_NULL((point));\\
        goto label;\\
    }\\
}while(0)


#ifdef __cplusplus
}
#endif
#endif
"""

marco_head_template = """
/****************************************************************************
            the error code marco define for module ###MODULE###
****************************************************************************/
"""


class codegen():
    def __init__(self, file_name="", enable_list=[]):
        self.output_path = "./release"
        self.modules = None  # json
        self.file_name = "_" + file_name if len(file_name) else ""
        self.enable_list = enable_list
        print("file_name: ", file_name)
        print("enable_list: ", enable_list)

    def load_modules(self):
        module_file = codecs.open("./module.json", "r", "utf-8")
        self.modules = json.load(module_file, encoding='utf-8')['TuyaEmbeddedErrcode']

    # 判断该组件错误码是否要被处理
    def _modules_is_ok(self, module):  
        if (len(module['errcode']) == 0):
            #  print("module length is 0: ", module['name'])
            return False
        if (module['offset']>255) or (len(module['errcode'])>255):
            print("module over offset: ", module['name'])
            return False
        if self.enable_list == []:
            # 使能列表为空中则生成全部错误码
            return True
        if module['name'] not in self.enable_list:
            # 模块没有被使能
            return False
        return True

    def _gen_tbl_header(self, module_name, index, errcode_cnt):
        tbl_header = "\n## " + str(index) + ". module " + '`' + module_name + '`' + "\n\n"
        if errcode_cnt > 0:
            tbl_header = tbl_header + "   No. | Name  | Value | message" + "\n"
            tbl_header = tbl_header + "-------|-------|-------|--------" + "\n"
        return tbl_header

    def _gen_marco_item(self, module_name, errcode, offset, index):
        marco_item = ""
        marco_msg = ""
        marco_val = ""

        # prefix, global error code not have module_name
        marco_item_prefix = "OPRT"+ "_" + module_name.upper() + "_"
        marco_item_prefix_global = "OPRT" + "_"   

        # only one key in error code
        for key in errcode:
            if module_name  == "global":
                marco_item = marco_item_prefix_global + key
            else:
                marco_item = marco_item_prefix + key
            error_val = "-%#06x" % ((offset<<8) + index)
            marco_val = "(%s)" % error_val
            marco_msg = str(int(error_val, 16)) + ", " + errcode[key]

        return marco_item, marco_val, marco_msg

    def gen_md(self):
        file_content = "# Tuya Embedded software error code define" + "\n"

        module_index = 0
        for module in self.modules:
            if not self._modules_is_ok(module):
                continue
            module_index += 1
            file_content = file_content + self._gen_tbl_header(module["name"].upper(), module_index, len(module['errcode']))
            index = 0
            for errcode in module["errcode"]:
                marco_item, marco_val, marco_msg = self._gen_marco_item(module["name"], errcode, module["offset"], index)
                marco_item, marco_val, marco_msg = '`'+marco_item+'`', '`'+marco_val+'`', '`'+marco_msg+'`'
                index += 1
                file_content = file_content + str(index) + "|" + marco_item + "|" + str(marco_val) + "|" + marco_msg
                file_content = file_content + "\n"

        # 生成.md文件
        file_name = self.output_path + "/tuya_###MODULE###_error_code.md".replace("_###MODULE###", self.file_name)
        fd = open(file_name, 'w')
        fd.write(file_content)
        fd.close()
        # print("OK!")

        return    

    def gen_marco(self, module):
        define = "#define"
        marco_content_head = marco_head_template.replace("###MODULE###", module["name"].upper())
        marco_content_body = ""

        marco_item_prefix = "OPRT"+ "_" + module["name"].upper() + "_"
        index = 0

        for errcode in module["errcode"]:
            marco_item, marco_val, marco_msg = self._gen_marco_item(module["name"], errcode, module["offset"], index)
            index += 1

            # appened to the marco content
            marco_define = "%(d)s %(i)-50s %(v)+8s  //%(m)s" % \
                            {'d':define, 'i':marco_item, 'v':marco_val, 'm':marco_msg}
            marco_content_body = marco_content_body + marco_define + '\n'

        max_marco_cnt = define + " " + marco_item_prefix + "ERRCODE_MAX_CNT" + " " + str(index)

        marco_content_body = marco_content_body + max_marco_cnt + '\n\n'
        marco_content = marco_content_head + marco_content_body

        return marco_content

    def gen_file(self):
        file_name = self.output_path +"/tuya_###MODULE###_error_code.h".replace("_###MODULE###", self.file_name)
        fd = open(file_name, 'w')

        # head
        file_head = file_head_template.replace("_###MODULE###", self.file_name)
        file_head = file_head.replace("_###MODULE_UPPER###", self.file_name.upper())
        file_head = file_head.replace("###DATE###", str(date.today()))
        fd.write(file_head)

        # marco        
        for module in self.modules:            
            if not self._modules_is_ok(module):
                continue
            marco_str = self.gen_marco(module)
            fd.write(marco_str)

        fd.write(file_foot_template)
        fd.close()
        # print("OK!")

        return 

    def dogen(self):
        # load all module description
        self.load_modules()
        
        # clean output path
        shutil.rmtree(self.output_path, ignore_errors=True)
        os.mkdir(self.output_path)

        # gen .h file
        # print("generate errcode .h file...")
        rt = self.gen_file()

        # gen .md file
        # print("generate errcode .md file...")
        self.gen_md()

        return 


if __name__ == '__main__':
    file_name = ""
    if len(sys.argv) > 1:
        file_name = sys.argv[1]
    enable_list = []
    if len(sys.argv) > 2:
        enable_list_str = sys.argv[2]
        enable_list = enable_list_str.replace(',', ' ').split()
    gen = codegen(file_name, enable_list)
    gen.dogen()


