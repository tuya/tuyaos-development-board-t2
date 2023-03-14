#!/bin/bash

cd $(dirname $0)/../

ROOT_DIR=$(pwd)
BUILD_DIR=${ROOT_DIR}/build #define bulid dir
GIT_USER_ALL=$(git config --get user.email)
GIT_USER=${GIT_USER_ALL%@*}
BUILD_TIME=$(date +%H_%M_%S)
BUILD_DATE=$(date +%Y_%m_%d)
PROJECT_NAME=$1
PROJECT_CFG_FILE=
TARGET_PLATFORM=$3
TARGET_PLATFORM_DOWNLOAD_URL=
BUILD_STATIC=1
BUILD_SHARED=
PROJECT_VERSION=$2
KERNEL_VERSION=$4


[ -z $PROJECT_NAME ] && echo "error: no project name!" && exit 99
[ -z $PROJECT_VERSION ] && echo "error: no version!" && exit 99
[ -z $TARGET_PLATFORM ] && echo "error: no platform!" && exit 99


SDK_FULL_VER=$PROJECT_VERSION
SDK_BETA_VER=`echo $PROJECT_VERSION | grep -o 'beta\.[0-9]\+'`
IOT_SDK_VER=`echo $PROJECT_VERSION | grep -o '[0-9]\+\.[0-9]\+\(\.[0-9]\+\)\?'`
IPC_SDK_VER=$IOT_SDK_VER
IOT_SDK_VER_MAJOR=`echo $IOT_SDK_VER | sed -n 's/^\([0-9]\+\)\..*/\1/p'`
IOT_SDK_VER_MINOR=`echo $IOT_SDK_VER | sed -n 's/^.*\.\([0-9]\+\)\..*/\1/p'`
IOT_SDK_VER_PATCH=`echo $IOT_SDK_VER | sed -n 's/.*\.\([0-9]\+\)$/\1/p'`


set -e

# 编译环境变量设置
TARGET_PLATFORM_VERSION=`grep -A 4 "^\s*name:\s*$TARGET_PLATFORM\s*$" make.yaml | sed -n 's/^\s*version:\s*\(.*\)\s*$/\1/p'`
TARGET_PLATFORM_REPO=https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/smart/embed/pruduct/${TARGET_PLATFORM}_${TARGET_PLATFORM_VERSION}.zip
echo "toolchain Name: $TARGET_PLATFORM"
echo "toolchain Repo: $TARGET_PLATFORM_REPO"
echo "toolchain Version: $TARGET_PLATFORM_VERSION"


# 判断当前编译环境是否OK
# 首先判断是否存在TUYAOS目录
PLATFPRM_TUYAOS_PATH=${ROOT_DIR}/vendor/$TARGET_PLATFORM/tuyaos
if [ -e $PLATFPRM_TUYAOS_PATH ]; then
    PLATFORM_BUILD_PATH_FILE=${ROOT_DIR}/vendor/$TARGET_PLATFORM/tuyaos/build_path
    if [ -e $PLATFORM_BUILD_PATH_FILE ]; then
        . $PLATFORM_BUILD_PATH_FILE
        if [ -n "$TUYA_TOOLCHAIN_ZIP" ]; then
            if [ ! -f ${ROOT_DIR}/vendor/${TARGET_PLATFORM}/tuyaos/${TUYA_TOOLCHAIN_PRE}gcc ]; then
                echo "unzip file $TUYA_TOOLCHAIN_ZIP"
                tar -xf ${ROOT_DIR}/vendor/$TARGET_PLATFORM/tuyaos/${TUYA_TOOLCHAIN_ZIP} -C ${ROOT_DIR}/vendor/$TARGET_PLATFORM/toolchain/
                echo "unzip finish"
            fi
        fi
    else
        echo "$PLATFORM_BUILD_PATH_FILE not found in vendor [ $TARGET_PLATFORM ]!"
    fi
else
    # 老的开发环境不存在TUYAOS目录，需要兼容
    PLATFORM_BUILD_PATH_FILE=${ROOT_DIR}/vendor/$TARGET_PLATFORM/toolchain/build_path
    if [ -e $PLATFORM_BUILD_PATH_FILE ]; then
        . $PLATFORM_BUILD_PATH_FILE
        if [ -n "$TUYA_SDK_TOOLCHAIN_ZIP" ]; then
            if [ ! -f ${ROOT_DIR}/vendor/${TARGET_PLATFORM}/toolchain/${TUYA_SDK_BUILD_PATH}gcc ]; then
                echo "unzip file $TUYA_SDK_TOOLCHAIN_ZIP"
                tar -xf ${ROOT_DIR}/vendor/$TARGET_PLATFORM/toolchain/$TUYA_SDK_TOOLCHAIN_ZIP -C ${ROOT_DIR}/vendor/$TARGET_PLATFORM/toolchain/
                echo "unzip finish"
            fi
        fi
    else
        echo "$PLATFORM_BUILD_PATH_FILE not found in vendor [ $TARGET_PLATFORM ]!"
    fi
fi

echo "++++++++++++++++++++ CI package fullname $CI_PACKAGE_FULLNAME ++++++++++++++++++++"
if [ -n "$CI_PACKAGE_FULLNAME" ]; then
    SDK_ID_TMP=`echo ${CI_PACKAGE_FULLNAME%.tar.gz*}`
    SDK_ID=`echo ${SDK_ID_TMP#*output/dist/}`
