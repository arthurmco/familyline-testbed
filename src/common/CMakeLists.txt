
# Add all files that are common to the server and client:
# (logic, logging, net and object code)

add_library(
  familyline-common
  "Log.cpp"
  "logic/ActionManager.cpp"
  "logic/action_queue.cpp"
  "logic/attack_manager.cpp"
  "logic/BuildQueue.cpp"
  "logic/City.cpp"
  "logic/CityManager.cpp"
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
  "logic/Player.cpp"
  "logic/PlayerManager.cpp"
  "logic/Terrain.cpp"
  "logic/TerrainException.cpp"
  "logic/TerrainFile.cpp"
  "NetMessageQueue.cpp"
  "net/NetPlayerFilter.cpp"
  "net/NetPlayerManager.cpp"
  "net/NetServer.cpp"
  "objects/Tent.cpp"
  "objects/WatchTower.cpp"
  )

 

if (MSVC)
	
  include(${CMAKE_TOOLCHAIN_FILE})
  find_package(fmt CONFIG REQUIRED)
  target_include_directories(familyline-common PUBLIC ${GLM_INCLUDE_DIRS} ${YAML_INCLUDE_DIR})
  target_link_libraries(familyline-common PUBLIC fmt::fmt fmt::fmt-header-only)

endif(MSVC)

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
