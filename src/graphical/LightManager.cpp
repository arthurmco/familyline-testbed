#include "LightManager.hpp"
#include <algorithm>
#include <cstring>

using namespace Tribalia::Graphics;

std::list<Light*> LightManager::_lights;

/* Adds a light to the manager */
void LightManager::AddLight(Light* l)
{
	LightManager::_lights.push_back(l);
}

/* Removes a light */
void LightManager::Remove(Light* l)
{
	std::remove_if(LightManager::_lights.begin(), LightManager::_lights.end(),
		[l](Light* curr) {
			return !strcmp(l->GetName(), curr->GetName()); 
		}
	);
}

/* Removes all lights */
void LightManager::RemoveAll()
{
	LightManager::_lights.clear();
}

/*
* Get the best lights, the strongest ones, from a circle that
* has the center in 'çenter' and radius 'radius'
*/
std::list<Light*> LightManager::GetBestLights(glm::vec3 center,
	double radius, const int max_lights)
{
	auto ret = std::list<Light*>();
	
	/* Calculate the light score. Bigger score means a stronger light*/
	auto fnGetScore = [&](Light* light) {
		auto distance = glm::distance(center, light->GetPosition());
		return (radius - distance) * light->GetStrength();
	};

	auto lights = LightManager::_lights;

	for (unsigned i = 0; i < max_lights; i++) {
		auto minScore = 0;
		auto it_minLight = lights.end();

		for (auto light = lights.begin(); light != lights.end(); light++) {
			const auto score = fnGetScore(*light);
			printf("%s %.3f", (*light)->GetName(), score);

			if (score > minScore) {
				minScore = score;
				it_minLight = light;
			}
		}

		if (it_minLight != lights.end()) {
			ret.push_back(*it_minLight);
			lights.erase(it_minLight);
		}
	}

	return ret;
}