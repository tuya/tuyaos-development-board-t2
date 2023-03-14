#!/bin/sh

# ==============================================================================
# 1，此脚本用于快速更新开发环境kernel版本，适用于使用tuyaos_auto_tester导入的开发环境
# 2，如果要更新kernel模板，增加下能力，修改tuyaos_kernel.config即可
# 3，仅更新头文件，init和utilities组件，差异需要手动对比更新
# 4，更新代码需要手动提交
# ------------------------------------------------------------------------------
# 入参说明：
#
# $1 - kernel 版本，如： 1.0.0，此参数可选，如果不提供，则会更新到最新的kernel版本，如
#      果不提供，则$2必须手动选择（脚本能力限制）
# $2 - 开发环境名称，如： bk7231n，此参数可选，如果不提供，则会列出vendor下目录供选择
#
# ------------------------------------------------------------------------------
# 使用示例：
# ./kernel_update.sh 
# 或者
# ./kernel_update.sh 1.0.0
# 或者
# ./kernel_update.sh 1.0.0 bk7231n
# ------------------------------------------------------------------------------

print_not_null()
{
    # $1 为空，返回错误
    if [ x"$1" = x"" ]; then
        return 1
    fi

    echo "$1"
}

update_kernel_by_template()
{
    echo copy kernel $TARGET_PLATFORM_KERNEL_VERSION to $TARGET_PLATFORM_PATH ...
    echo TEMPLATE_PATH=$TEMPLATE_PATH
    [ ! -e $TEMPLATE_PATH ] && return 99 && echo "error: kernel template not existed!"
    
    if [ ! -d $KERNEL_PATH ]; then
        mkdir $TEMPLATE_PATH/include
    fi 

    # utilities
    rm -rf  $KERNEL_PATH/include/utilities
    mkdir -p $KERNEL_PATH/include/utilities
    cp -rf $TARGET_PLATFORM_PATH/tmp/kernel/adapter/utilities/include  $KERNEL_PATH/include/utilities/
    cp -rf $TARGET_PLATFORM_PATH/tmp/kernel/adapter/utilities/src  $KERNEL_PATH/include/utilities/

    # init
    rm -rf  $KERNEL_PATH/include/init
    mkdir -p $KERNEL_PATH/include/init
    cp -rf $TARGET_PLATFORM_PATH/tmp/kernel/adapter/init/include  $KERNEL_PATH/include/init/
    cp -rf $TARGET_PLATFORM_PATH/tmp/kernel/adapter/init/src  $KERNEL_PATH/include/init/
    rm $KERNEL_PATH/include/init/src/tkl_init.c -rf
    rm $KERNEL_PATH/include/init/src/tkl_init_bt.c -rf

    # security
    rm -rf  $KERNEL_PATH/include/security
    mkdir -p $KERNEL_PATH/include/security
    cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/security/include/*  $KERNEL_PATH/include/security/

    # network
    rm -rf  $KERNEL_PATH/include/network
    mkdir -p $KERNEL_PATH/include/network
    cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/network/include/*  $KERNEL_PATH/include/network/

    # system
    rm -rf  $KERNEL_PATH/include/system
    mkdir -p $KERNEL_PATH/include/system
    cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/system/include/*  $KERNEL_PATH/include/system/

    cat $TEMPLATE_PATH | while read line
    do 
        # null
        if [ x"$line" = x"" ] ; then
            continue
        fi

        # disable ability
        IS_DISABLE_ABILITY=$(echo "$line" | grep -b -o "#" | cut -d: -f1)
        if [ ! x"$IS_DISABLE_ABILITY" = x"" ] ; then
            ABILITY=$(echo "$line" | cut -d " " -f2)
            [ x"$ABILITY" = x"CONFIG_ENABLE_WIRED" ] && rm -rf  $KERNEL_PATH/include/wired && rm $KERNEL_PATH/include/init/include/tkl_init_wired.h && rm $KERNEL_PATH/include/init/src/tkl_init_wired.c && echo ABILITY $ABILITY is disabled, rm wired
            [ x"$ABILITY" = x"CONFIG_ENABLE_WIFI" ] && rm -rf  $KERNEL_PATH/include/wifi && rm $KERNEL_PATH/include/init/include/tkl_init_wifi.h && rm $KERNEL_PATH/include/init/src/tkl_init_wifi.c && echo ABILITY $ABILITY is disabled, rm wifi
            [ x"$ABILITY" = x"CONFIG_ENABLE_NB" ] && rm -rf  $KERNEL_PATH/include/nb && rm $KERNEL_PATH/include/init/include/tkl_init_nbiot.h && rm $KERNEL_PATH/include/init/src/tkl_init_nbiot.c && echo ABILITY $ABILITY is disabled, rm nb
            [ x"$ABILITY" = x"CONFIG_ENABLE_PM" ] && rm -rf  $KERNEL_PATH/include/pm && rm $KERNEL_PATH/include/init/include/tkl_init_pm.h && rm $KERNEL_PATH/include/init/src/tkl_init_pm.c && echo ABILITY $ABILITY is disabled, rm pm
            [ x"$ABILITY" = x"CONFIG_ENABLE_CELLULAR" ] && rm -rf  $KERNEL_PATH/include/cellular && rm $KERNEL_PATH/include/init/include/tkl_init_cellular.h && rm $KERNEL_PATH/include/init/src/tkl_init_cellular.c && echo ABILITY $ABILITY is disabled, rm cellular
            [ x"$ABILITY" = x"CONFIG_ENABLE_THIRD_NETWORK" ] && rm $KERNEL_PATH/include/network/tkl_network.h && rm $KERNEL_PATH/include/init/include/tkl_init_network.h && rm $KERNEL_PATH/include/init/src/tkl_init_network.c && echo ABILITY $ABILITY is disabled, rm wakeup            
            [ x"$ABILITY" = x"CONFIG_ENABLE_BLUETOOTH" ] && rm -rf  $KERNEL_PATH/include/bluetooth && echo ABILITY $ABILITY is disabled, rm bluetooth
            [ x"$ABILITY" = x"CONFIG_ENABLE_ZIGBEE" ] && rm -rf  $KERNEL_PATH/include/zigbee && echo ABILITY $ABILITY is disabled, rm zigbee
            [ x"$ABILITY" = x"CONFIG_ENABLE_RTC" ] && rm -rf  $KERNEL_PATH/include/rtc && echo ABILITY $ABILITY is disabled, rm rtc
            [ x"$ABILITY" = x"CONFIG_ENABLE_ADC" ] && rm -rf  $KERNEL_PATH/include/adc && echo ABILITY $ABILITY is disabled, rm adc
            [ x"$ABILITY" = x"CONFIG_ENABLE_UART" ] && rm -rf  $KERNEL_PATH/include/uart && echo ABILITY $ABILITY is disabled, rm uart
            [ x"$ABILITY" = x"CONFIG_ENABLE_WATCHDOG" ] && rm -rf  $KERNEL_PATH/include/watchdog && echo ABILITY $ABILITY is disabled, rm watchdog
            [ x"$ABILITY" = x"CONFIG_ENABLE_FLASH" ] && rm -rf  $KERNEL_PATH/include/flash && echo ABILITY $ABILITY is disabled, rm flash
            [ x"$ABILITY" = x"CONFIG_ENABLE_PWM" ] && rm -rf  $KERNEL_PATH/include/pwm && echo ABILITY $ABILITY is disabled, rm pwm
            [ x"$ABILITY" = x"CONFIG_ENABLE_GPIO" ] && rm -rf  $KERNEL_PATH/include/gpio && echo ABILITY $ABILITY is disabled, rm gpio
            [ x"$ABILITY" = x"CONFIG_ENABLE_TIMER" ] && rm -rf  $KERNEL_PATH/include/timer && echo ABILITY $ABILITY is disabled, rm timer
            [ x"$ABILITY" = x"CONFIG_ENABLE_HCI" ] && rm -rf  $KERNEL_PATH/include/hci && echo ABILITY $ABILITY is disabled, rm hci
            [ x"$ABILITY" = x"CONFIG_ENABLE_DISPLAY" ] && rm -rf  $KERNEL_PATH/include/dispaly && echo ABILITY $ABILITY is disabled, rm dispaly
            [ x"$ABILITY" = x"CONFIG_ENABLE_MEDIA" ] && rm -rf  $KERNEL_PATH/include/media && echo ABILITY $ABILITY is disabled, rm media
            [ x"$ABILITY" = x"CONFIG_ENABLE_CELLULAR" ] && rm -rf  $KERNEL_PATH/include/cellular && echo ABILITY $ABILITY is disabled, rm cellular
            [ x"$ABILITY" = x"CONFIG_ENABLE_STORAGE" ] && rm -rf  $KERNEL_PATH/include/storage && echo ABILITY $ABILITY is disabled, rm storage
            [ x"$ABILITY" = x"CONFIG_ENABLE_DAC" ] && rm -rf  $KERNEL_PATH/include/dac && echo ABILITY $ABILITY is disabled, rm dac
            [ x"$ABILITY" = x"CONFIG_ENABLE_I2C" ] && rm -rf  $KERNEL_PATH/include/i2c && echo ABILITY $ABILITY is disabled, rm i2c
            [ x"$ABILITY" = x"CONFIG_ENABLE_I2S" ] && rm -rf  $KERNEL_PATH/include/i2s && echo ABILITY $ABILITY is disabled, rm i2s
            [ x"$ABILITY" = x"CONFIG_ENABLE_PINMUX" ] && rm -rf  $KERNEL_PATH/include/pinmux && echo ABILITY $ABILITY is disabled, rm pinmux
            [ x"$ABILITY" = x"CONFIG_ENABLE_WAKEUP" ] && rm -rf  $KERNEL_PATH/include/wakeup && echo ABILITY $ABILITY is disabled, rm wakeup
            [ x"$ABILITY" = x"CONFIG_GPRS_GW" ] && rm -rf  $KERNEL_PATH/include/gprs_gw && echo ABILITY $ABILITY is disabled, rm gprs_gw
            [ x"$ABILITY" = x"CONFIG_ENABLE_REGISTER" ] && rm -rf  $KERNEL_PATH/include/gprs_gw && echo ABILITY $ABILITY is disabled, rm register

            continue
        fi

        # enable ability
        ABILITY=$(echo "$line" | cut -d "=" -f 1)
        ABILITY_STATUS=$(echo "$line" | cut -d "=" -f 2)
        echo ABILITY $ABILITY is enable, update...
        if [ x"$ABILITY" = x"CONFIG_ENABLE_WIRED" ]; then
            rm -rf  $KERNEL_PATH/include/wired
            mkdir -p $KERNEL_PATH/include/wired
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/wired/include/* $KERNEL_PATH/include/wired/
        fi

        if [ x"$ABILITY" = x"CONFIG_ENABLE_WIFI" ]; then
            rm -rf  $KERNEL_PATH/include/wifi
            mkdir -p $KERNEL_PATH/include/wifi
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/wifi/include/*  $KERNEL_PATH/include/wifi/
        fi

        if [ x"$ABILITY" = x"CONFIG_ENABLE_WIFI" ]; then
            rm -rf  $KERNEL_PATH/include/wifi
            mkdir -p $KERNEL_PATH/include/wifi
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/wifi/include/*  $KERNEL_PATH/include/wifi/
        fi

        if [ x"$ABILITY" = x"CONFIG_ENABLE_BLUETOOTH" ]; then
            rm -rf  $KERNEL_PATH/include/bluetooth
            mkdir -p $KERNEL_PATH/include/bluetooth
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/bluetooth/include/*  $KERNEL_PATH/include/bluetooth/
        fi

        if [ x"$ABILITY" = x"CONFIG_ENABLE_ZIGBEE" ]; then
            rm -rf  $KERNEL_PATH/include/zigbee
            mkdir -p $KERNEL_PATH/include/zigbee
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/zigbee/include/*  $KERNEL_PATH/include/zigbee/
        fi
        
        if [ x"$ABILITY" = x"CONFIG_ENABLE_RTC" ]; then
            rm -rf  $KERNEL_PATH/include/rtc
            mkdir -p $KERNEL_PATH/include/rtc
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/rtc/include/*  $KERNEL_PATH/include/rtc/
        fi

        if [ x"$ABILITY" = x"CONFIG_ENABLE_ADC" ]; then
            rm -rf  $KERNEL_PATH/include/adc
            mkdir -p $KERNEL_PATH/include/adc
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/adc/include/*  $KERNEL_PATH/include/adc/
        fi

        if [ x"$ABILITY" = x"CONFIG_ENABLE_UART" ]; then
            rm -rf  $KERNEL_PATH/include/uart
            mkdir -p $KERNEL_PATH/include/uart
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/uart/include/*  $KERNEL_PATH/include/uart/
        fi

        if [ x"$ABILITY" = x"CONFIG_ENABLE_WATCHDOG" ]; then
            rm -rf  $KERNEL_PATH/include/watchdog
            mkdir -p $KERNEL_PATH/include/watchdog
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/watchdog/include/*  $KERNEL_PATH/include/watchdog/
        fi        

        if [ x"$ABILITY" = x"CONFIG_ENABLE_FLASH" ]; then
            rm -rf  $KERNEL_PATH/include/flash
            mkdir -p $KERNEL_PATH/include/flash
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/flash/include/*  $KERNEL_PATH/include/flash/
        fi        

        if [ x"$ABILITY" = x"CONFIG_ENABLE_PWM" ]; then
            rm -rf  $KERNEL_PATH/include/pwm
            mkdir -p $KERNEL_PATH/include/pwm
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/pwm/include/*  $KERNEL_PATH/include/pwm/
        fi 

        if [ x"$ABILITY" = x"CONFIG_ENABLE_GPIO" ]; then
            rm -rf  $KERNEL_PATH/include/gpio
            mkdir -p $KERNEL_PATH/include/gpio
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/gpio/include/*  $KERNEL_PATH/include/gpio/
        fi 

        if [ x"$ABILITY" = x"CONFIG_ENABLE_TIMER" ]; then
            rm -rf  $KERNEL_PATH/include/timer
            mkdir -p $KERNEL_PATH/include/timer
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/timer/include/*  $KERNEL_PATH/include/timer/
        fi 

        if [ x"$ABILITY" = x"CONFIG_ENABLE_HCI" ]; then
            rm -rf  $KERNEL_PATH/include/bluetooth
            rm -rf  $KERNEL_PATH/include/hci
            mkdir -p $KERNEL_PATH/include/bluetooth
            mkdir -p $KERNEL_PATH/include/hci
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/bluetooth/include/*  $KERNEL_PATH/include/bluetooth/
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/hci/include/*  $KERNEL_PATH/include/hci/
        fi 

        if [ x"$ABILITY" = x"CONFIG_ENABLE_DISPLAY" ]; then
            rm -rf  $KERNEL_PATH/include/dispaly
            mkdir -p $KERNEL_PATH/include/dispaly
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/dispaly/include/*  $KERNEL_PATH/include/dispaly/
        fi 

        if [ x"$ABILITY" = x"CONFIG_ENABLE_MEDIA" ]; then
            rm -rf  $KERNEL_PATH/include/media
            mkdir -p $KERNEL_PATH/include/media
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/media/include/*  $KERNEL_PATH/include/media/
        fi 

        if [ x"$ABILITY" = x"CONFIG_ENABLE_PM" ]; then
            rm -rf  $KERNEL_PATH/include/pm
            mkdir -p $KERNEL_PATH/include/pm
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/pm/include/*  $KERNEL_PATH/include/pm/
        fi 

        if [ x"$ABILITY" = x"CONFIG_ENABLE_CELLULAR" ]; then
            rm -rf  $KERNEL_PATH/include/cellular
            mkdir -p $KERNEL_PATH/include/cellular
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/cellular/include/*  $KERNEL_PATH/include/cellular/
        fi 

        if [ x"$ABILITY" = x"CONFIG_ENABLE_STORAGE" ]; then
            rm -rf  $KERNEL_PATH/include/storage
            mkdir -p $KERNEL_PATH/include/storage
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/storage/include/*  $KERNEL_PATH/include/storage/
        fi 

        if [ x"$ABILITY" = x"CONFIG_ENABLE_NB" ]; then
            rm -rf  $KERNEL_PATH/include/nb
            mkdir -p $KERNEL_PATH/include/nb
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/nb/include/*  $KERNEL_PATH/include/nb/
        fi 

        if [ x"$ABILITY" = x"CONFIG_ENABLE_DAC" ]; then
            rm -rf  $KERNEL_PATH/include/dac
            mkdir -p $KERNEL_PATH/include/dac
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/dac/include/*  $KERNEL_PATH/include/dac/
        fi 

        if [ x"$ABILITY" = x"CONFIG_ENABLE_I2C" ]; then
            rm -rf  $KERNEL_PATH/include/i2c
            mkdir -p $KERNEL_PATH/include/i2c
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/i2c/include/*  $KERNEL_PATH/include/i2c/
        fi 

        if [ x"$ABILITY" = x"CONFIG_ENABLE_I2S" ]; then
            rm -rf  $KERNEL_PATH/include/i2s
            mkdir -p $KERNEL_PATH/include/i2s
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/i2s/include/*  $KERNEL_PATH/include/i2s/
        fi 

        if [ x"$ABILITY" = x"CONFIG_ENABLE_PINMUX" ]; then
            rm -rf  $KERNEL_PATH/include/pinmux
            mkdir -p $KERNEL_PATH/include/pinmux
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/pinmux/include/*  $KERNEL_PATH/include/pinmux/
        fi 

        if [ x"$ABILITY" = x"CONFIG_ENABLE_WAKEUP" ]; then
            rm -rf  $KERNEL_PATH/include/wakeup
            mkdir -p $KERNEL_PATH/include/wakeup
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/wakeup/include/*  $KERNEL_PATH/include/wakeup/
        fi 

        if [ x"$ABILITY" = x"CONFIG_GPRS_GW" ]; then
            rm -rf  $KERNEL_PATH/include/gprs_gw
            mkdir -p $KERNEL_PATH/include/gprs_gw
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/gprs_gw/include/*  $KERNEL_PATH/include/gprs_gw/
        fi 

        if [ x"$ABILITY" = x"CONFIG_ENABLE_REGISTER" ]; then
            rm -rf  $KERNEL_PATH/include/register
            mkdir -p $KERNEL_PATH/include/register
            cp $TARGET_PLATFORM_PATH/tmp/kernel/adapter/register/include/*  $KERNEL_PATH/include/register/
        fi 

        if [ x"$ABILITY" = x"CONFIG_ENABLE_THIRD_NETWORK" ]; then
            rm  $KERNEL_PATH/include/network/tkl_lwip.h
        fi 
    done

}

# clone kernel repo
clone_kernel()
{
    echo clone kernel $TARGET_PLATFORM_KERNEL_VERSION to $TARGET_PLATFORM_PATH ...

    cd $TARGET_PLATFORM_PATH
    rm tmp -rf
    mkdir tmp
    cd tmp
    git clone https://registry.code.tuya-inc.top/embtuyaosimport/rtos-kernel/kernel.git
    cd kernel
    
    # 不提供版本则使用最新master
    if [ ! x"$TARGET_PLATFORM_KERNEL_VERSION" = x"latest" ] ; then
        git checkout $TARGET_PLATFORM_KERNEL_VERSION
    fi

    ./prepare.sh kernel $TARGET_PLATFORM_KERNEL_VERSION linux-ubuntu-6.2.0_64Bit
    cd 
}

clear_kernel()
{
    cd $TARGET_PLATFORM_PATH/tmp/kernel
    cat $TEMPLATE_PATH | sed -e '/CONFIG_KERNEL_VER/d' > $TEMPLATE_PATH.bak
    rm  $TEMPLATE_PATH
    mv  $TEMPLATE_PATH.bak $TEMPLATE_PATH
    echo "CONFIG_KERNEL_VER=\"$($ROOT_DIR/scripts/get_ver_tag.sh)\"" >>  $TEMPLATE_PATH

    rm -rf $TARGET_PLATFORM_PATH/tmp
}

set -e
cd `dirname $0`
CURRENT_DIR=$(pwd)
ROOT_DIR=$CURRENT_DIR/..
TARGET_PLATFORM_KERNEL_VERSION=`print_not_null $1 || echo latest`
TARGET_PLATFORM=`print_not_null $2 || bash ./get_platform.sh ../make.yaml`
TARGET_PLATFORM_PATH=$ROOT_DIR/vendor/$TARGET_PLATFORM
TEMPLATE_PATH=$TARGET_PLATFORM_PATH/tuyaos/tuyaos_kernel.config
KERNEL_PATH=$TARGET_PLATFORM_PATH/tuyaos/tuyaos_adapter
echo TARGET_PLATFORM=$TARGET_PLATFORM
echo TARGET_PLATFORM=$TARGET_PLATFORM_PATH
echo CURRENT_DIR=$CURRENT_DIR
echo ROOT_DIR=$ROOT_DIR
echo ABILITY=$ABILITY
[ -z $TARGET_PLATFORM ] && echo "error: no platform!" && exit 99
[ ! -d $TARGET_PLATFORM_PATH ] && echo "error: platform not exist!" && exit 99

clone_kernel
update_kernel_by_template
clear_kernel

echo update kernel to $TARGET_PLATFORM_KERNEL_VERSION success
cd $CURRENT_DIR
