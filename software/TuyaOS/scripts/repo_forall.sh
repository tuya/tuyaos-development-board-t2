
# echo '$1'="$1"
# echo '$2'="$2"
# echo '$3'="$3"
# echo '$4'="$4"
# echo '$5'="$5"

echo "repo for all ..."

while read REPO_VAR; do
    echo "------------------------------------"

    REPO_NAME=
    REPO_REF=
    REPO_BRANCH=
    REPO_TARGET=
    eval "${REPO_VAR}"
    echo "REPO_NAME=$REPO_NAME"
    echo "REPO_REF=$REPO_REF"
    echo "REPO_BRANCH=$REPO_BRANCH"
    echo "REPO_TARGET=$REPO_TARGET"

    cd $REPO_TARGET/$REPO_NAME
    eval "$@"
    cd -
done

