
# Add all files that are exclusive to the client:
# (rendering and input gathering code)

add_library(
  familyline-client
  "params.cpp"
  "game.cpp"
  "player_enumerator.cpp"
  "graphical/asset_file.cpp"
  "graphical/asset_manager.cpp"
  "graphical/asset_object.cpp"
  "graphical/camera.cpp"
  "graphical/deform_animator.cpp"
  "graphical/exceptions.cpp"
  "graphical/framebuffer.cpp"
  "graphical/gfx_debug_drawer.cpp"
  "graphical/gfx_service.cpp"
  "graphical/GraphicalPlotInterface.cpp"
  "graphical/gui/gui_button.cpp"
  "graphical/gui/control.cpp"
  "graphical/gui/root_control.cpp"
  "graphical/gui/gui_container_component.cpp"
  "graphical/gui/gui_imageview.cpp"
  "graphical/gui/gui_label.cpp"
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
  "graphical/shader.cpp"
  "graphical/shader_manager.cpp"
  "graphical/static_animator.cpp"
  "graphical/terrain_renderer.cpp"
  "graphical/Texture.cpp"
  "graphical/TextureFile.cpp"
  "graphical/texture_manager.cpp"
  "graphical/TextureOpener.cpp"
  "graphical/vertexdata.cpp"
  "graphical/opengl/gl_device.cpp"
  "graphical/opengl/gl_window.cpp"
  "graphical/opengl/gl_renderer.cpp"
  "HumanPlayer.cpp"
  "input/Cursor.cpp"
  "input/input_processor.cpp"
  "input/input_manager.cpp"
  "input/InputPicker.cpp"
  "input/input_service.cpp"
  "Timer.cpp"
  )


if (MSVC)

  include(${CMAKE_TOOLCHAIN_FILE})
  find_library(PANGO_LIB pango-1.0)
  find_library(PANGOCAIRO_LIB pangocairo-1.0)
  find_library(GOBJECT_LIB gobject-2.0)
  find_path(PANGO_INCLUDE pango/pango.h)

  target_link_libraries(familyline-client PUBLIC ${PANGO_LIB} ${PANGOCAIRO_LIB} ${GOBJECT_LIB})
  target_include_directories(familyline-client PUBLIC ${PANGO_INCLUDE} ${GLEW_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} ${IL_INCLUDE_DIRS} ${CAIRO_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS} ${YAML_INCLUDE_DIR})

else()
  find_package(PkgConfig REQUIRED)

  pkg_search_module(PANGOCAIRO REQUIRED pangocairo) # finds pango-cairo
  target_include_directories(familyline-client PUBLIC ${PANGOCAIRO_INCLUDE_DIRS})
  
endif(MSVC)

target_include_directories(familyline-client PRIVATE "${CMAKE_SOURCE_DIR}/src/include" )
target_link_libraries(familyline-client PUBLIC familyline-common)
target_compile_features(familyline-client PRIVATE cxx_std_20)

if (WIN32 OR USE_RELATIVE_PATH)
  set(SHADERS_DIR "\"./shaders/\"")
	set(ICONS_DIR "\"./icons/\"")
	set(MATERIALS_DIR "\"./materials/\"")
	set(MODELS_DIR "\"./models/\"")
	set(TEXTURES_DIR "\"./textures/\"")
	set(ASSET_FILE_DIR "\"./\"")
else()
	set(SHADERS_DIR "\"${CMAKE_INSTALL_PREFIX}/share/familyline/shaders/\"")
	set(ICONS_DIR "\"${CMAKE_INSTALL_PREFIX}/share/familyline/icons/\"")
	set(MATERIALS_DIR "\"${CMAKE_INSTALL_PREFIX}/share/familyline/materials/\"")
	set(MODELS_DIR "\"${CMAKE_INSTALL_PREFIX}/share/familyline/models/\"")
	set(TEXTURES_DIR "\"${CMAKE_INSTALL_PREFIX}/share/familyline/textures/\"")
	set(ASSET_FILE_DIR "\"${CMAKE_INSTALL_PREFIX}/share/familyline/\"")
endif(WIN32 OR USE_RELATIVE_PATH)

target_compile_definitions(familyline-client PUBLIC
  SHADERS_DIR=${SHADERS_DIR}
  ICONS_DIR=${ICONS_DIR}
  MATERIALS_DIR=${MATERIALS_DIR}
  MODELS_DIR=${MODELS_DIR}
  TEXTURES_DIR=${TEXTURES_DIR}
  ASSET_FILE_DIR=${ASSET_FILE_DIR}
)
