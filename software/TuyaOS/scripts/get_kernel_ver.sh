
THIS_DIR=`dirname $0`

grep -m1 'import:\s*kernel[-_0-9a-zA-Z]*' $1 | sed -n 's/^\s*import:\s*kernel[-_0-9a-zA-Z]*\s*(\(.*\))$/\1/p'


