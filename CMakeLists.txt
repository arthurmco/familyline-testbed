####
# Familyline makelist
#
# Copyright (C) 2016-2020 Arthur M
####
cmake_minimum_required(VERSION 3.12)

project(Familyline VERSION 0.2.1)

option(USE_RELATIVE_PATH "Set if you want the path to the game resources to be local" OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)



#Familyline game data
add_subdirectory(src/common)
add_subdirectory(src/client)

add_executable(familyline "src/client/Tribalia.cpp")
target_link_libraries(familyline familyline-client)

target_compile_features(familyline PUBLIC cxx_std_20)

include("test/unit-tests.cmake")

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake" )

# Add packages natively supported by cmake
find_package(GLEW REQUIRED)
find_package(DevIL REQUIRED)
find_package(Yaml REQUIRED)
find_package(OpenGL REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(AFTER ${GLEW_INCLUDE_DIRS} ${IL_INCLUDE_DIR} ${yaml_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR})
target_include_directories(familyline PRIVATE "${CMAKE_SOURCE_DIR}/src/include")

target_link_libraries(familyline ${OPENGL_LIBRARIES})
target_link_libraries(familyline ${YAML_LIBRARIES}) # You need to build YAML as a dll (use -DBUILD_SHARED_LIBS)
target_link_libraries(familyline ${ZLIB_LIBRARIES})

if (MSVC)
	
  include(${CMAKE_TOOLCHAIN_FILE})

  # Set include directories
  find_package(GLM CONFIG REQUIRED)
  find_package(unofficial-cairo CONFIG REQUIRED)

  # SDL, GLM and cairo includes will be made available via vcpkg and put in a default path.
  # We don't really need to put them here'
  include_directories(AFTER ${YAML_INCLUDE_DIR})

  target_link_libraries(familyline SDL2::SDL2 SDL2::SDL2main)
  target_link_libraries(familyline GLEW::GLEW)
  target_link_libraries(familyline ${IL_LIBRARIES})
  target_link_libraries(familyline ${ILU_LIBRARIES})
  target_link_libraries(familyline unofficial::cairo::cairo)
  target_link_libraries(familyline glm)
  target_link_libraries(familyline wsock32 ws2_32)
  
#  target_link_libraries(familyline-server wsock32 ws2_32)

  #if (DO_TESTS)
	target_link_libraries(familyline-tests SDL2::SDL2 SDL2::SDL2main)
  	target_link_libraries(familyline-tests ${OPENGL_gl_LIBRARY})
  	target_link_libraries(familyline-tests ${GLEW_LIBRARY})
  	target_link_libraries(familyline-tests ${IL_LIBRARIES})
  	target_link_libraries(familyline-tests ${ILU_LIBRARIES})
	target_link_libraries(familyline-tests unofficial::cairo::cairo)
	target_link_libraries(familyline-tests glm)
	target_link_libraries(familyline-tests wsock32 ws2_32)
	target_link_libraries(familyline-tests ${YAML_LIBRARIES})
    target_link_libraries(familyline-tests ${ZLIB_LIBRARIES})
  #endif(DO_TESTS)

  add_compile_options(/EHsc /GR /W4)

else()
	
  find_package(PkgConfig REQUIRED)
  find_package(Threads REQUIRED)
  pkg_search_module(CAIRO REQUIRED cairo) # finds cairo
  pkg_search_module(PANGOCAIRO REQUIRED pangocairo) # finds pango-cairo
  pkg_search_module(GLM REQUIRED glm) # finds glm
  
  
  if (TARGET fmt::fmt)
	target_link_libraries(familyline fmt::fmt)
  	if(DO_TESTS)
		target_link_libraries(familyline-tests fmt::fmt)
	endif(DO_TESTS)
  else()
	message(WARNING "libfmt was not found, maybe we have it locally, like on travis")
	add_subdirectory(fmt)

  	target_link_libraries(familyline fmt::fmt)
  	if(DO_TESTS)
		target_link_libraries(familyline-tests fmt::fmt)
	endif(DO_TESTS)
  endif(TARGET fmt::fmt)
  
  target_link_libraries(familyline ${SDL2_LIBRARIES})
  target_link_libraries(familyline GLEW::GLEW)
  target_link_libraries(familyline ${IL_LIBRARIES})
  target_link_libraries(familyline ${ILU_LIBRARIES})
  target_link_libraries(familyline ${CAIRO_LINK_LIBRARIES})
  target_link_libraries(familyline ${PANGOCAIRO_LINK_LIBRARIES})
  target_link_libraries(familyline ${GLM_LINK_LIBRARIES})
  target_link_libraries(familyline Threads::Threads)
  
  if(DO_TESTS)
  	target_link_libraries(familyline-tests ${SDL2_LIBRARIES})
  	target_link_libraries(familyline-tests GLEW::GLEW)
  	target_link_libraries(familyline-tests ${IL_LIBRARIES})
  	target_link_libraries(familyline-tests ${ILU_LIBRARIES})
  	target_link_libraries(familyline-tests ${CAIRO_LIBRARIES})
    target_link_libraries(familyline-tests ${PANGOCAIRO_LINK_LIBRARIES})
	target_link_libraries(familyline-tests ${GLM_LIBRARIES})
	target_link_libraries(familyline-tests ${YAML_LIBRARIES})
    target_link_libraries(familyline-tests Threads::Threads)    
    target_link_libraries(familyline-tests ${ZLIB_LIBRARIES})
  endif(DO_TESTS)

  # if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  #  message("clang detected, linking with libc++")
  #  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  #endif()


  add_compile_options(${SDL2_CFLAGS_OTHER} -Wall -Wextra)

  include_directories(AFTER  ${GLEW_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} `${OPENGL_INCLUDE_DIR} ${IL_INCLUDE_DIRS} ${PANGOCAIRO_INCLUDE_DIRS} ${CAIRO_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS} ${YAML_INCLUDE_DIR})

endif(MSVC)

if (WIN32)
	
  get_filename_component(CMAKE_TOOLCHAIN_DIR "${CMAKE_TOOLCHAIN_FILE}" DIRECTORY)

  # Copy a stubborn dependent DLLs to the release directory, so the software can be run and packaged.
  # See https://github.com/Microsoft/vcpkg/issues/2630
  #file(COPY "${CMAKE_TOOLCHAIN_DIR}/../../installed/x86-windows/bin/lcms2.dll" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
  #file(COPY "${CMAKE_TOOLCHAIN_DIR}/../../installed/x86-windows/bin/lcms2.dll" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")


endif (WIN32)

# Custom targets
add_custom_target(docs COMMAND "doxygen" "Doxyfile"
  COMMENT "Creating the Doxygen documentation"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")


# CPack configuration

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "crappy but fun RTS game, where you guide a family in its fantastic adventures through the Ancient Era")
SET(CPACK_PACKAGE_VENDOR "Arthur Mendes")
SET(CPACK_PACKAGE_CONTACT "Arthur Mendes <arthurmco@gmail.com>")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "98")
SET(CPACK_SOURCE_IGNORE_FILES "googletest-*/*;test/*.*;test/googletest/*")
SET(CPACK_GENERATOR "DEB;RPM")
SET(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libglew2.0, libsdl2-2.0-0, libdevil1c2, libcairo2, libyaml-0-2")
SET(CPACK_DEBIAN_PACKAGE_SECTION "Games")
SET(CPACK_RPM_PACKAGE_REQUIRES "glew, SDL2, DevIL, cairo, libyaml")
SET(CPACK_RPM_PACKAGE_LICENSE "MIT")
  
INCLUDE(CPack)

# Copy the asset folders to the build folder
# This means that we have to make a separate build directory.
# Usually this is the case.
file(COPY "${CMAKE_SOURCE_DIR}/shaders" DESTINATION "${PROJECT_BINARY_DIR}")
file(COPY "${CMAKE_SOURCE_DIR}/icons" DESTINATION "${PROJECT_BINARY_DIR}")
file(COPY "${CMAKE_SOURCE_DIR}/materials" DESTINATION "${PROJECT_BINARY_DIR}")
file(COPY "${CMAKE_SOURCE_DIR}/models" DESTINATION "${PROJECT_BINARY_DIR}")
file(COPY "${CMAKE_SOURCE_DIR}/textures" DESTINATION "${PROJECT_BINARY_DIR}")
file(COPY "${CMAKE_SOURCE_DIR}/terrain_test.flte" DESTINATION "${PROJECT_BINARY_DIR}")
file(COPY "${CMAKE_SOURCE_DIR}/assets.yml" DESTINATION "${PROJECT_BINARY_DIR}")


# Install locations
install(TARGETS familyline DESTINATION bin)
install(DIRECTORY shaders DESTINATION share/familyline)
install(DIRECTORY textures DESTINATION share/familyline)
install(DIRECTORY icons DESTINATION share/familyline)
install(DIRECTORY materials DESTINATION share/familyline)
install(DIRECTORY models DESTINATION share/familyline)
install(FILES assets.yml DESTINATION share/familyline)
install(FILES terrain_test.flte DESTINATION share/familyline)
