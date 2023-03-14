#!/bin/sh

COMPONENTS_ALL=$(ls -d . components/* adapter/*)

echo "install pre-commit..."

pre-commit --version
if [ "$?" -ne 0 ];then
    echo "try to run 'pip3 install pre-commit' for pre-commit installation!"
    exit 1
fi

echo "$COMPONENTS_ALL" | xargs -I{} sh -c \
    'BANNER="\033[1;33m====================== {} ======================\033[0m" && \
    cd {} && \
    [ -f ".pre-commit-config.yaml" ] && \
    [ ! -f ".git/hooks/pre-commit" ] && \
    (echo "$BANNER" && pre-commit install --allow-missing-config || echo "install pre-commit ERROR!")'
echo "install pre-commit DONE!"
echo "-----------------------------------"
