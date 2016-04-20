####
# Tribalia makefile
#
# Copyright (C) 2016 Arthur M
####

cmake_minimum_required(VERSION 2.8)

project(Tribalia)

file(GLOB_RECURSE SRC_LOGIC "${CMAKE_SOURCE_DIR}/src/logic/*.cpp")
file(GLOB SRC_BASE "${CMAKE_SOURCE_DIR}/src/*.cpp")


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")


add_executable(tribalia ${SRC_BASE} ${SRC_LOGIC})
