#!/bin/sh

COMPONENTS_ALL=$(ls -d . components/* adapter/*)

echo "uninstall pre-commit..."

pre-commit --version
if [ "$?" -ne 0 ];then
    echo "try to run 'pip3 install pre-commit' for pre-commit installation!"
    exit 1
fi

echo "$COMPONENTS_ALL" | xargs -I{} sh -c \
    'BANNER="\033[1;33m====================== {} ======================\033[0m" && \
    cd {} && \
    [ -f ".git/hooks/pre-commit" ] && \
    (echo "$BANNER" && pre-commit uninstall || echo "uninstall pre-commit ERROR!")'
echo "uninstall pre-commit DONE!"
echo "-----------------------------------"
