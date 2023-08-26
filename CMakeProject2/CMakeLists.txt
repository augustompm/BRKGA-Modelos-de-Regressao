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

# ============ GMP from CONAN ==============

# ...
find_package(gmp REQUIRED)
# ...
target_link_libraries(aplicacao PRIVATE gmp::gmp)

# =================== GINAC + CLN ======================

# INSTALL CLN: https://www.ginac.de/CLN/
#
## Set the path to the CLN library
##set(CLN_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/cln)
## Add the CLN library
##add_subdirectory(${CLN_INCLUDE_DIR})
# ON UBUNTU:
#RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#&& apt-get -y install --no-install-recommends libcln-dev

# Set the path to the GiNaC library
set(GINAC_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/ginac)

# Add the GiNaC library
add_subdirectory(${GINAC_INCLUDE_DIR})

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
