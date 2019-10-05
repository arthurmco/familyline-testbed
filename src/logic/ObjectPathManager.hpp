/*
  Manages object movement paths and update them in the game

  Copyright (C) 2017 Arthur M

*/

#include <map>
#include <vector>

#include <glm/glm.hpp>
#include "game_object.hpp"
#include "Terrain.hpp"

#ifndef OBJECTPATHMANAGER_HPP
#define OBJECTPATHMANAGER_HPP

namespace familyline::logic {

    /* The object path unit */
    struct ObjectPathRef {
        int pathid;
        GameObject* lc;
        std::vector<glm::vec2>* path;
        glm::vec2 path_point;
        unsigned int path_ptr = 0;
        bool interrupted = false;
        int dbg_path_plot;

        unsigned current_time = 0; // The current time (in ms) of this path
        unsigned last_step_time = 0;    // The last time of that path last movement

        /* The path only advances in a certain time. The last_step variable records
           that time
        */ 
    
        ObjectPathRef(int pathid, GameObject* lc, 
                      std::vector<glm::vec2>* path,
                      int dbg_path_plot)
            : pathid(pathid), lc(lc), path(path), path_point((*path)[0]),
              path_ptr(0), dbg_path_plot(dbg_path_plot)
        {}
    };

    class ObjectPathManager {
    private:    
        std::vector<ObjectPathRef> _pathrefs;
        Terrain* _terr = nullptr;
    
    public:
        /*  Adds a path from object 'o' to the path manager 
            Returns true if added successfully, or false if there's already a path 
            there for the same object
        */
        bool AddPath(GameObject* o, std::vector<glm::vec2>* path);

        /*  Removes a path from object 'oid'. 
            Returns true if path removed, or false if path didn't exist there */
        bool RemovePath(long oid);

        /* Update the paths
         * Also, removes the completed paths
         * */
        void UpdatePaths(unsigned ms_frame);

        void SetTerrain(Terrain*);
    
    

        static ObjectPathManager* getInstance() {
            static ObjectPathManager* i = nullptr;
            if (!i)     i = new ObjectPathManager;

            return i;
        }

    };

}

#endif
