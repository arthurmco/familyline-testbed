####
# Tribalia makelist
#
# Copyright (C) 2016 Arthur M
####

cmake_minimum_required(VERSION 2.8)

project(Tribalia)

SET(GEN_HEADER "${CMAKE_SOURCE_DIR}/src/EnviroDefs.h")

# Create a include for generated content
file(WRITE "${GEN_HEADER}" "/* Auto-generated header by CMakeLists.txt \n")
file(APPEND "${GEN_HEADER}" " Do not directly edit it! */\n\n")

# Write version
file(APPEND "${GEN_HEADER}" "#define VERSION \"v0.0.1\" \n")

# Retrieve commit number (useful when getting logs from git clones)
execute_process(COMMAND sh "${CMAKE_SOURCE_DIR}/scripts/get_commit.sh"
  OUTPUT_VARIABLE COMMIT_NUM)

file(APPEND "${GEN_HEADER}" "#define COMMIT \"${COMMIT_NUM}\"\n")

file(GLOB_RECURSE SRC_LOGIC "${CMAKE_SOURCE_DIR}/src/logic/*.cpp")
file(GLOB SRC_BASE "${CMAKE_SOURCE_DIR}/src/*.cpp")


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")


add_executable(Tribalia ${SRC_BASE} ${SRC_LOGIC})
