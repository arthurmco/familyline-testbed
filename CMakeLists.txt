####
# Tribalia makelist
#
# Copyright (C) 2016 Arthur M
####

cmake_minimum_required(VERSION 3.1)

project(Tribalia)

SET(GEN_HEADER "${CMAKE_SOURCE_DIR}/src/EnviroDefs.h")
configure_file("${CMAKE_SOURCE_DIR}/src/EnviroDefs.h.in" "${CMAKE_SOURCE_DIR}/src/EnviroDefs.h" @ONLY)

SET(T_VERSION "0.1.92")

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
file(GLOB_RECURSE SRC_NET "${CMAKE_SOURCE_DIR}/src/net/*.cpp")
file(GLOB_RECURSE SRC_OBJECTS "${CMAKE_SOURCE_DIR}/src/objects/*.cpp")
file(GLOB SRC_BASE "${CMAKE_SOURCE_DIR}/src/*.cpp")

# Tribalia common files
file(GLOB SRC_COMMON "${CMAKE_SOURCE_DIR}/common/*.cpp")

# Tribalia server data
file(GLOB_RECURSE SRC_SERVER "${CMAKE_SOURCE_DIR}/server/*.cpp")

#Compilation data
add_executable(tribalia ${SRC_BASE} ${SRC_LOGIC} ${SRC_GFX} ${SRC_GUI} ${SRC_INPUT} ${SRC_NET} ${SRC_OBJECTS} ${SRC_COMMON})
add_executable(tribalia-server ${SRC_SERVER} ${SRC_COMMON})

include("test/unit-tests.cmake")

# Add packages natively supported by cmake
find_package(GLEW REQUIRED)
find_package(DevIL REQUIRED)

include_directories(AFTER ${GLEW_INCLUDE_DIR} ${IL_INCLUDE_DIR} ${yaml_INCLUDE_DIR})

# Include the common directory
include_directories(AFTER "${CMAKE_SOURCE_DIR}/common")

if (MSVC)

  # Set include directories
  SET(SDL_INCLUDE "." CACHE PATH "SDL2 include directory")
  SET(GLM_INCLUDE "." CACHE PATH "GLM include directory")
  SET(CAIRO_INCLUDE "." CACHE PATH "Cairo include directory")

  include_directories(AFTER ${SDL_INCLUDE} ${GLM_INCLUDE} ${CAIRO_INCLUDE})

  #Set library directories
  SET(SDL_LIB_FOLDER "." CACHE PATH "SDL2 library directory")
  SET(CAIRO_LIB_FOLDER "." CACHE PATH "Cairo library folder")
  
  find_library(SDL_LIB "SDL2.lib" ${SDL_LIB_FOLDER})
  find_library(CAIRO_LIB "cairo.lib" ${CAIRO_LIB_FOLDER})

  link_directories(${SDL_LIB} ${GLEW_LIB})
  
  target_link_libraries(tribalia ${SDL_LIB})
  target_link_libraries(tribalia "opengl32.lib")
  target_link_libraries(tribalia ${GLEW_LIBRARY})
  target_link_libraries(tribalia ${IL_LIBRARIES})
  target_link_libraries(tribalia ${ILU_LIBRARIES})
  target_link_libraries(tribalia ${CAIRO_LIB})
  target_link_libraries(tribalia wsock32 ws2_32)

  target_link_libraries(tribalia-server wsock32 ws2_32)

  #if (DO_TESTS)
  	target_link_libraries(tribalia-tests ${SDL_LIB})
  	target_link_libraries(tribalia-tests "opengl32.lib")
  	target_link_libraries(tribalia-tests ${GLEW_LIBRARY})
  	target_link_libraries(tribalia-tests ${IL_LIBRARIES})
  	target_link_libraries(tribalia-tests ${ILU_LIBRARIES})
  	target_link_libraries(tribalia-tests ${CAIRO_LIB})
	target_link_libraries(tribalia-tests wsock32 ws2_32)
  #endif(DO_TESTS)

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /GR /W4 /std:c++latest")

