#!/bin/sh

APP_PATH=$1
APP_NAME=$2
APP_VERSION=$3
USER_CMD=$4
echo APP_PATH=$APP_PATH
echo APP_NAME=$APP_NAME
echo APP_VERSION=$APP_VERSION
echo USER_CMD=$USER_CMD


fatal() {
    echo -e "\033[0;31merror: $1\033[0m"
    exit 1
}


[ -z $APP_PATH ] && fatal "no app path!"
[ -z $APP_NAME ] && fatal "no app name!"
[ -z $APP_VERSION ] && fatal "no version!"


DEBUG_FLAG=`echo $APP_VERSION | sed -n 's,^[0-9]\+\.\([0-9]\+\)\.[0-9]\+\.*$,\1,p'`
if [ $((DEBUG_FLAG%2)) = 0 ]; then
    export APP_DEBUG=1
fi


cd `dirname $0`

TARGET_PLATFORM=t2
TARGET_PLATFORM_REPO=https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/smart/embed/pruduct/t2_1.4.0.zip
TARGET_PLATFORM_VERSION=1.4.0
ROOT_DIR=$(pwd)

# 下载编译环境
if [ ! -d vendor/$TARGET_PLATFORM ]; then
    if [ -n "$TARGET_PLATFORM_REPO" ]; then
        # download toolchain
        echo "=========开始：tuyaos kernel下载，请耐心等待5分钟========="
        mkdir -p vendor
        cd vendor
        rm -f ${TARGET_PLATFORM}_${TARGET_PLATFORM_VERSION}.zip
        wget $TARGET_PLATFORM_REPO 
        unzip -o ${TARGET_PLATFORM}_${TARGET_PLATFORM_VERSION}.zip

        if [ -f ${TARGET_PLATFORM}_${TARGET_PLATFORM_VERSION}_temp/${TARGET_PLATFORM}_${TARGET_PLATFORM_VERSION}.tar.gz ]; then
            tar xvf ${TARGET_PLATFORM}_${TARGET_PLATFORM_VERSION}_temp/${TARGET_PLATFORM}_${TARGET_PLATFORM_VERSION}.tar.gz -C ${TARGET_PLATFORM}_${TARGET_PLATFORM_VERSION}_temp
            rm ${TARGET_PLATFORM}_${TARGET_PLATFORM_VERSION}_temp/${TARGET_PLATFORM}_${TARGET_PLATFORM_VERSION}.tar.gz
        fi
        mkdir -p ${TARGET_PLATFORM}
        mv ${TARGET_PLATFORM}_${TARGET_PLATFORM_VERSION}_temp/* ${TARGET_PLATFORM}
        rm ${TARGET_PLATFORM}_${TARGET_PLATFORM_VERSION}_temp -rf
        cd -
        echo "---------完成：tuyaos kernel下载---------"
    fi
fi

# 判断当前编译环境是否OK
# 首先判断是否存在TUYAOS目录
TUYAOS_BUILD_PATH=${ROOT_DIR}/vendor/$TARGET_PLATFORM/tuyaos
export TUYAOS_BUILD_PATH=$TUYAOS_BUILD_PATH
if [ -e $TUYAOS_BUILD_PATH ]; then
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

if [ -e $TUYAOS_BUILD_PATH ]; then
    TUYAOS_BUILD_CMD_PATH=${ROOT_DIR}/vendor/$TARGET_PLATFORM/tuyaos
    if [ -z "$TUYA_TOOLCHAIN_PRE" ];then
        COMPILE_PREX=
    else
        COMPILE_PREX=${ROOT_DIR}/vendor/$TARGET_PLATFORM/tuyaos/$TUYA_TOOLCHAIN_PRE
    fi
else
    TUYAOS_BUILD_CMD_PATH=${ROOT_DIR}/vendor/$TARGET_PLATFORM/toolchain
    if [ -z "$TUYA_SDK_BUILD_PATH" ];then
        COMPILE_PREX=
    else
        COMPILE_PREX=${ROOT_DIR}/vendor/$TARGET_PLATFORM/toolchain/$TUYA_SDK_BUILD_PATH
    fi
fi

export COMPILE_PREX TARGET_PLATFORM
if [ -f ./prepare_app.sh ]; then
    sh ./prepare_app.sh $APP_NAME $APP_VERSION $TARGET_PLATFORM
fi
USER_SW_VER=`echo $APP_VERSION | grep -oP '\d*\.\d*\.\d*'`
if [ x"clean" = x$USER_CMD ]; then
    make app_clean_by_name APP_NAME=$APP_NAME APP_VER=$USER_SW_VER USER_CMD=$USER_CMD
else
    [ -f ./apps/$APP_NAME/build/tuya_app.config ] && make app_config
    make app_static_clean
    make app_by_name APP_NAME=$APP_NAME APP_VER=$USER_SW_VER USER_CMD=$USER_CMD
fi


