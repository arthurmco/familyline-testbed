/*
    Object picking class

    Copyright (C) 2015 Arthur M
*/

#ifndef INPUTPICKER_HPP
#define INPUTPICKER_HPP

#include <glm/glm.hpp>

#include "Cursor.hpp"
#include "../graphical/TerrainRenderer.hpp"
#include "../graphical/Renderer.hpp"
#include "../graphical/Camera.hpp"
#include "../graphical/SceneManager.hpp"
#include "../logic/ObjectManager.hpp"
#include "../logic/LocatableObject.hpp"
namespace Tribalia {
namespace Input {

    class InputPicker {
    private:
        Tribalia::Graphics::TerrainRenderer* _terrain;
		Tribalia::Graphics::Renderer* _renderer;
		Tribalia::Graphics::SceneManager* _sm;
		Tribalia::Graphics::Camera* _cam;
		Tribalia::Logic::ObjectManager* _om;

		bool CheckIfTerrainIntersect(glm::vec3 ray, float start, float end);

    public:

#define MAX_PICK_ITERATIONS 20

        InputPicker(Tribalia::Graphics::TerrainRenderer* terrain,
			Tribalia::Graphics::Renderer* renderer,
			Tribalia::Graphics::SceneManager* sm,
			Tribalia::Graphics::Camera* cam,
			Tribalia::Logic::ObjectManager* om);

        /* Get cursor ray in screen space */
        glm::vec4 GetCursorScreenRay();

		/* Get cursor ray in eye space */
        glm::vec4 GetCursorEyeRay();

		/* Get cursor ray in world space */
		glm::vec3 GetCursorWorldRay();

		/*	Get position where the cursor collides with the
			terrain, in render coordinates */
		glm::vec3 GetTerrainProjectedPosition();

		/*	Get the object that were intersected by the cursor ray */
		Tribalia::Logic::LocatableObject* GetIntersectedObject();

    };

} /* Input */
} /* Tribalia */

#endif /* end of include guard: INPUTPICKER_HPP */
