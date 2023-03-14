INPUT_NAME=`ls ./build/config | bash ./scripts/listmenu.sh "config" 20 60 13`

if [ "$INPUT_NAME" = "" ];then
    INPUT_NAME="../tuya_iot.config"
fi

echo $INPUT_NAME