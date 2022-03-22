# CMakeLists for the common part of the engine, the code
# that is shared between client and server.
include("${CMAKE_SOURCE_DIR}/cmake/functions.cmake")

include("${CMAKE_SOURCE_DIR}/src/common/generated.cmake")

include(FetchContent)

get_target_property(INPUT_FLATBUFFER_INCLUDE input-flatbuffer GENERATED_INCLUDES_DIR)
get_target_property(INPUT_FLATBUFFER_INCLUDE input-ser-flatbuffer GENERATED_INCLUDES_DIR)
get_target_property(INPUT_FLATBUFFER_INCLUDE network-flatbuffer GENERATED_INCLUDES_DIR)

add_library(
  familyline-common
  "logger.cpp"
  "logic/action_queue.cpp"
  "logic/attack_manager.cpp"
  "logic/BuildQueue.cpp"
  "logic/colony.cpp"
  "logic/colony_manager.cpp"
  "logic/debug_drawer.cpp"
  "logic/game_event.cpp"
  "logic/game_object.cpp"
  "logic/input_recorder.cpp"
  "logic/input_reproducer.cpp"
  "logic/lifecycle_manager.cpp"
  "logic/logic_service.cpp"
  "logic/script_environment.cpp"
  "logic/object_components.cpp"
  "logic/object_factory.cpp"
  "logic/object_listener.cpp"
  "logic/object_manager.cpp"
  "logic/object_path_manager.cpp"
  "logic/pathfinder.cpp"
  "logic/player.cpp"
  "logic/replay_player.cpp"
  "logic/player_manager.cpp"
  "logic/terrain.cpp"
  "logic/terrain_file.cpp"
  "objects/Tent.cpp"
  "objects/WatchTower.cpp"
  "net/server_finder.cpp"
  "net/server.cpp"
  "net/game_packet_server.cpp"
  "net/network_client.cpp"
  "net/net_player_sender.cpp"
  "net/network_player.cpp"
  )

add_dependencies(familyline-common input-flatbuffer input-ser-flatbuffer network-flatbuffer)

target_compile_features(familyline-common PRIVATE cxx_std_20)

find_package(fmt 6...7.2 CONFIG REQUIRED)
find_package(tl-expected 1 CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(FlatBuffers REQUIRED)
find_package(SDL2 CONFIG)
find_package(range-v3 CONFIG)
find_package(glm CONFIG)

message("Downloading s7 scheme...")
FetchContent_Declare(
  s7
  URL "https://arthurmco.github.io/externals/s7.tar.gz"
  URL_HASH "SHA256=749ff831fbc5a43029952a8436ec492a659dc207c298b746c65cd1735fd08c28"
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/generated/s7"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  )
FetchContent_MakeAvailable(s7)
target_include_directories(familyline-common PUBLIC ${s7_SOURCE_DIR})
set_source_files_properties("${s7_SOURCE_DIR}/s7.c" PROPERTIES LANGUAGE CXX)
add_library(
  s7-lib "${s7_SOURCE_DIR}/s7.c")
target_link_libraries(familyline-common PUBLIC s7-lib)  

if (FLINE_USE_VCPKG)	
  include(${CMAKE_TOOLCHAIN_FILE})

  if (FLINE_NET_SUPPORT)
  find_package(unofficial-curlpp CONFIG REQUIRED)
  target_link_libraries(familyline-common PUBLIC unofficial::curlpp::curlpp)
  endif()

  target_include_directories(familyline-common PUBLIC
    ${YAML_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR}
    ${INPUT_FLATBUFFER_INCLUDE})
  target_link_libraries(familyline-common PUBLIC fmt::fmt glm::glm ${ZLIB_LIBRARIES})
  target_link_libraries(familyline-common PUBLIC nlohmann_json::nlohmann_json)
  target_link_libraries(familyline-common PUBLIC tl::expected range-v3::range-v3)

else()
  find_package(PkgConfig REQUIRED)

  if (FLINE_NET_SUPPORT)
  pkg_search_module(CURLPP REQUIRED curlpp)
  target_link_libraries(familyline-common PUBLIC ${CURLPP_LDFLAGS})
  target_include_directories(familyline-common PUBLIC ${CURLPP_INCLUDE_DIRS})
  endif()
  pkg_search_module(ZLIB REQUIRED zlib)

  target_link_libraries(familyline-common PUBLIC ${glm_LIBRARY})

  target_link_libraries(s7-lib PUBLIC "-ldl")  

  target_link_libraries(familyline-common PUBLIC fmt::fmt ${ZLIB_LIBRARIES})
  
  target_link_libraries(familyline-common PUBLIC tl::expected range-v3::range-v3)
  target_link_libraries(familyline-common PUBLIC ${CURLPP_LDFLAGS})
  target_link_libraries(familyline-common PUBLIC nlohmann_json::nlohmann_json)
  target_include_directories(familyline-common PUBLIC ${ZLIB_INCLUDE_DIR}
    ${INPUT_FLATBUFFER_INCLUDE} ${CURLPP_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})
endif(FLINE_USE_VCPKG)

add_sanitizers(familyline-common)
add_coverage(familyline-common)

target_include_directories(familyline-common PUBLIC "${CMAKE_SOURCE_DIR}/src/include"
   "${CMAKE_SOURCE_DIR}/generated")
fill_build_information(familyline-common)