else()
	
  # Some compiler-dependant options
  option(IS_DEBUG "Set if you want to output debugging symbols" OFF)

  find_package(PkgConfig REQUIRED)
  pkg_search_module(SDL2 REQUIRED sdl2) # finds sdl
  pkg_search_module(CAIRO REQUIRED cairo) # finds cairo
  pkg_search_module(GLM REQUIRED glm) # finds glm
  
  
  
  target_link_libraries(tribalia ${SDL2_LIBRARIES})
  target_link_libraries(tribalia "GL")
  target_link_libraries(tribalia ${GLEW_LIBRARY})
  target_link_libraries(tribalia ${IL_LIBRARIES})
  target_link_libraries(tribalia ${ILU_LIBRARIES})
  target_link_libraries(tribalia ${CAIRO_LIBRARIES})
  target_link_libraries(tribalia ${GLM_LIBRARIES})
  target_link_libraries(tribalia "yaml")

  
  if(DO_TESTS)
  	target_link_libraries(tribalia-tests ${SDL2_LIBRARIES})
  	target_link_libraries(tribalia-tests "GL")
  	target_link_libraries(tribalia-tests ${GLEW_LIBRARY})
  	target_link_libraries(tribalia-tests ${IL_LIBRARIES})
  	target_link_libraries(tribalia-tests ${ILU_LIBRARIES})
  	target_link_libraries(tribalia-tests ${CAIRO_LIBRARIES})
	target_link_libraries(tribalia-tests ${GLM_LIBRARIES})
	target_link_libraries(tribalia-tests "yaml")
  endif(DO_TESTS)

  # if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  #  message("clang detected, linking with libc++")
  #  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  #endif()


  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SDL2_CFLAGS_OTHER}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall -Wextra")
  include_directories(AFTER ${SDL2_INCLUDE_DIRS} ${IL_INCLUDE_DIRS} ${CAIRO_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS})

  if (IS_DEBUG)
	  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
  endif(IS_DEBUG)

endif(MSVC)

if (WIN32)

  # Copy the DLLs to the release directory, so the software can be run and packaged.
  file(GLOB WIN32_DEPS "${CMAKE_SOURCE_DIR}/distribute/win32/*.dll")
  file(COPY ${WIN32_DEPS} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug")

  # Copy the asset folders to the Debug folder
  file(COPY "${CMAKE_SOURCE_DIR}/shaders" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug")
  file(COPY "${CMAKE_SOURCE_DIR}/icons" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug")
  file(COPY "${CMAKE_SOURCE_DIR}/materials" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug")
  file(COPY "${CMAKE_SOURCE_DIR}/models" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug")
  file(COPY "${CMAKE_SOURCE_DIR}/textures" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug")
  file(COPY "${CMAKE_SOURCE_DIR}/terrain_test.trtb" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug")
  file(COPY "${CMAKE_SOURCE_DIR}/assets.taif" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug")

endif (WIN32)


option(USE_RELATIVE_PATH "Set if you want the path to the game resources to be local" OFF)

if (WIN32 OR USE_RELATIVE_PATH)
	set(SHADERS_DIR "\"./shaders/\"")
	set(ICONS_DIR "\"./icons/\"")
	set(MATERIALS_DIR "\"./materials/\"")
	set(MODELS_DIR "\"./models/\"")
	set(TEXTURES_DIR "\"./textures/\"")
	set(ASSET_FILE_DIR "\"./\"")
else()
	set(SHADERS_DIR "\"${CMAKE_INSTALL_PREFIX}/share/tribalia/shaders/\"")
	set(ICONS_DIR "\"${CMAKE_INSTALL_PREFIX}/share/tribalia/icons/\"")
	set(MATERIALS_DIR "\"${CMAKE_INSTALL_PREFIX}/share/tribalia/materials/\"")
	set(MODELS_DIR "\"${CMAKE_INSTALL_PREFIX}/share/tribalia/models/\"")
	set(TEXTURES_DIR "\"${CMAKE_INSTALL_PREFIX}/share/tribalia/textures/\"")
	set(ASSET_FILE_DIR "\"${CMAKE_INSTALL_PREFIX}/share/tribalia/\"")
endif(WIN32 OR USE_RELATIVE_PATH)

configure_file("${CMAKE_SOURCE_DIR}/src/config.h.in" "${CMAKE_SOURCE_DIR}/src/config.h")



# Install locations
install(TARGETS tribalia DESTINATION bin)
install(DIRECTORY shaders DESTINATION share/tribalia)
install(DIRECTORY textures DESTINATION share/tribalia)
install(DIRECTORY icons DESTINATION share/tribalia)
install(DIRECTORY materials DESTINATION share/tribalia)
install(DIRECTORY models DESTINATION share/tribalia)
install(FILES assets.yml DESTINATION share/tribalia)
install(FILES terrain_test.trtb DESTINATION share/tribalia)

