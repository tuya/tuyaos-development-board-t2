
#echo "REPO_NAME=base_db;REPO_REF=1.0.0;REPO_BRANCH=master;REPO_TARGET=sdk;"


PARSER_SEPERATOR="===PARSER_SEPERATOR==="


sed "/\s*-\s*.*:\s*$/i$PARSER_SEPERATOR" $1 | while read LINE; do
    #echo "LINE=$LINE"
    if [ x"$LINE" = x"$PARSER_SEPERATOR" ]; then
        if [ -n "$REPO_NAME" ]; then
            # 在一行中输出一个 repo 信息
            echo "REPO_NAME=$REPO_NAME;REPO_REF=$REPO_REF;REPO_BRANCH=$REPO_BRANCH;REPO_TARGET=$REPO_TARGET;"
            # 清空仓库名
            REPO_NAME=
        fi
        PARSER_READY=1
    elif [ x"$PARSER_READY" = x"1" ]; then
        TMP_NAME=`echo "$LINE" | sed -n 's/\s*-\s*\(.*\):\s*$/\1/p'`
        if [ x"$TMP_NAME" = x"" ]; then
            TMP_KEY=`echo "$LINE" | sed -n 's/\s*\(.*\):\s*\(.*\)\s*$/\1/p'`
            TMP_VALUE=`echo "$LINE" | sed -n 's/\s*\(.*\):\s*\(.*\)\s*$/\2/p'`
            if [ x"$TMP_KEY" = x"version" ]; then
                REPO_REF=$TMP_VALUE
            elif [ x"$TMP_KEY" = x"branch" ]; then
                REPO_BRANCH=$TMP_VALUE
            elif [ x"$TMP_KEY" = x"target" ]; then
                REPO_TARGET=$TMP_VALUE
            fi
        else
            REPO_NAME=$TMP_NAME
        fi
    fi
    #echo "REPO_NAME=$REPO_NAME;REPO_REF=$REPO_REF;REPO_BRANCH=$REPO_BRANCH;REPO_TARGET=$REPO_TARGET;"
done

