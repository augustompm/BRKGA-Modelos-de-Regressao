#!/bin/bash

conan profile detect

VCPKG_DIR=./deps/vcpkg
VCPKG_TRIPLET=x64-windows

COUNT_LINUX=`conan profile show | grep Linux | wc -l`
if [[ "$COUNT_LINUX" -ne 0 ]]; then
    VCPKG_TRIPLET=x64-linux
fi

echo "VCPKG_TRIPLET=$VCPKG_TRIPLET"

mkdir -p deps
git submodule add https://github.com/microsoft/vcpkg $VCPKG_DIR
$VCPKG_DIR/bootstrap-vcpkg.sh

$VCPKG_DIR/vcpkg --vcpkg-root $VCPKG_DIR search gmp

(cd $VCPKG_DIR && git pull)

$VCPKG_DIR/vcpkg --vcpkg-root $VCPKG_DIR update

$VCPKG_DIR/vcpkg --vcpkg-root $VCPKG_DIR install gmp # --triplet=$VCPKG_TRIPLET
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