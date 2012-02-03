# Build file for Isana

cmake_minimum_required(VERSION 2.8)
project(Isana)
set(version "0.1")

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")
find_package(LLVM)

add_subdirectory(src)
