# Forbid in-source builds.
# More or less inspired by https://github.com/friendlyanon/cmake-init generated
# prelude.

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(
      FATAL_ERROR
      "In-source builds are not supported."
      "You may need to delete 'CMakeCache.txt' and 'CMakeFiles/' first."
  )
endif()


# Export the compile_commands.json so that autocompleters will work.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Detect if we are using vcpkg or not.
# If we are, we will use the vcpkg names, and get all dependencies from there.
# If not, we use pkg-config and cmake's find_package. Remember that you will 
# probably need to pass the required directories by hand.
if (DEFINED VCPKG_TARGET_TRIPLET)
  set(FLINE_USE_VCPKG ON)
else()
  set(FLINE_USE_VCPKG OFF)
endif()

if (WIN32)
  set(FLINE_USE_VCPKG ON)
else()
  set(FLINE_USE_VCPKG OFF)
endif()


# Add our cmake directory, where we have custom Find*** scripts for cmake
# to find some of our dependencies.
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake" )
