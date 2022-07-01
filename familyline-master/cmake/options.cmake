# A list of the custom options for this makefile.

include(CMakeDependentOption)

option(FLINE_USE_RELATIVE_PATH
  "Set if you want the path to the game resources to be relative to the file (OFF), or in a system default location (ON). On Windows, this option makes no difference" ON)

option(FLINE_BUILD_TESTS "Set if you want to enable unit tests, unset if you don't" ON)

option(FLINE_DO_CHECK_ASAN "Enable address sanitizer" ON)
option(FLINE_DO_CHECK_UBSAN "Enable undefined behaviour sanitizer" ON)

cmake_dependent_option(FLINE_SET_COVERAGE "Enable coverage testing. Only enabled when tests are enabled" OFF,
  "FLINE_BUILD_TESTS" OFF)

option(FLINE_NET_SUPPORT "Enable networking support" ON)

set(FLINE_RENDERER "opengl" CACHE STRING "Set if you want to support the opengl renderer. 
Since this is the only renderer, if you disable, you will not be able to render anything")

option(FLINE_USE_GLES  "Set if you want to use OpenGL ES instead of OpenGL. Only has effect if FLINE_RENDERER=opengl" OFF)


# Set the default directory location
set(SHADERS_DIR "\"${CMAKE_INSTALL_PREFIX}/share/familyline/shaders/\"")
set(ICONS_DIR "\"${CMAKE_INSTALL_PREFIX}/share/familyline/icons/\"")
set(MATERIALS_DIR "\"${CMAKE_INSTALL_PREFIX}/share/familyline/materials/\"")
set(MODELS_DIR "\"${CMAKE_INSTALL_PREFIX}/share/familyline/models/\"")
set(TEXTURES_DIR "\"${CMAKE_INSTALL_PREFIX}/share/familyline/textures/\"")
set(ASSET_FILE_DIR "\"${CMAKE_INSTALL_PREFIX}/share/familyline/\"")
set(SCRIPTS_DIR "\"${CMAKE_INSTALL_PREFIX}/share/familyline/scripts/\"")

if (WIN32 OR FLINE_USE_RELATIVE_PATH)
  set(SHADERS_DIR "\"./shaders/\"")
  set(ICONS_DIR "\"./icons/\"")
  set(MATERIALS_DIR "\"./materials/\"")
  set(MODELS_DIR "\"./models/\"")
  set(TEXTURES_DIR "\"./textures/\"")
  set(ASSET_FILE_DIR "\"./\"")
  set(SCRIPTS_DIR "\"./scripts/\"")
endif()




