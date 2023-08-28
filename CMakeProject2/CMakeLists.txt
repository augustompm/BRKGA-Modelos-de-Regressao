cmake_minimum_required(VERSION 3.22)
project(brkgp-project LANGUAGES CXX VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ===================
# header only library
# -------------------
add_library(brkpg_biblioteca INTERFACE)
target_include_directories(brkpg_biblioteca INTERFACE include/)

add_executable(aplicacao src/main.cpp)
target_link_libraries(aplicacao PRIVATE brkpg_biblioteca Minha_pasta_external)

add_library(Minha_pasta_external INTERFACE)
target_include_directories(Minha_pasta_external INTERFACE external/)

#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/instances_short_range DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/instances DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# ============ SETUP GINAC, CLN AND GMP ==============

# =============================
# MAKE CLN FIND THE GMP LIBRARY
# =============================
#
# First, activate conan on CMakePresets.json with:
#    =>  "CMAKE_TOOLCHAIN_FILE": "./build-conan/conan_toolchain.cmake",
# Note that conan should get local GMP library and setup cmake paths to include it!
message(STATUS "MAIN CMAKE_LIBRARY_PATH:" ${CMAKE_LIBRARY_PATH})
message(STATUS "MAIN CMAKE_INCLUDE_PATH:" ${CMAKE_INCLUDE_PATH})
# Expects (for linux/devcontainer):
# [cmake] -- MAIN CMAKE_LIBRARY_PATH:/root/.conan2/p/gmp4ac8a4314e429/p/lib
# [cmake] -- MAIN CMAKE_INCLUDE_PATH:/root/.conan2/p/gmp4ac8a4314e429/p/include
# ---------------------------------------------
# Allow cln/cmake/modules/FindGMP.cmake to work
# ---------------------------------------------
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/cln/cmake/modules")
# -----------------------------------------------------------------------------------------
# Override build/gmp-config.cmake from conan (if toolchain is activated) - prefix is first!
# Note that we need to follow CLN recipes, not conan... as they are slightly different!
# conan demands find_package(gmp ...) while CLN deals with find_package(GMP ...)
# So, conan works with gmp_INCLUDE_DIR and CLN requires GMP_INCLUDE_DIR
# -----------------------------------------------------------------------------------------
set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/external/cln/cmake/modules")
# invoke FindGMP.cmake from CLN
find_package(GMP REQUIRED)
# This is just a small test, to make sure GMP is included in application, at this point.
target_link_libraries(aplicacao PRIVATE gmp::gmp)

# Allow GINAC to find CLN source files
set(CLN_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/cln)
#
add_subdirectory(external/ginac)
# ------------------------------------
# build ginac into current application
# ------------------------------------
target_link_libraries(aplicacao PRIVATE ginac)


#==========
#  TESTS
#----------
#Include(FetchContent)
#
#FetchContent_Declare(Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.3.1)
#FetchContent_MakeAvailable(Catch2)
#
#list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
#include(CTest)
#include(Catch)
#
#add_executable(my_test tests/get_data.test.cpp)
#target_link_libraries(my_test PRIVATE brkpg_biblioteca Minha_pasta_external Catch2::Catch2WithMain)
#catch_discover_tests(my_test)
