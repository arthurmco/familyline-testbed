####
# The default cmakefile of this game.
#
# Copyright (C) 2016-2021 Arthur M
####

cmake_minimum_required(VERSION 3.14)
include(cmake/prelude.cmake)

project(
  familyline
  VERSION 0.3.1
  DESCRIPTION "a RTS game where you follow the story of a fictional family throughout the ages"
  HOMEPAGE_URL "https://github.com/arthurmco/familyline"
  LANGUAGES CXX
  )

include(cmake/options.cmake)

message(STATUS "Renderer: ${FLINE_RENDERER} ")

if (${FLINE_RENDERER} EQUAL "opengl")
  message(STATUS "OpenGL options:")
  message(STATUS "\tUse GLES: ${FLINE_USE_GLES} ")
endif()

if (FLINE_USE_VCPKG)
  message(STATUS "Using **vcpkg repository** to get the dependencies")
else()
  message(STATUS "Using **the system libraries** to get the dependencies")
endif()

if (FLINE_USE_RELATIVE_PATH)
  message(STATUS "The resources are in the same directory as the executable")
else()
  message(STATUS "The resources respect the unix convention for paths")
endif()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/common")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/client")

if (FLINE_BUILD_TESTS)
  message(STATUS "Tests: ENABLED")
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif()

add_executable(familyline "src/client/familyline.cpp" "src/client/familyline.exe.manifest"
  "src/game.rc")
target_compile_features(familyline PUBLIC cxx_std_20)
target_include_directories(familyline PUBLIC "${CMAKE_SOURCE_DIR}/src/include" "${CMAKE_SOURCE_DIR}/generated")


add_sanitizers(familyline)

# Do not add code coverage building to the main executable, because
# we want to add it to the test suite only
target_link_libraries(familyline PUBLIC familyline-client)

copy_files_to_build("${CMAKE_BINARY_DIR}")

# Install locations
install(TARGETS familyline DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY shaders DESTINATION share/familyline)
install(DIRECTORY textures DESTINATION share/familyline)
install(DIRECTORY icons DESTINATION share/familyline)
install(DIRECTORY materials DESTINATION share/familyline)
install(DIRECTORY models DESTINATION share/familyline)
install(DIRECTORY scripts DESTINATION share/familyline)
install(FILES assets.yml DESTINATION share/familyline)
install(FILES theme.yml DESTINATION share/familyline)
install(FILES terrain_test.flte DESTINATION share/familyline)

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/src/include/config.h.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/include/config.h" )
