#!/bin/bash

git submodule update --init --recursive
git pull --recurse-submodules

conan profile detect

VCPKG_DIR=./deps/vcpkg
VCPKG_TRIPLET=x64-windows
VCPKG_BOOTSTRAP=bootstrap-vcpkg.bat

COUNT_LINUX=`conan profile show | grep Linux | wc -l`
if [[ "$COUNT_LINUX" -ne 0 ]]; then
    VCPKG_TRIPLET=x64-linux
    VCPKG_BOOTSTRAP=bootstrap-vcpkg.sh
fi

echo "VCPKG_TRIPLET=$VCPKG_TRIPLET"

mkdir -p deps
git submodule add https://github.com/microsoft/vcpkg $VCPKG_DIR

(cd $VCPKG_DIR && git checkout master && git pull)
$VCPKG_DIR/$VCPKG_BOOTSTRAP

# update DOES NOT WORK IN MANIFEST MODE!
# $VCPKG_DIR/vcpkg --vcpkg-root $VCPKG_DIR update

$VCPKG_DIR/vcpkg --vcpkg-root $VCPKG_DIR search gmp
$VCPKG_DIR/vcpkg --vcpkg-root $VCPKG_DIR search mpir

$VCPKG_DIR/vcpkg --vcpkg-root $VCPKG_DIR integrate install

$VCPKG_DIR/vcpkg --vcpkg-root $VCPKG_DIR install

#$VCPKG_DIR/vcpkg --vcpkg-root $VCPKG_DIR remove gmp # --triplet=$VCPKG_TRIPLET

#$VCPKG_DIR/vcpkg --vcpkg-root $VCPKG_DIR install gmp --triplet=$VCPKG_TRIPLET

# vcpkg integrate install 
# https://devblogs.microsoft.com/cppblog/vcpkg-september-2022-release-is-now-available-celebrating-6-years-with-over-2000-libraries/
# https://learn.microsoft.com/en-us/vcpkg/examples/manifest-mode-cmake

###vcpkg install mpir --triplet=x64-windows-static
#
#
# CMake Error at scripts/cmake/vcpkg_configure_make.cmake:625 (message):
#  gmp requires autoconf from the system package manager (example: "sudo
#  apt-get install autoconf")
#Call Stack (most recent call first):
#  ports/gmp/portfile.cmake:85 (vcpkg_configure_make)
#  scripts/ports.cmake:147 (include)
#
# HOW TO DETECT THIS ERROR??
#
# SOLUTION: sudo apt install autoconf
#
# ==============
# other error:
#CMake Error at scripts/cmake/vcpkg_execute_required_process.cmake:112 (message):
#    Command failed: /usr/bin/autoreconf -vfi
#    Working Directory: /workspaces/Filipe-UERJ-BRKGA/deps/vcpkg/buildtrees/gmp/src/v6.2.1-0c723d4b6f.clean/
#    Error code: 1
#    See logs for more information:
#      /workspaces/Filipe-UERJ-BRKGA/deps/vcpkg/buildtrees/gmp/autoconf-x64-linux-err.log
#
# SOLUTION: issue: https://github.com/microsoft/vcpkg/issues/33014
#  apt-get install autoconf automake libtool
#
# ===============
# OK
#The package gmp can be imported via CMake FindPkgConfig module:
#
#    #  gmp
#    find_package(PkgConfig REQUIRED)
#    pkg_check_modules(gmp REQUIRED IMPORTED_TARGET gmp)
#    target_link_libraries(main PkgConfig::gmp)
#
#    # gmpxx
#    find_package(PkgConfig REQUIRED)
#    pkg_check_modules(gmpxx REQUIRED IMPORTED_TARGET gmpxx)
#    target_link_libraries(main PkgConfig::gmpxx)

# =============
# FROM CONAN!
# [cmake] -- MAIN CMAKE_LIBRARY_PATH:/root/.conan2/p/gmp4ac8a4314e429/p/lib
# [cmake] -- MAIN CMAKE_INCLUDE_PATH:/root/.conan2/p/gmp4ac8a4314e429/p/include
# [cmake] -- Found GMP: /root/.conan2/p/gmp4ac8a4314e429/p/include 
# ==============
#
# FROM vcpkg... 
# TOOLCHAIN: scripts/buildsystems/vcpkg.cmake
# ./deps/vcpkg/installed/x64-linux/include
# ./deps/vcpkg/installed/x64-linux/lib
# ============
# 