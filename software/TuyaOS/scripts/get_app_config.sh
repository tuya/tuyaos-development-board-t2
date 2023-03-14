#!/bin/sh

APP_NAME=$1
INPUT_NAME=`ls ./apps/${APP_NAME}/build/appconfig | bash ./scripts/listmenu.sh "config" 20 60 13`

echo $INPUT_NAME
