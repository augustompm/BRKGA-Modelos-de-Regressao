cmake_minimum_required(VERSION 3.22)

#set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/build-conan/conan_toolchain.cmake" CACHE STRING "")
#set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../deps/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")


if(EXISTS ${CMAKE_TOOLCHAIN_FILE})
message(STATUS "MAIN CMAKE_TOOLCHAIN_FILE EXISTS:" ${CMAKE_TOOLCHAIN_FILE})
else()
message(STATUS "MAIN CMAKE_TOOLCHAIN_FILE NOT FOUND:" ${CMAKE_TOOLCHAIN_FILE})
endif()

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
message(STATUS "MAIN CMAKE_LIBRARY_PATH:" "${CMAKE_LIBRARY_PATH}")
message(STATUS "MAIN CMAKE_INCLUDE_PATH:" "${CMAKE_INCLUDE_PATH}")
# Expects (for linux/devcontainer):
# [cmake] -- MAIN CMAKE_LIBRARY_PATH:/root/.conan2/p/gmp4ac8a4314e429/p/lib
# [cmake] -- MAIN CMAKE_INCLUDE_PATH:/root/.conan2/p/gmp4ac8a4314e429/p/include
# ---------------------------------------------
# Allow cln/cmake/modules/FindGMP.cmake to work
# ---------------------------------------------
#find_path(GMP_INCLUDE_DIR NAMES gmp.h)
#find_library(GMP_LIBRARIES NAMES gmp libgmp)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cln/cmake/modules")
# -----------------------------------------------------------------------------------------
# Override build/gmp-config.cmake from conan (if toolchain is activated) - prefix is first!
# Note that we need to follow CLN recipes, not conan... as they are slightly different!
# conan demands find_package(gmp ...) while CLN deals with find_package(GMP ...)
# So, conan works with gmp_INCLUDE_DIR and CLN requires GMP_INCLUDE_DIR
# -----------------------------------------------------------------------------------------
set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/cln/cmake/modules")

message(STATUS "MAIN INCLUDE:" ${INCLUDE})
message(STATUS "MAIN PATH:" ${PATH})
message(STATUS "MAIN CMAKE_FRAMEWORK_PATH:" ${CMAKE_FRAMEWORK_PATH})
# MUST USE QUOTES ON VARIABLE, IF THEY ARE LISTS!! SO THEY GET ';' BETWEEN
message(STATUS "MAIN CMAKE_FIND_ROOT_PATH:" "${CMAKE_FIND_ROOT_PATH}")
list(LENGTH CMAKE_FIND_ROOT_PATH MY_SIZE)
message(STATUS "MAIN MY_SIZE:" ${MY_SIZE})
foreach(P ${CMAKE_FIND_ROOT_PATH})
  message(STATUS "MAIN P:" ${P})
endforeach()



list(APPEND CMAKE_FIND_ROOT_PATH "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows-static")
message(STATUS "MAIN FORCE CMAKE_FIND_ROOT_PATH:" "${CMAKE_FIND_ROOT_PATH}")

message(STATUS "\nMAIN BEGIN TESTING find_path and find_library")
find_path(MAIN_GMP_INCLUDE_DIR NAMES gmp.h  ) 
find_library(MAIN_GMP_LIBRARIES NAMES gmp libgmp  )
message(STATUS "MAIN_GMP_INCLUDE_DIR:" "${MAIN_GMP_INCLUDE_DIR}")
message(STATUS "MAIN_GMP_LIBRARIES:" "${MAIN_GMP_LIBRARIES}")
message(STATUS "MAIN FINISHED TESTING find_path and find_library\n")
# CLEAR UNIMPORTANT STUFF
unset(MAIN_GMP_INCLUDE_DIR)
unset(MAIN_GMP_LIBRARIES)
# CLEAR IMPORTANT STUFF
unset(CMAKE_FIND_ROOT_PATH)
unset(GMP_INCLUDE_DIR)
unset(GMP_LIBRARIES)
# ================================================
# EXPLICIT LOCATION FOR GMP FILES ON VCPKG!
# NOT USING TOOLCHAIN CMAKE/VCPKG AT THE MOMENT...
# ================================================
if(UNIX)
message(STATUS "MAIN THIS IS LINUX!")
set(GMP_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-linux/include")
set(GMP_LIBRARIES "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-linux/lib/libgmp.a")
else() # WINDOWS?
message(STATUS "MAIN THIS IS WINDOWS!")
set(GMP_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows-static/include")
set(GMP_LIBRARIES "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows-static/lib") 
endif()
#
message(STATUS "\nMAIN MANUAL GMP_INCLUDE_DIR: ${GMP_INCLUDE_DIR}")
message(STATUS "MAIN MANUAL GMP_LIBRARIES: ${GMP_LIBRARIES}\n")
message(STATUS "MAIN MANUAL CMAKE_CXX_LIBRARY_ARCHITECTURE: ${CMAKE_CXX_LIBRARY_ARCHITECTURE}\n")
#


#set(GMP_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../deps/vcpkg/installed/x64-linux/include")
#set(GMP_LIBRARIES "${CMAKE_SOURCE_DIR}/../deps/vcpkg/installed/x64-linux/lib/libgmp.a")

# invoke FindGMP.cmake from CLN
find_package(GMP REQUIRED)
# This is just a small test, to make sure GMP is included in application, at this point.
target_link_libraries(aplicacao PRIVATE gmp::gmp)

# Allow GINAC to find CLN source files
set(CLN_SOURCE_DIR ${CMAKE_SOURCE_DIR}/cln)
#
add_subdirectory(ginac)
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
