cmake_minimum_required(VERSION 3.22)
project(brkgp-project LANGUAGES CXX VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 11)
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

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/instances_short_range DESTINATION ${CMAKE_CURRENT_BINARY_DIR})