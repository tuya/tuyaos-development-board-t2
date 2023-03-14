#!/bin/sh
#
# 导出开发环境（工具链）
#   有些开发环境工具链是一个tar包，编译过程中会解包，此脚本导出的工具链不包含解包后文件
#   如果开发环境中有export.sh脚本，则优先使用此脚本导出
#
# 测试命令：
# sh ./build/export_platform.sh platforms/rsdk-1.5.5-4181-EB-2.6.30-0.9.30.3-110225 output/test_platform


EXPORT_SRC_DIR=$1
EXPORT_DST_DIR=$2


[ -z $EXPORT_SRC_DIR ] && echo "no src dir" && exit 99
[ -z $EXPORT_DST_DIR ] && echo "no dst dir" && exit 99


export_all()
{
    cp -a ${EXPORT_SRC_DIR}/* ${EXPORT_DST_DIR}/
}

export_exclude_toolchain()
{
    for dir in $(ls -1 ${EXPORT_SRC_DIR}); do
        if [ x"$dir" != x"toolchain" ]; then
            cp -av ${EXPORT_SRC_DIR}/$dir ${EXPORT_DST_DIR}/
        fi
    done
    mkdir -p ${EXPORT_DST_DIR}/toolchain
    for dir in $(ls -1 ${EXPORT_SRC_DIR}/toolchain); do
        if [ x"$dir" != x"$1" ]; then
            cp -av ${EXPORT_SRC_DIR}/toolchain/$dir ${EXPORT_DST_DIR}/toolchain
        fi
    done
}


mkdir -p $EXPORT_DST_DIR

if [ -f $EXPORT_SRC_DIR/export.sh ]; then
    sh $EXPORT_SRC_DIR/export.sh $EXPORT_DST_DIR
elif [ -f $EXPORT_SRC_DIR/build/export.sh ]; then
    sh $EXPORT_SRC_DIR/build/export.sh $EXPORT_DST_DIR
elif [ -f $EXPORT_SRC_DIR/toolchain/export.sh ]; then
    sh $EXPORT_SRC_DIR/toolchain/export.sh $EXPORT_DST_DIR
elif [ -f ${EXPORT_SRC_DIR}/toolchain/build_path ]; then
    # 标准开发环境导出
    . ${EXPORT_SRC_DIR}/toolchain/build_path
    if [ ! -z ${TUYA_SDK_TOOLCHAIN_ZIP} ]; then
        # 第一级目录名
        TOOLCHAN_PATH=$(echo ${TUYA_SDK_BUILD_PATH} | cut -d'/' -f1)
        if [ x"" = x".." ]; then
            export_all
        elif [ ! -z ${TOOLCHAN_PATH} ]; then
            export_exclude_toolchain ${TOOLCHAN_PATH}
        else
            export_all
        fi
    else
        export_all
    fi
else
    export_all
fi

