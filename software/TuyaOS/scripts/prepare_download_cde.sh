#!/bin/sh

echo 开发者缺少部分组件权限的话
echo 只有当CDE线上有成功生成过某个版本的SDK产物后，
echo 线下使用此版本，并选择同样的产物时，
echo 会把成功产物中的库文件和头文件拷贝到本地，供开发者使用
echo 所以必须先保证线上编译通过才可以在本地开发

print_not_null()
{
    # $1 为空，返回错误
    if [ x"$1" = x"" ]; then
        return 1
    fi

    echo "$1"
}


set -e


TARGET_PLATFORM=`print_not_null $3 || bash ./scripts/get_platform.sh make.yaml`
CONFIG_NAME=`bash ./scripts/get_config.sh`
CDETOKEN=`embcli cdetoken`
echo TARGET_PLATFORM=$TARGET_PLATFORM
echo CONFIG_NAME=$CONFIG_NAME
echo CDETOKEN=$CDETOKEN


[ -z $TARGET_PLATFORM ] && echo "error: no platform!" && exit 99
[ -z $CONFIG_NAME ] && echo "error: no config!" && exit 99
[ -z $CDETOKEN ] && echo "error: no cdetoken!" && exit 99

python3 ./scripts/download_cde_package.py $TARGET_PLATFORM $CONFIG_NAME $CDETOKEN

if [ -d ./tmp/TuyaOS/include ]; then
    echo "coping files ..."
    cp -a ./tmp/TuyaOS/include ./
fi

echo -----finished-----


