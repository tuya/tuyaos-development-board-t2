
THIS_DIR=`dirname $0`

grep -B 4 '^\s*locater:\s*\(platforms\|vendor\)\s*$' $1  \
    | sed -n 's/^\s*name:\s*\(.*\)$/\1/p'       \
    | bash $THIS_DIR/listmenu.sh "Platform" 20 60 13


