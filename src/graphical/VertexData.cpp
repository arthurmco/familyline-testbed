#include "VertexData.hpp"
#include "ShaderManager.hpp"

using namespace Tribalia::Graphics;

VertexData::VertexData()
{
	this->shader = ShaderManager::Get("default");
	this->animator = std::unique_ptr<BaseAnimator>(new BaseAnimator(this));
}

VertexData::VertexData(VertexData& vd)
{
	this->Positions = vd.Positions;
	this->Normals = vd.Normals;
	this->TexCoords = vd.TexCoords;
	this->render_format = vd.render_format;
	this->shader = vd.shader;
	this->MaterialIDs = vd.MaterialIDs;
}

VertexData::VertexData(BaseAnimator* a)
{
	this->shader = ShaderManager::Get("default");
	this->animator = std::unique_ptr<BaseAnimator>(a);
}

const std::vector<glm::vec3>& BaseAnimator::getVertices(unsigned frameno)
{
	return vdata->Positions;
}