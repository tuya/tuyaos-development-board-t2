#!/usr/bin/python
# -*- coding: utf-8 -*-
##
# @file openDoc.py
# @brief 用于浏览sdk生成的doc文件
# @author huatuo
# @version 1.0.0
# @date 2021-06-25


# 检查版本
import sys
if sys.version_info.major < 3:
    print("Now python version is:\n")
    print(sys.version)
    print("\nPlease use python3")
    exit()

# 修改路径
import os
root_path = os.path.split(os.path.realpath(__file__))[0]
os.chdir(root_path)

import socket
import webbrowser
from http.server import HTTPServer, SimpleHTTPRequestHandler


def portUsed(port, host="127.0.0.1"):
    """ 判断端口是否已经使用 """
    s = None
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(1)
        s.connect((host, int(port)))
        return True
    except socket.error:
        return False
    finally:
        if s:
            s.close()


def getFreePort(minPort, maxPort, host="127.0.0.1"):
    """ 获取空闲端口 """
    for p in range(minPort, maxPort):
        if not portUsed(p, host):
            return p
    return None


def openServer(port, host="127.0.0.1"):
    """ 开启本地服务 """
    if portUsed(port, host):
        print("prot: %d is used" % (port))
        return None

    server = HTTPServer((host, port), SimpleHTTPRequestHandler)
    server.serve_forever()
 

if __name__ == '__main__':
    minPort = 8100
    maxPort = 8200
    port = getFreePort(minPort, maxPort)
    if not port:
        print("All prots are used: [%d to %d]" % (minPort, maxPort))
        exit()

    docUrl = "http://127.0.0.1:%d/" % (port)
    print("open: ", docUrl)
    webbrowser.open(docUrl, new=0, autoraise=True) 
    openServer(port)
 
