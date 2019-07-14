#include "gl_renderer.hpp"
#include "exceptions.hpp"
#include "shader_manager.hpp"

#include "Log.hpp"

using namespace familyline;
using namespace familyline::graphics;

GLRenderer::GLRenderer()
{
	_sForward = new ShaderProgram("forward", {
		Shader("shaders/Forward.vert", ShaderType::Vertex),
		Shader("shaders/Forward.frag", ShaderType::Fragment)
	});

	_sForward->link();
}

VertexHandle* GLRenderer::createVertex(VertexData& vd, VertexInfo& vi)
{
	auto [vao, vboPos, vboNorm, vboTex] = this->createRaw(vd);

	if (!vi.shaderState.shader)
		throw graphical_exception("Invalid shader detected while creating vertex");

	auto vhandle = std::make_unique<GLVertexHandle>(vao, *this, vi);
	vhandle->vsize = vd.position.size();

    Log::GetLog()->InfoWrite("gl-renderer",
		"created vertex handle: vao=%#x, vsize=%zu",
		vao, vhandle->vsize);

	vhandle->vboPos = vboPos;
	vhandle->vboNorm = vboNorm;
	vhandle->vboTex = vboTex;

	_vhandle_list.push_back(std::move(vhandle));

	return _vhandle_list.back().get();
}

void GLRenderer::render(Camera* c)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now

	auto& shaderManager = GFXService::getManager();
    auto l = Log::GetLog();

	auto viewMatrix = c->GetViewMatrix();
	auto projMatrix = c->GetProjectionMatrix();

	for (auto& vh : _vhandle_list) {
		ShaderProgram* shader = vh->vinfo.shaderState.shader;
		shaderManager->use(*shader);
		vh->vinfo.shaderState.updateShader();

		shader->setUniform("m_view", viewMatrix);
		shader->setUniform("m_projection", projMatrix);

		bool hasTexture = true;

		glBindVertexArray(vh->vao);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vh->vboPos);
		glVertexAttribPointer(
			0,                  // attribute 0 (positions)
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vh->vboNorm);
		glVertexAttribPointer(
			1,                  // attribute 1 (normals/color)
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		if (hasTexture) {
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, vh->vboTex);
			glVertexAttribPointer(
				2,					// attribute 2 (texcoords)
				2,					// size
				GL_FLOAT, GL_FALSE, 0, (void*)0);
		}

		glDrawArrays(GL_TRIANGLES, 0, vh->vsize);
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			l->Fatal("glrenderer", "rendering plot: OpenGL error %#x", err);
			return;
		}
	}

//	glBindVertexArray(0);
}

/**
 * Create a raw VAO, with the vbox for position, normal and texture.
 *
 * Useful when we need to only retrieve the basic elements VAO, without an object
 */
std::tuple<int, int, int, int> GLRenderer::createRaw(VertexData& vd)
{
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vboPos, vboNorm, vboTex;
	bool animated = true;

	glGenBuffers(1, &vboPos);
	glBindBuffer(GL_ARRAY_BUFFER, vboPos);
	glBufferData(GL_ARRAY_BUFFER, vd.position.size() * sizeof(glm::vec3),
		vd.position.data(), (animated ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &vboNorm);
	glBindBuffer(GL_ARRAY_BUFFER, vboNorm);
	glBufferData(GL_ARRAY_BUFFER, vd.normals.size() * sizeof(glm::vec3),
		vd.normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	if (vd.texcoords.size() > 0) {
		glGenBuffers(1, &vboTex);
		glBindBuffer(GL_ARRAY_BUFFER, vboTex);
		glBufferData(GL_ARRAY_BUFFER, vd.texcoords.size() * sizeof(glm::vec2),
			vd.texcoords.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);
	}

	glBindVertexArray(0);


    Log::GetLog()->InfoWrite("gl-renderer",
		"created vertex set: vao=%#x, vbos=%#x,%#x,%#x",
		vao, vboPos, vboNorm, vboTex);

	return std::tie(vao, vboPos, vboNorm, vboTex);
}

void GLRenderer::removeVertex(int vao)
{

}

bool GLVertexHandle::update(VertexData & vd)
{
	glBindVertexArray(this->vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->vboPos);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vd.position.size() * sizeof(glm::vec3), vd.position.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

bool GLVertexHandle::remove()
{
	this->_renderer.removeVertex(this->vao);
	return true;
}

bool GLVertexHandle::recreate(VertexData & vd, VertexInfo& vi)
{
	auto [vao, vboPos, vboNorm, vboTex] = this->_renderer.createRaw(vd);
	auto err = glGetError();

	if (err == GL_NO_ERROR) {
		this->vao = vao;

		this->vboPos = vboPos;
		this->vboNorm = vboNorm;
		this->vboTex = vboTex;
		this->vinfo = vi;

		return true;
	}

	return false;
}
