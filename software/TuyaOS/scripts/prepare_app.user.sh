#!/usr/bin/env bash

print_not_null()
{
    # $1 为空，返回错误
    if [ x"$1" = x"" ]; then
        return 1
    fi

    echo "$1"
}


cd `dirname $0`

APP_NAME=`print_not_null $1 || bash ./scripts/get_app_name_cde.sh cluster.yaml ./tmp/app_names.log`
echo APP_NAME=$APP_NAME
[ -z $APP_NAME ] && echo "error: no app name!" && exit 99 

# 提取APP信息
cd ./apps/$APP_NAME
APP_VERSION=`print_not_null $2 || bash ../../scripts/get_ver_tag.sh`
TARGET_PLATFORM=`print_not_null $3 || bash ../../scripts/get_app_platform.sh app.yaml`
cd -

echo APP_VERSION= $APP_VERSION
echo TARGET_PLATFORM= $TARGET_PLATFORM
[ -z $APP_VERSION ] && echo "error: no app version!" && exit 99 
[ -z $TARGET_PLATFORM ] && echo "error: no platform!" && exit 99 

if [ -f vendor/$TARGET_PLATFORM/prepare.sh ]; then
    bash vendor/$TARGET_PLATFORM/prepare.sh
fi

mkdir -p ./tmp && echo $APP_NAME > ./tmp/app_name.log
python3 ./scripts/set_app_config.py $APP_NAME

bash ./scripts/prepare_param.sh $APP_NAME $APP_VERSION $TARGET_PLATFORM 

