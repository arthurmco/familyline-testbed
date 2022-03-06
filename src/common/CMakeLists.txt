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

message("${CMAKE_CURRENT_BINARY_DIR}")

if (FLINE_USE_VCPKG)	
  include(${CMAKE_TOOLCHAIN_FILE})

  if (FLINE_NET_SUPPORT)
  find_package(unofficial-curlpp CONFIG REQUIRED)
  target_link_libraries(familyline-common PUBLIC unofficial::curlpp::curlpp)
  endif()

  message("GNU Guile is not present on vcpkg and, currently, we have no way of integrating it...")

  message("Downloading a prebuilt GNU Guile (this may not work)")
  FetchContent_Declare(
    guile
    URL "https://downloads.sourceforge.net/project/ezwinports/guile-2.0.11-2-w32-bin.zip"
    URL_HASH "SHA256=01c231826e01f640b01516657b133c84075d3e0316fba46a15d5bd97de166d0e"
    SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin/guile"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    )
  FetchContent_MakeAvailable(guile)

  target_include_directories(familyline-common PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}/bin/guile/include/guile/2.0")
  target_link_libraries(familyline-common PUBLIC fmt::fmt glm::glm
    "${CMAKE_CURRENT_BINARY_DIR}/bin/guile/lib/libguile-2.0.la")

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
  pkg_search_module(guile REQUIRED guile-2.2)

  target_link_libraries(familyline-common PUBLIC ${glm_LIBRARY})
  target_link_libraries(familyline-common PUBLIC fmt::fmt glm::glm ${ZLIB_LIBRARIES})
  
  target_link_libraries(familyline-common PUBLIC tl::expected range-v3::range-v3)
  target_link_libraries(familyline-common PUBLIC ${CURLPP_LDFLAGS} ${guile_LDFLAGS})
  target_link_libraries(familyline-common PUBLIC nlohmann_json::nlohmann_json)
  target_include_directories(familyline-common PUBLIC ${ZLIB_INCLUDE_DIR}
    ${INPUT_FLATBUFFER_INCLUDE} ${CURLPP_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS}
    ${guile_INCLUDE_DIRS})
endif(FLINE_USE_VCPKG)

add_sanitizers(familyline-common)
add_coverage(familyline-common)

target_include_directories(familyline-common PUBLIC "${CMAKE_SOURCE_DIR}/src/include"
   "${CMAKE_SOURCE_DIR}/generated")
fill_build_information(familyline-common)
