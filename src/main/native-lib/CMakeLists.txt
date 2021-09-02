cmake_minimum_required(VERSION 3.21)
project(native_lib)

set(CMAKE_CXX_STANDARD 17)

find_package(JNI REQUIRED)
include_directories(${JNI_INCLUDE_DIRS})

add_library(native_lib SHARED  library.cpp)
