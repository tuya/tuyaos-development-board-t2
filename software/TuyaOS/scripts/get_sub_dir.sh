
THIS_DIR=`dirname $0`


ls $1 -1F | grep -oP '[\w-]*(?=/)' | bash $THIS_DIR/listmenu.sh "Application" 20 60 13

