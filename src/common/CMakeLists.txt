
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
  "logic/ObjectFactory.cpp"
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

