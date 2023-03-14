
THIS_DIR=`dirname $0`

grep -A 2 '^\s*toolchain:' $1 | sed -n 's/^\s*name:\s*\(.*\)$/\1/p'       


