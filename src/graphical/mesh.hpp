#pragma once

#include <string>
#include "scene_object.hpp"
#include "animator.hpp"

class Mesh : public SceneObject {
private:
	std::string _name;
	glm::mat4 _worldMatrix;
	Animator* _ani;

public:
	Mesh(const char* name, Animator* ani, std::vector<VertexInfo> vinfo)
		: _name(name), _ani(ani), _worldMatrix(glm::mat4(1.0))
	{
		this->vinfo = vinfo;
	}

	glm::vec3 worldPosition;

	/**
	 * Update the world matrix and the animator
	 */
	virtual void update();

	virtual std::string_view getName() const;
	virtual glm::vec3 getPosition() const;
	virtual glm::mat4 getWorldMatrix() const;
	virtual std::vector<VertexData> getVertexData();
	virtual bool isVertexDataDirty();

	Animator* getAnimator();

};