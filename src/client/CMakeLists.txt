# CMakeLists for the client part of the engine, the code
# that is shared between client and server.
include("${CMAKE_SOURCE_DIR}/cmake/functions.cmake")

execute_process(COMMAND python3 "${CMAKE_SOURCE_DIR}/tools/gen_keyset.py"
  OUTPUT_VARIABLE keyset-out)
file(WRITE "${CMAKE_SOURCE_DIR}/generated/keyset_gen.h" "${keyset-out}")


add_library(
  familyline-client
  "params.cpp"
  "config_reader.cpp"
  "game.cpp"
  "preview_renderer.cpp"
  "player_enumerator.cpp"
  "graphical/asset_file.cpp"
  "graphical/asset_manager.cpp"
  "graphical/asset_object.cpp"
  "graphical/camera.cpp"
  "graphical/deform_animator.cpp"
  "graphical/exceptions.cpp"
  "graphical/gfx_debug_drawer.cpp"
  "graphical/gfx_service.cpp"
  "graphical/GraphicalPlotInterface.cpp"
  "graphical/gui/theme.cpp"
  "graphical/gui/gui_button.cpp"
  "graphical/gui/gui_control.cpp"
  "graphical/gui/gui_box.cpp"
  "graphical/gui/gui_imageview.cpp"
  "graphical/gui/gui_label.cpp"
#  "graphical/gui/gui_checkbox.cpp"
#  "graphical/gui/gui_listbox.cpp"
  "graphical/gui/gui_textbox.cpp"
  "graphical/gui/gui_manager.cpp"
  "graphical/gui/gui_window.cpp"
  "graphical/material.cpp"
  "graphical/material_manager.cpp"
  "graphical/materialopener/MTLOpener.cpp"
  "graphical/mesh.cpp"
  "graphical/meshopener/MD2Opener.cpp"
  "graphical/meshopener/MeshOpener.cpp"
  "graphical/meshopener/OBJOpener.cpp"
  "graphical/object_renderer.cpp"
  "graphical/scene_manager.cpp"
  "graphical/shader_manager.cpp"
  "graphical/static_animator.cpp"
  "graphical/texture_environment.cpp"
  "graphical/texture_manager.cpp"
  "graphical/vertexdata.cpp"
  "graphical/opengl/gl_device.cpp"
  "graphical/opengl/gl_window.cpp"
  "graphical/opengl/gl_renderer.cpp"
  "graphical/opengl/gl_terrain_renderer.cpp"
  "graphical/opengl/gl_framebuffer.cpp"
  "graphical/opengl/gl_texture_environment.cpp"
  "graphical/opengl/gl_gui_renderer.cpp"
  "graphical/opengl/gl_shader.cpp"
  "graphical/opengl/gles_utils.cpp"
  "HumanPlayer.cpp"
  "input/Cursor.cpp"
  "input/command_table.cpp"
  "input/input_processor.cpp"
  "input/input_manager.cpp"
  "input/InputPicker.cpp"
  "input/input_service.cpp"
  "Timer.cpp"
  )

target_compile_features(familyline-client PRIVATE cxx_std_20)

find_package(FlatBuffers REQUIRED)
add_dependencies(familyline-client familyline-common input-flatbuffer input-ser-flatbuffer network-flatbuffer)

find_package(fmt 6...7.2 CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(tl-expected 1 CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(familyline-client PUBLIC GLEW::GLEW)
target_link_libraries(familyline-client PUBLIC ${OPENGL_LIBRARIES})

if (FLINE_USE_VCPKG)
  include(${CMAKE_TOOLCHAIN_FILE})
  find_library(PANGO_LIB pango-1.0 REQUIRED)
  find_library(PANGOCAIRO_LIB pangocairo-1.0)
  find_library(GOBJECT_LIB gobject-2.0)
  find_library(CAIRO_LIBRARY cairo REQUIRED)
  find_package(sdl2-image CONFIG REQUIRED)

  find_path(PANGO_INCLUDE NAMES pango/pango.h REQUIRED PATH_SUFFIXES "pango-1.0")
  find_path(HB_INCLUDE NAMES hb.h REQUIRED PATH_SUFFIXES "harfbuzz")
  find_path(PANGOCAIRO_INCLUDE_DIR NAMES "pango/pangocairo.h" REQUIRED PATH_SUFFIXES "pango-1.0")
  find_path(GLIB_INCLUDES NAMES glib-object.h REQUIRED
    PATH_SUFFIXES "glib-2.0")
  find_path(GLIB_CONFIG_INCLUDES NAMES glibconfig.h REQUIRED
    PATH_SUFFIXES "lib/glib-2.0/include")
  find_path(CAIRO_INCLUDE_DIRS cairo.h REQUIRED PATH_SUFFIXES "cairo")
  target_link_libraries(familyline-client PUBLIC ${PANGO_LIB} ${PANGOCAIRO_LIB} ${GOBJECT_LIB}
    ${CAIRO_LIBRARY} yaml-cpp)

  target_link_libraries(familyline-client PUBLIC SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_image)

  target_include_directories(familyline-client PUBLIC
    ${GLIB_INCLUDES} ${HB_INCLUDE} ${GLIB_CONFIG_INCLUDES} ${PANGO_INCLUDE}
    ${GLEW_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR}
    ${CAIRO_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS} ${PANGOCAIRO_INCLUDE_DIR}
    ${yaml_INCLUDE_DIR})

else()
  find_package(PkgConfig REQUIRED)
  find_package(Threads REQUIRED)
#  find_package(SDL_image REQUIRED)
  pkg_search_module(CAIRO REQUIRED cairo) # finds cairo
  pkg_search_module(PANGOCAIRO REQUIRED pangocairo) # finds pango-cairo
  pkg_search_module(GLM REQUIRED glm) # finds glm

  if (DEFINED SDL2::SDL2)
    target_link_libraries(familyline-client PUBLIC SDL2::SDL2 SDL2::SDL2main "-lSDL2_image")
  else()
    target_link_libraries(familyline-client PUBLIC ${SDL2_LIBRARIES} "-lSDL2_image")
  endif()

  target_link_libraries(familyline-client PUBLIC fmt::fmt)

  target_link_libraries(familyline-client PUBLIC GLEW::GLEW)
  target_link_libraries(familyline-client PUBLIC ${CAIRO_LINK_LIBRARIES})
  target_link_libraries(familyline-client PUBLIC ${PANGOCAIRO_LINK_LIBRARIES})
  target_link_libraries(familyline-client PUBLIC ${GLM_LINK_LIBRARIES})
  target_link_libraries(familyline-client PUBLIC ${YAML_CPP_LIBRARIES})
  target_link_libraries(familyline-client PUBLIC Threads::Threads)
  target_include_directories(familyline-client PUBLIC ${GLEW_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR} ${PANGOCAIRO_INCLUDE_DIRS} ${CAIRO_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS} ${YAML_INCLUDE_DIR})

endif(FLINE_USE_VCPKG)

add_sanitizers(familyline-client)
add_coverage(familyline-client)

fill_build_information(familyline-client)

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/../../src/include/client/config.h.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/../../src/include/client/config.h" )

target_link_libraries(familyline-client PUBLIC familyline-common)

target_include_directories(familyline-client PUBLIC "${CMAKE_SOURCE_DIR}/src/include"
  "${CMAKE_SOURCE_DIR}/generated")
