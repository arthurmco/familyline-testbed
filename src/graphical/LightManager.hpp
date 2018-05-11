/*
 *	  Manages the lights in the scene
 *	  It's primary job is to sort the lights by strength and send only
 *    the lights that will affect the scene to the renderer
 *
 *	  Copyright (C) 2018 Arthur M
 */
#pragma once

#include "Light.hpp"
#include <glm/glm.hpp>
#include <list>

namespace Familyline::Graphics {

	class LightManager {
	private:
		static std::list<Light*> _lights;

	public:
		/* Adds a light to the manager */
		static void AddLight(Light* l);

		/* Removes a light */
		static void Remove(Light* l);

		/* Removes all lights */
		static void RemoveAll();

		/* 
		 * Get the best lights, the strongest ones, from a circle that 
		 * has the center in 'çenter' and radius 'radius'
		 */
		static std::list<Light*> GetBestLights(glm::vec3 center, double radius, const int max_lights);
	};

}