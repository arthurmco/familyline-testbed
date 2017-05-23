####
# Tribalia makelist
#
# Copyright (C) 2016 Arthur M
####

cmake_minimum_required(VERSION 2.8)

project(Tribalia)

SET(GEN_HEADER "${CMAKE_SOURCE_DIR}/src/EnviroDefs.h")
configure_file("${CMAKE_SOURCE_DIR}/src/EnviroDefs.h.in" "${CMAKE_SOURCE_DIR}/src/EnviroDefs.h" @ONLY)

SET(T_VERSION "0.1.2")

file(APPEND "${GEN_HEADER}" "#define VERSION \"${T_VERSION}\" \n")

# Retrieve commit number (useful when getting logs from git clones)
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE COMMIT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE )
file(APPEND "${GEN_HEADER}" "#define COMMIT \"${COMMIT_HASH}\" \n")

# Retrieve the user who compiled
execute_process(COMMAND whoami OUTPUT_VARIABLE USERNAME
  OUTPUT_STRIP_TRAILING_WHITESPACE)
file(APPEND "${GEN_HEADER}" "#define USERNAME \"${USERNAME}\"\n")

#Tribalia game data
file(GLOB_RECURSE SRC_LOGIC "${CMAKE_SOURCE_DIR}/src/logic/*.cpp")
file(GLOB_RECURSE SRC_GFX "${CMAKE_SOURCE_DIR}/src/graphical/*.cpp")
file(GLOB_RECURSE SRC_GUI "${CMAKE_SOURCE_DIR}/src/graphical/gui/*.cpp")
file(GLOB_RECURSE SRC_INPUT "${CMAKE_SOURCE_DIR}/src/input/*.cpp")
file(GLOB_RECURSE SRC_OBJECTS "${CMAKE_SOURCE_DIR}/src/objects/*.cpp")
file(GLOB SRC_BASE "${CMAKE_SOURCE_DIR}/src/*.cpp")

# Tribalia server data
file(GLOB_RECURSE SRC_SERVER "${CMAKE_SOURCE_DIR}/server/*.cpp")

#include_directories(${SDL2_INCLUDE_DIR})

#Compilation data

add_executable(tribalia ${SRC_BASE} ${SRC_LOGIC} ${SRC_GFX} ${SRC_GUI} ${SRC_INPUT} ${SRC_OBJECTS})
add_executable(tribalia-server ${SRC_SERVER})

# Add test suites
include("test/unit-tests.cmake")

if (MSVC)
  # Set include directories
  SET(SDL_INCLUDE "." CACHE PATH "SDL2 include directory")
  SET(GLEW_INCLUDE "." CACHE PATH "GLEW include directory")
  SET(GLM_INCLUDE "." CACHE PATH "GLM include directory")
  SET(DEVIL_INCLUDE "." CACHE PATH "DevIL include directory")
  SET(CAIRO_INCLUDE "." CACHE PATH "Cairo include directory")

  include_directories(AFTER ${SDL_INCLUDE} ${GLEW_INCLUDE} ${GLM_INCLUDE} ${DEVIL_INCLUDE} ${CAIRO_INCLUDE})

  #Set library directories
  SET(SDL_LIB_FOLDER "." CACHE PATH "SDL2 library directory")
  SET(GLEW_LIB_FOLDER "." CACHE PATH "GLEW library directory")
  SET(DEVIL_LIB_FOLDER "." CACHE PATH "DevIL library folder")
  SET(CAIRO_LIB_FOLDER "." CACHE PATH "Cairo library folder")

  find_library(SDL_LIB "SDL2.lib" ${SDL_LIB_FOLDER})
  find_library(GLEW_LIB "glew32.lib" ${GLEW_LIB_FOLDER})
  find_library(DEVIL_LIB "devil.lib" ${DEVIL_LIB_FOLDER})
  find_library(DEVIL_ILU_LIB "ILU.lib" ${DEVIL_LIB_FOLDER})
  find_library(CAIRO_LIB "cairo.lib" ${CAIRO_LIB_FOLDER})

  link_directories(${SDL_LIB} ${GLEW_LIB})

  target_link_libraries(tribalia ${SDL_LIB})
  target_link_libraries(tribalia "opengl32.lib")
  target_link_libraries(tribalia ${GLEW_LIB})
  target_link_libraries(tribalia ${DEVIL_LIB})
  target_link_libraries(tribalia ${DEVIL_ILU_LIB})
  target_link_libraries(tribalia ${CAIRO_LIB})

  if (DO_TESTS)
  	target_link_libraries(tribalia-tests ${SDL_LIB})
  	target_link_libraries(tribalia-tests "opengl32.lib")
  	target_link_libraries(tribalia-tests ${GLEW_LIB})
  	target_link_libraries(tribalia-tests ${DEVIL_LIB})
  	target_link_libraries(tribalia-tests ${DEVIL_ILU_LIB})
  	target_link_libraries(tribalia-tests ${CAIRO_LIB})
  endif(DO_TESTS)

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /GR /W4")

else()
	
  # Some compiler-dependant options
  option(IS_DEBUG "Set if you want to output debugging symbols" OFF)
  
  target_link_libraries(tribalia "SDL2")
  target_link_libraries(tribalia "GL")
  target_link_libraries(tribalia "GLEW")
  target_link_libraries(tribalia "IL")
  target_link_libraries(tribalia "cairo")

  if(DO_TESTS)
  	target_link_libraries(tribalia-tests "SDL2")
  	target_link_libraries(tribalia-tests "GL")
  	target_link_libraries(tribalia-tests "GLEW")
  	target_link_libraries(tribalia-tests "IL")
  	target_link_libraries(tribalia-tests "cairo")
  endif(DO_TESTS)



  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -O2")

  if (IS_DEBUG)
		  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
  endif(IS_DEBUG)

endif(MSVC)
