echo 开发者缺少部分组件权限的话
echo 只有当CI线上有成功生成过某个版本的SDK产物后，
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


PROJECT_NAME=`print_not_null $1 || basename $PWD`
PROJECT_VERSION=`print_not_null $2 || bash ./scripts/get_ver_tag.sh`
TARGET_PLATFORM=`print_not_null $3 || bash ./scripts/get_platform.sh make.yaml`
echo PROJECT_NAME=$PROJECT_NAME
echo PROJECT_VERSION=$PROJECT_VERSION
echo TARGET_PLATFORM=$TARGET_PLATFORM


[ -z $PROJECT_NAME ] && echo "error: no project name!" && exit 99
[ -z $PROJECT_VERSION ] && echo "error: no version!" && exit 99
[ -z $TARGET_PLATFORM ] && echo "error: no platform!" && exit 99

FULL_NAME=$PROJECT_NAME"_"$PROJECT_VERSION"_"$TARGET_PLATFORM

DOWNLOAD_PATH=`ls ~/.embedded-cli/embed_repo_space/$PROJECT_NAME | grep ".*$FULL_NAME.*" | bash ./scripts/listmenu.sh "Download" 20 60 13`
if [ "$DOWNLOAD_PATH" = "" ]; then
    echo not found download path
    exit 0
fi

echo DOWNLOAD_PATH=$DOWNLOAD_PATH

DOWNLOAD_URL=`cat ~/.embedded-cli/embed_repo_space/$PROJECT_NAME/$DOWNLOAD_PATH/$PROJECT_NAME.yaml \
    | grep -A 4 '^\s*source:\s*$' \
    | sed -n 's/^\s*https:\s*\(.*\)$/\1/p'`
if [ "$DOWNLOAD_URL" = "" ]; then
    echo not found download url
    exit 0
fi

echo DOWNLOAD_URL=$DOWNLOAD_URL

rm -rf tmp
mkdir tmp
cd ./tmp

TAR_NAME=${PROJECT_NAME}_${TARGET_PLATFORM}_${PROJECT_VERSION}
wget $DOWNLOAD_URL -O ${TAR_NAME}.tar.gz
tar -xzvf ${TAR_NAME}.tar.gz

if [ -d $TAR_NAME/sdk/include ]; then 
    cp -a $TAR_NAME/sdk/include ../sdk/
fi

echo -----finished-----


