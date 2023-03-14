cd `dirname $0`
#curl -fSsL https://registry.code.tuya-inc.top/embed_ci_space_group/scripts/raw/master/install_for_app.sh | install_dir=./ sh

# standard scripts
REPO_URL=https://registry.code.tuya-inc.top/embed_ci_space_group/cde_build_scripts
BRANCH=220727
TARGET_DIR=std_scripts
git clone $REPO_URL -b $BRANCH $TARGET_DIR
rm -rf ./$TARGET_DIR/.git
cp -ar ./$TARGET_DIR/* ./
rm -rf ./$TARGET_DIR

# kconfig
rm -rf ./kconfiglib
git clone https://registry.code.tuya-inc.top/embed_ci_space_group/kconfiglib.git
rm -rf ./kconfiglib/.git

# error_code
rm -rf ./error_code
git clone https://registry.code.tuya-inc.top/embed_ci_space_group/error_code.git
rm -rf ./error_code/.git

echo "tools update finished"
