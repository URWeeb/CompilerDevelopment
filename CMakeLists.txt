cmake_minimum_required(VERSION 3.13.4)
project(mtsist-llvm-tasks)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT LT_LLVM_INSTALL_DIR)
  message(FATAL_ERROR "LT_LLVM_INSTALL_DIR не задан. Пример:\n"
                       "  cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR .")
endif()

find_package(LLVM REQUIRED CONFIG
  PATHS "${LT_LLVM_INSTALL_DIR}/build/lib/cmake/llvm"
        "${LT_LLVM_INSTALL_DIR}/lib/cmake/llvm"
  NO_DEFAULT_PATH)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
list(APPEND CMAKE_MODULE_PATH ${LLVM_CMAKE_DIR})
include(AddLLVM)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})

if(NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

add_library(RPOPass MODULE src/RPOPass.cpp)
add_library(InstCountPass MODULE src/InstCountPass.cpp)
