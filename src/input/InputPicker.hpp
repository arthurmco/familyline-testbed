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

namespace Tribalia {
namespace Input {

    class InputPicker {
    private:
        Tribalia::Graphics::TerrainRenderer* _terrain;
		Tribalia::Graphics::Renderer* _renderer;
		Tribalia::Graphics::SceneManager* _sm;
		Tribalia::Graphics::Camera* _cam;

    public:
        InputPicker(Tribalia::Graphics::TerrainRenderer* terrain,
			Tribalia::Graphics::Renderer* renderer,
			Tribalia::Graphics::SceneManager* sm,
			Tribalia::Graphics::Camera* cam);

        /* Get cursor ray in screen space */
        glm::vec4 GetCursorScreenRay();

		/* Get cursor ray in eye space */
        glm::vec4 GetCursorEyeRay();

		/* Get cursor ray in world space */
		glm::vec3 GetCursorWorldRay();

		/*	Get position where the cursor collides with the
			terrain, in render coordinates */
		glm::vec3 GetTerrainProjectedPosition();
    };

} /* Input */
} /* Tribalia */

#endif /* end of include guard: INPUTPICKER_HPP */
