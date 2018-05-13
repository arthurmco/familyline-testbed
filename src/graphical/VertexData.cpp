#include "VertexData.hpp"
#include "ShaderManager.hpp"
#include "Camera.hpp"

using namespace Familyline::Graphics;

VertexData::VertexData()
{
	this->SetShader(ShaderManager::Get(ShaderManager::DefaultShader));
	this->animator = std::unique_ptr<BaseAnimator>(new BaseAnimator(this));
}


/* Sets the shader and the shader setup function together, so we
 * don't fuck up something
 */
void VertexData::SetShader(ShaderProgram* s) {
	this->shader = s;

	if (s) {
		if (!strcmp(s->name, "forward")) {
			this->fnShaderSetup = [](ShaderProgram* s, glm::mat4 m, glm::mat4 v,
				glm::mat4 p, Camera* c) {
				(void)c;
				s->SetUniform("mvp", p * v * m);
				s->SetUniform("mModel", m);
			};
		}

		if (!strcmp(s->name, "lines")) {
			this->fnShaderSetup = [](ShaderProgram* s, glm::mat4 m, glm::mat4 v,
				glm::mat4 p, Camera* c) {
				
				s->SetUniform("mvp", 
					c->GetProjectionMatrix() * c->GetViewMatrix() *	m);

			};
		}
	}

}


VertexData::VertexData(VertexData& vd)
{
	this->Positions = vd.Positions;
	this->Normals = vd.Normals;
	this->TexCoords = vd.TexCoords;
	this->render_format = vd.render_format;
	this->SetShader(vd.shader);
	this->materialID = vd.materialID;
	this->shader = ShaderManager::Get(ShaderManager::DefaultShader);
	this->animator = std::unique_ptr<BaseAnimator>(new BaseAnimator(this));
}

VertexData::VertexData(BaseAnimator* a)
{
	this->SetShader(ShaderManager::Get(ShaderManager::DefaultShader));
	this->animator = std::unique_ptr<BaseAnimator>(a);
}

const std::vector<glm::vec3>& BaseAnimator::getVertices(unsigned frameno)
{
	return vdata->Positions;
}
