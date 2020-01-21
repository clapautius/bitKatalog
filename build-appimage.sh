#!/bin/bash

# $1 - src dir, $2 - build dir

die()
{
    local rc="$1"
    shift
    echo -e "\n$*\n"
    popd > /dev/null
    exit $rc
}


locate_or_download_linuxdeployqt()
{
    which linuxdeployqt > /dev/null && return 3
    # trying to find it in current dir.
    export PATH="$PATH:./"
    which linuxdeployqt > /dev/null && return 3
    echo "linuxdeployqt not found, will try to download it"
    wget https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage &&
        echo "wget https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage OK" || echo "wget error"
    chmod +x ./linuxdeployqt-continuous-x86_64.AppImage
    ln -s ./linuxdeployqt-continuous-x86_64.AppImage ./linuxdeployqt
    return 0
}

pushd "." > /dev/null
[ -d "$1" ] || die 1 "No such dir: $1"
[ -d "$2" ] || die 1 "No such dir: $2"
src_dir=$(realpath "$1")
popd > /dev/null
pushd "$2" > /dev/null
locate_or_download_linuxdeployqt

# don't use standard KDE open/save dialogs, but Qt file dialogs (to prevent crashes on
# some systems)
cxxflags="-DUSE_QT_FILEDLG "

os_name=$(sed -n "/^ID=/ s/^ID=// p" /etc/os-release)
os_ver=$(sed -n "/^VERSION_ID=/ s/^VERSION_ID=\"\([^\"]*\)\"/\1/ p" /etc/os-release)
if [ $os_name=="ubuntu" -a $os_ver=="16.04" ]; then
    cxxflags="${cxxflags} -std=c++11 -fPIC "
    echo "Ubuntu 16.04 xenial detected, using custom CXX flags: $cxxflags"
fi

cmake -D CMAKE_CXX_FLAGS="$cxxflags" "$src_dir" || die 2 "cmake error"

make || die 2 "make error"
mkdir -p AppDir/bin || die 2 "mkdir error"
mkdir -p AppDir/share/applications || die 2 "mkdir error"

# copy file one by one (atm there's no "make install" :fixme:)
cp "$src_dir/data/bitKatalog.desktop" "AppDir/share/applications/"
cp "$src_dir/data/bitKatalog-icon.png" "AppDir/share/applications/"
cp "src/app_kde/bitKatalog" "AppDir/bin"

./linuxdeployqt AppDir/share/applications/bitKatalog.desktop -no-translations -appimage || die 2 "linuxdeployqt error"

popd > /dev/null

exit 0
