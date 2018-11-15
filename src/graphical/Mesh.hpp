/***
    Representation of a mesh

    Copyright (C) 2016, 2018 Arthur Mendes.

***/
#define GLM_FORCE_RADIANS

#include <algorithm> //min(), max()

#include <glm/glm.hpp>

#include "../logic/IMesh.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "VertexData.hpp"
#include "SceneObject.hpp"

#ifndef MESH_HPP
#define MESH_HPP

namespace familyline::graphics {

    class Material;
    
    class Mesh : public SceneObject, public logic::IMesh
    {
    private:
        glm::mat4 _rotMatrix;
        glm::mat4 _scaleMatrix;
        glm::mat4 _translMatrix;

        bool _isModelChanged = false;
        glm::mat4 _modelMatrix;

        void Translate(glm::vec3);
        void Scale(glm::vec3);

        /* warning: rotate angle should be in radians */
        void Rotate(glm::vec3 axis, float angle);

	std::vector<VertexData*> _vdata;
    public:
        Mesh(VertexData* vd);
	Mesh(std::vector<VertexData*> vd);
	
        virtual void SetPosition(glm::vec3);
        void AddPosition(glm::vec3);

        void SetRotation(float x, float y, float z);
        void AddRotation(float x, float y, float z);

        void ApplyTransformations();

        glm::mat4 GetModelMatrix() const;
        glm::mat4* GetModelMatrixPointer();

        std::vector<VertexData*>& GetVertexData();

        void GenerateBoundingBox();

	/* Sets material for whole mesh */
	void SetMaterial(Material* m);

	virtual ~Mesh() {}
    };



} /* familyline::graphics */



#endif /* end of include guard: MESH_HPP */