else
    SDK_ID=${PROJECT_NAME}_${SDK_FULL_VER}_${TARGET_PLATFORM}
fi
echo "++++++++++++++++++++ the SDK ID $SDK_ID ++++++++++++++++++++"

#生成build_param
rm -rf $BUILD_DIR/build_param;
echo -e "generate build_param for [ $PROJECT_NAME ] AT [ $TARGET_PLATFORM ]"
echo -e "# Project [ $PROJECT_NAME ] Param:" > $BUILD_DIR/build_param;
echo -e "\r\n" >> $BUILD_DIR/build_param;

echo -e "BUILD_DATE=$BUILD_DATE" >> $BUILD_DIR/build_param;
echo -e "BUILD_TIME=$BUILD_TIME" >> $BUILD_DIR/build_param;
echo -e "GIT_USER=$GIT_USER" >> $BUILD_DIR/build_param;
echo -e "IOT_SDK_VER=$IOT_SDK_VER" >> $BUILD_DIR/build_param;
echo -e "IPC_SDK_VER=$IPC_SDK_VER" >> $BUILD_DIR/build_param;
echo -e "SDK_FULL_VER=$SDK_FULL_VER" >> $BUILD_DIR/build_param;
echo -e "SDK_BETA_VER=$SDK_BETA_VER" >> $BUILD_DIR/build_param;
echo -e "IOT_SDK_VER_MAJOR=$IOT_SDK_VER_MAJOR" >> $BUILD_DIR/build_param;
echo -e "IOT_SDK_VER_MINOR=$IOT_SDK_VER_MINOR" >> $BUILD_DIR/build_param;
echo -e "IOT_SDK_VER_PATCH=$IOT_SDK_VER_PATCH" >> $BUILD_DIR/build_param;
echo -e "KERNEL_VERSION=$KERNEL_VERSION" >> $BUILD_DIR/build_param;
echo -e "PROJECT_NAME=$PROJECT_NAME" >> $BUILD_DIR/build_param;
echo -e "TARGET_PLATFORM=$TARGET_PLATFORM" >> $BUILD_DIR/build_param;
echo -e "TARGET_PLATFORM_REPO=$TARGET_PLATFORM_REPO" >> $BUILD_DIR/build_param;
echo -e "TARGET_PLATFORM_VERSION=$TARGET_PLATFORM_VERSION" >> $BUILD_DIR/build_param;
echo -e "BUILD_STATIC=$BUILD_STATIC" >> $BUILD_DIR/build_param;
echo -e "BUILD_SHARED=$BUILD_SHARED" >> $BUILD_DIR/build_param;
echo -e "SDK_ID=$SDK_ID" >> $BUILD_DIR/build_param;
echo -e "\r\n" >> $BUILD_DIR/build_param;

echo -e "ROOT_DIR=${ROOT_DIR}" >> $BUILD_DIR/build_param;
echo -e "OUTPUT_DIR=\$(ROOT_DIR)/output/\$(TARGET_PLATFORM)_\$(PROJECT_NAME)" >> $BUILD_DIR/build_param;
echo -e "OUTPUT_DIR_INC=\$(OUTPUT_DIR)/include" >> $BUILD_DIR/build_param;
echo -e "OUTPUT_DIR_STATIC_LIB=\$(OUTPUT_DIR)/static/lib" >> $BUILD_DIR/build_param;
echo -e "OUTPUT_DIR_SHARED_LIB=\$(OUTPUT_DIR)/shared/lib" >> $BUILD_DIR/build_param;
echo -e "OUTPUT_DIR_STATIC_OBJS=\$(OUTPUT_DIR)/static/objs" >> $BUILD_DIR/build_param;
echo -e "OUTPUT_DIR_SHARED_OBJS=\$(OUTPUT_DIR)/shared/objs" >> $BUILD_DIR/build_param;
echo -e "\r\n" >> $BUILD_DIR/build_param;


if [ -e $PLATFPRM_TUYAOS_PATH ]; then
    if [ -z "$TUYA_TOOLCHAIN_PRE" ];then
        echo -e "COMPILE_PREX = " >> $BUILD_DIR/build_param;
    else
        echo -e "COMPILE_PREX = ${ROOT_DIR}/vendor/$TARGET_PLATFORM/tuyaos/$TUYA_TOOLCHAIN_PRE" >> $BUILD_DIR/build_param;
    fi
    echo -e "TUYAOS_BUILD_PATH = ${ROOT_DIR}/vendor/$TARGET_PLATFORM/tuyaos" >> $BUILD_DIR/build_param;
else
    if [ -z "$TUYA_SDK_BUILD_PATH" ];then
        echo -e "COMPILE_PREX = " >> $BUILD_DIR/build_param;
    else
        echo -e "COMPILE_PREX = ${ROOT_DIR}/vendor/$TARGET_PLATFORM/toolchain/$TUYA_SDK_BUILD_PATH" >> $BUILD_DIR/build_param;
    fi
    echo -e "TUYAOS_BUILD_PATH = ${ROOT_DIR}/vendor/$TARGET_PLATFORM/toolchain" >> $BUILD_DIR/build_param;
fi

cat $BUILD_DIR/build_param.template >> $BUILD_DIR/build_param;
echo -e "\r\n" >> $BUILD_DIR/build_param;


echo -e "+++++++++++++++++++++++++++++++++++\r\n"
cat $BUILD_DIR/build_param
echo -e "\r\n-----------------------------------"

