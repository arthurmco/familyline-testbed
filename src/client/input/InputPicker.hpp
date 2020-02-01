
/*
  Object picking class

  Copyright (C) 2015, 2018 Arthur M
*/

#ifndef INPUTPICKER_HPP
#define INPUTPICKER_HPP

#include <glm/glm.hpp>

#include "Cursor.hpp"
#include "../graphical/TerrainRenderer.hpp"
#include "../graphical/mesh.hpp"
#include "../graphical/window.hpp"
#include "../graphical/camera.hpp"
#include "../graphical/scene_renderer.hpp"
#include "../logic/object_manager.hpp"
#include "../logic/object_components.hpp"
#include "../logic/game_event.hpp"

namespace familyline::input {
#define MAX_PICK_ITERATIONS 16

    /**
     * \brief Object information needed by the picker
     *
     * This structure is made so we can get from the objects just what we
     * need when they are created.
     *
     * Since the meshes doesn't change, we don't need the object around
     */
    struct PickerObjectInfo {
        glm::vec3 position;
        std::shared_ptr<familyline::graphics::Mesh> mesh;
        familyline::logic::object_id_t ID;

        PickerObjectInfo(glm::vec3 pos,
                         std::shared_ptr<familyline::graphics::Mesh> mesh,
                         familyline::logic::object_id_t ID)
            : position(pos), mesh(mesh), ID(ID)
            {}

    };

    /**
     * \brief Allows the game to identify the game object under the cursor
     *
     * If you can select a unit or building, and see its information, thank this class
     */
    class InputPicker {
    private:
        familyline::graphics::TerrainRenderer* _terrain;
        familyline::graphics::Window* _win;
        familyline::graphics::SceneRenderer* _sm;
        familyline::graphics::Camera* _cam;
        familyline::logic::ObjectManager* _om;

        glm::vec3 _intersectedPosition;
        std::weak_ptr<familyline::logic::GameObject> _locatableObject;

        std::vector<const familyline::logic::GameObject*> _olist;
        familyline::logic::ObjectEventReceiver oel;

        bool CheckIfTerrainIntersect(glm::vec3 ray, float start, float end);

        std::vector<PickerObjectInfo> poi_list;
    public:

        InputPicker(familyline::graphics::TerrainRenderer* terrain,
                    familyline::graphics::Window* win,
                    familyline::graphics::SceneRenderer* sm,
                    familyline::graphics::Camera* cam,
                    familyline::logic::ObjectManager* om);

        /**
         * Get cursor ray in screen space
         */
        glm::vec4 GetCursorScreenRay();

        /**
         * Get cursor ray in eye space
         */
        glm::vec4 GetCursorEyeRay();

        /**
         * Get cursor ray in world space
         */
        glm::vec3 GetCursorWorldRay();

        void UpdateTerrainProjectedPosition();
        void UpdateIntersectedObject();

        /**
         * Get position where the cursor collides with the
         * terrain, in render coordinates
         */
        glm::vec3 GetTerrainProjectedPosition();

        /**
         * Get position where the cursor collides with the
         * terrain, in game coordinates
         */
        glm::vec2 GetGameProjectedPosition();

        /**
         * Get the object that were intersected by the cursor ray 
         */
        std::weak_ptr<familyline::logic::GameObject> GetIntersectedObject();

    };

    /* Input */
} /* Familyline */

#endif /* end of include guard: INPUTPICKER_HPP */
