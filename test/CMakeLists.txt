#
# Unit test compilation routines
#
# Copyright (C) 2017,2021 Arthur M
#
include("${CMAKE_SOURCE_DIR}/cmake/functions.cmake")
enable_testing()

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

set( SRC_TEST_FILES
  "${CMAKE_SOURCE_DIR}/test/test_colony_manager.cpp"
  "${CMAKE_SOURCE_DIR}/test/test_game.cpp"
  "${CMAKE_SOURCE_DIR}/test/test_input_recorder.cpp"
  "${CMAKE_SOURCE_DIR}/test/test_input_reproducer.cpp"
  "${CMAKE_SOURCE_DIR}/test/test_humanplayer.cpp"
  "${CMAKE_SOURCE_DIR}/test/test_command_table.cpp"
  "${CMAKE_SOURCE_DIR}/test/test_model_opener.cpp"
  "${CMAKE_SOURCE_DIR}/test/test_object_attack.cpp"
  "${CMAKE_SOURCE_DIR}/test/test_object_factory.cpp"
  "${CMAKE_SOURCE_DIR}/test/test_object_operations.cpp"
  "${CMAKE_SOURCE_DIR}/test/test_pathfinder.cpp"
  "${CMAKE_SOURCE_DIR}/test/test_pathmanager.cpp"
  "${CMAKE_SOURCE_DIR}/test/test_player_manager.cpp"
  "${CMAKE_SOURCE_DIR}/test/test_scene_manager.cpp"
  "${CMAKE_SOURCE_DIR}/test/test_texture_manager.cpp"
  "${CMAKE_SOURCE_DIR}/test/test_terrain.cpp"
  "${CMAKE_SOURCE_DIR}/test/test_gui_base.cpp"
  "${CMAKE_SOURCE_DIR}/test/tests.cpp"

  "${CMAKE_SOURCE_DIR}/test/utils.cpp"
  "${CMAKE_SOURCE_DIR}/test/utils/test_device.cpp"
  "${CMAKE_SOURCE_DIR}/test/utils/test_texenv.cpp"
  "${CMAKE_SOURCE_DIR}/test/utils/test_inputprocessor.cpp"
  "${CMAKE_SOURCE_DIR}/test/utils/test_gui_renderer.cpp"
  )

file( GLOB SRC_UTIL_FILES "${CMAKE_SOURCE_DIR}/test/utils/test_renderer.cpp" )

message("${SRC_SERVER_TEST}")

find_package(OpenGL REQUIRED)

add_executable(familyline-tests ${SRC_TEST_FILES} ${SRC_UTIL_FILES})
target_link_libraries(familyline-tests PUBLIC familyline-common)
target_link_libraries(familyline-tests PUBLIC familyline-client)

# You need to build YAML as a dll (use -DB)
target_compile_features(familyline-tests PUBLIC cxx_std_20)
target_include_directories(familyline-tests PRIVATE "${CMAKE_SOURCE_DIR}/src/include")

file(COPY "${CMAKE_SOURCE_DIR}/test" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug")

target_link_libraries( familyline-tests
  PRIVATE gtest gtest_main)

add_sanitizers(familyline-tests)
add_coverage(familyline-tests)

target_compile_definitions(familyline-tests PUBLIC
  TESTS_DIR="${CMAKE_SOURCE_DIR}/test"
  )

add_test(NAME general-test COMMAND familyline-tests)
