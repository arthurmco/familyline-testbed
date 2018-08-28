/*
    Object picking class

    Copyright (C) 2015, 2018 Arthur M
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
#include "../logic/AttackableObject.hpp"
#include "../logic/ObjectEventListener.hpp"

namespace Familyline::Input {
#define MAX_PICK_ITERATIONS 16

    /**
     * Object information needed by the picker
     *
     * This structure is made so we can get from the objects just what we
     * need when they are created.
     *
     * Since the meshes doesn't change, we don't need the object around
     */
    struct PickerObjectInfo {
	glm::vec3 position;
	std::shared_ptr<Familyline::Graphics::Mesh> mesh;
	Familyline::Logic::object_id_t ID;

	PickerObjectInfo(glm::vec3 pos,
			 std::shared_ptr<Familyline::Graphics::Mesh> mesh,
			 Familyline::Logic::object_id_t ID)
	    : position(pos), mesh(mesh), ID(ID)
	    {}
    
    };

    
    class InputPicker {
    private:
	Familyline::Graphics::TerrainRenderer* _terrain;
	Familyline::Graphics::Window* _win;
	Familyline::Graphics::SceneManager* _sm;
	Familyline::Graphics::Camera* _cam;
	Familyline::Logic::ObjectManager* _om;

	glm::vec3 _intersectedPosition;
	Familyline::Logic::GameObject* _locatableObject = nullptr;

	std::vector<const Familyline::Logic::GameObject*> _olist;
	Familyline::Logic::ObjectEventListener oel;
    
	bool CheckIfTerrainIntersect(glm::vec3 ray, float start, float end);

	std::vector<PickerObjectInfo> poi_list;	
    public:

	InputPicker(Familyline::Graphics::TerrainRenderer* terrain,
		    Familyline::Graphics::Window* win,
		    Familyline::Graphics::SceneManager* sm,
		    Familyline::Graphics::Camera* cam,
		    Familyline::Logic::ObjectManager* om);

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
	Familyline::Logic::GameObject* GetIntersectedObject();

    };

    /* Input */
} /* Familyline */

#endif /* end of include guard: INPUTPICKER_HPP */
