
# Add all files that are common to the server and client:
# (logic, logging, net and object code)

add_library(
  familyline-common
  "logger.cpp"
  "logic/action_queue.cpp"
  "logic/attack_manager.cpp"
  "logic/BuildQueue.cpp"
  "logic/colony.cpp"
  "logic/colony_manager.cpp"
  "logic/debug_drawer.cpp"
  "logic/GameActionManager.cpp"
  "logic/game_event.cpp"
  "logic/game_object.cpp"
  "logic/lifecycle_manager.cpp"
  "logic/logic_service.cpp"
  "logic/object_components.cpp"
  "logic/object_factory.cpp"
  "logic/object_listener.cpp"
  "logic/object_manager.cpp"
  "logic/ObjectPathManager.cpp"
  "logic/PathFinder.cpp"
  "logic/player.cpp"
  "logic/player_manager.cpp"
  "logic/terrain.cpp"
  "logic/terrain_file.cpp"
  "NetMessageQueue.cpp"
  "net/NetPlayerFilter.cpp"
  "net/NetPlayerManager.cpp"
  "net/NetServer.cpp"
  "objects/Tent.cpp"
  "objects/WatchTower.cpp"
  )

 
find_package(fmt CONFIG REQUIRED)
find_package(ZLIB REQUIRED)

if (MSVC)
	
  include(${CMAKE_TOOLCHAIN_FILE})
  find_package(GLM CONFIG REQUIRED)
  target_include_directories(familyline-common PUBLIC ${GLM_INCLUDE_DIRS} ${YAML_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR})
  target_link_libraries(familyline-common PUBLIC fmt::fmt glm ${ZLIB_LIBRARIES})

else()
  find_package(PkgConfig REQUIRED)
  pkg_search_module(GLM REQUIRED glm) # finds glm
  target_link_libraries(familyline-common PUBLIC fmt::fmt ${GLM_LIBRARIES} ${ZLIB_LIBRARIES})
  target_include_directories(familyline-common PUBLIC ${GLM_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIR})
endif(MSVC)

if (SET_COVERAGE)
  target_compile_options(familyline-common PUBLIC "-fprofile-arcs")
  target_compile_options(familyline-common PUBLIC "-ftest-coverage")
  target_compile_options(familyline-common PUBLIC "-O0")
  target_link_libraries(familyline-common PUBLIC "-fprofile-arcs")
  target_link_libraries(familyline-common PUBLIC "-ftest-coverage")
  target_link_libraries(familyline-common PUBLIC "gcov")
  target_link_libraries(familyline-common PUBLIC "--coverage")
endif()


target_include_directories(familyline-common PRIVATE "${CMAKE_SOURCE_DIR}/src/include")
target_compile_features(familyline-common PRIVATE cxx_std_20)

# Retrieve commit number (useful when getting logs from git clones)
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE COMMIT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE )

# Retrieve the user who compiled
execute_process(COMMAND whoami OUTPUT_VARIABLE USERNAME
  OUTPUT_STRIP_TRAILING_WHITESPACE)

target_compile_definitions(familyline-common PUBLIC VERSION="${PROJECT_VERSION}"
  COMMIT="${COMMIT_HASH}" USERNAME="${USERNAME}")
