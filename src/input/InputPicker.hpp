/*
    Object picking class

    Copyright (C) 2015 Arthur M
*/

#ifndef INPUTPICKER_HPP
#define INPUTPICKER_HPP

#include <glm/glm.hpp>

#include "Cursor.hpp"
#include "../graphical/TerrainRenderer.hpp"
#include "../graphical/Window.hpp"
#include "../graphical/Camera.hpp"
#include "../graphical/SceneManager.hpp"
#include "../logic/ObjectManager.hpp"
#include "../logic/LocatableObject.hpp"
namespace Tribalia {
namespace Input {
#define MAX_PICK_ITERATIONS 12

class InputPicker {
private:
    Tribalia::Graphics::TerrainRenderer* _terrain;
		Tribalia::Graphics::Window* _win;
		Tribalia::Graphics::SceneManager* _sm;
		Tribalia::Graphics::Camera* _cam;
		Tribalia::Logic::ObjectManager* _om;

    glm::vec3 _intersectedPosition;
    Tribalia::Logic::LocatableObject* _locatableObject = nullptr;

		bool CheckIfTerrainIntersect(glm::vec3 ray, float start, float end);

public:

    InputPicker(Tribalia::Graphics::TerrainRenderer* terrain,
		Tribalia::Graphics::Window* win,
		Tribalia::Graphics::SceneManager* sm,
		Tribalia::Graphics::Camera* cam,
		Tribalia::Logic::ObjectManager* om);

    /* Get cursor ray in screen space */
    glm::vec4 GetCursorScreenRay();

		/* Get cursor ray in eye space */
    glm::vec4 GetCursorEyeRay();

		/* Get cursor ray in world space */
		glm::vec3 GetCursorWorldRay();

    void UpdateTerrainProjectedPosition();
    void UpdateIntersectedObject();

		/*	Get position where the cursor collides with the
			terrain, in render coordinates */
		glm::vec3 GetTerrainProjectedPosition();

    /*	Get position where the cursor collides with the
    terrain, in game coordinates */
    glm::vec2 GetGameProjectedPosition();

		/*	Get the object that were intersected by the cursor ray */
		Tribalia::Logic::LocatableObject* GetIntersectedObject();

    };

} /* Input */
} /* Tribalia */

#endif /* end of include guard: INPUTPICKER_HPP */
