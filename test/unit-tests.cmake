#
# Unit test compilation routines
# 
# Copyright (C) 2017 Arthur M
#

option(DO_TESTS "Set if you want to enable unit tests, unset if you don't" ON)

if (DO_TESTS)
  enable_testing()
  
  configure_file(${CMAKE_SOURCE_DIR}/test/unit-tests.cmake.in googletest-download/CMakeLists.txt)
  execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )

if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )

if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build)
  
  file( GLOB SRC_TEST_FILES "${CMAKE_SOURCE_DIR}/test/*.cpp" )
  add_executable(tribalia-tests ${SRC_TEST_FILES} ${SRC_LOGIC} ${SRC_GFX} ${SRC_GUI} ${SRC_INPUT} 
	  "${CMAKE_SOURCE_DIR}/src/Log.cpp" "${CMAKE_SOURCE_DIR}/src/Timer.cpp") 
  include_directories("${CMAKE_SOURCE_DIR}/src")
  target_link_libraries( tribalia-tests
    gtest gtest_main)

  add_test(NAME general-test COMMAND tribalia-tests)
endif(DO_TESTS)

