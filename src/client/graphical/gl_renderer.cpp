#include <client/graphical/gl_renderer.hpp>
#include <client/graphical/exceptions.hpp>
#include <client/graphical/shader_manager.hpp>
#include <client/graphical/gfx_service.hpp>

#include <common/logger.hpp>

using namespace familyline;
using namespace familyline::graphics;

GLRenderer::GLRenderer()
{
	_sForward = new ShaderProgram("forward", {
		Shader("shaders/Forward.vert", ShaderType::Vertex),
		Shader("shaders/Forward.frag", ShaderType::Fragment)
	});

	_sForward->link();

    _sLines = new ShaderProgram("lines", {
		Shader("shaders/Lines.vert", ShaderType::Vertex),
		Shader("shaders/Lines.frag", ShaderType::Fragment)
	});

	_sLines->link();
    glLineWidth(4.0f);
}

VertexHandle* GLRenderer::createVertex(VertexData& vd, VertexInfo& vi)
{
    auto& log = LoggerService::getLogger();
	auto [vao, vboPos, vboNorm, vboTex] = this->createRaw(vd);

	if (!vi.shaderState.shader)
		throw graphical_exception("Invalid shader detected while creating vertex");

	auto vhandle = std::make_unique<GLVertexHandle>(vao, *this, vi);
	vhandle->vsize = vd.position.size();

    log->write("gl-renderer", LogType::Debug,
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
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now

	auto& shaderManager = GFXService::getShaderManager();
    auto& log = LoggerService::getLogger();

	auto viewMatrix = c->GetViewMatrix();
	auto projMatrix = c->GetProjectionMatrix();

    

	for (auto& vh : _vhandle_list) {        
		ShaderProgram* shader = vh->vinfo.shaderState.shader;
		shaderManager->use(*shader);
        shader->setUniform("lightCount", 0);
//        shader->setUniform("lights[0].position", glm::vec3(30, 50, 30));
//        shader->setUniform("lights[0].color", glm::vec3(1, 1, 1));
//        shader->setUniform("lights[0].strength", 100.0f);

        shader->setUniform("mView", viewMatrix);
		shader->setUniform("mProjection", projMatrix);
		shader->setUniform("mvp", projMatrix * viewMatrix * glm::mat4(1.0));

        if (vh->vinfo.materialID >= 0) {
  
            Material* m = GFXService::getMaterialManager()->getMaterial(vh->vinfo.materialID);
            MaterialData md = m->getData();

            shader->setUniform("diffuse_color", md.diffuseColor);
            shader->setUniform("ambient_color", md.ambientColor);
            shader->setUniform("diffuse_intensity", 1.0f);
            shader->setUniform("ambient_intensity", 1.0f);

            if (Texture *t = m->getTexture(); t) {
                glBindTexture(GL_TEXTURE_2D, t->GetHandle());
                shader->setUniform("tex_amount", 1.0f);

            } else {
                glBindTexture(GL_TEXTURE_2D, 0);
                shader->setUniform("tex_amount", 0.0f);
            }

//            printf("<<<< %d dif=%.2f,%.2f,%.2f amb=%.2f,%.2f,%.2f  >>>>",
//                   vh->vinfo.materialID,
//                   md.diffuseColor.x, md.diffuseColor.y, md.diffuseColor.z,
//                   md.ambientColor.x, md.ambientColor.y, md.ambientColor.z);

        } else {
            glBindTexture(GL_TEXTURE_2D, 0);
            shader->setUniform("tex_amount", 0.0f);
            shader->setUniform("diffuse_color", glm::vec3(0.5));
            shader->setUniform("ambient_color", glm::vec3(0.1));
            shader->setUniform("diffuse_intensity", 0.0f);
            shader->setUniform("ambient_intensity", 0.0f);            
        }

        vh->vinfo.shaderState.updateShader();

		bool hasTexture = vh->vinfo.hasTexCoords;

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

        auto glFormat = vh->vinfo.renderStyle == VertexRenderStyle::Triangles ?
            GL_TRIANGLES : GL_LINE_STRIP;        
		glDrawArrays(glFormat, 0, vh->vsize);
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
		    log->write("gl-renderer", LogType::Error,
                       "OpenGL error %#x", err);
			return;
		}
	}

    glBindTexture(GL_TEXTURE_2D, 0);

//	glBindVertexArray(0);
}

/**
 * Create a raw VAO, with the vbox for position, normal and texture.
 *
 * Useful when we need to only retrieve the basic elements VAO, without an object
 */
std::tuple<int, int, int, int> GLRenderer::createRaw(VertexData& vd)
{
    auto& log = LoggerService::getLogger();

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


    log->write("gl-renderer", LogType::Debug,
               "created vertex set: vao=%#x, vbos=%#x,%#x,%#x",
               vao, vboPos, vboNorm, vboTex);

	return std::tie(vao, vboPos, vboNorm, vboTex);
}

void GLRenderer::removeVertex(VertexHandle* vh)
{
    GLVertexHandle* gvh = dynamic_cast<GLVertexHandle*>(vh);
    if (!gvh)
        return;

    glDeleteVertexArrays(1, (GLuint*)&gvh->vao);
    glDeleteBuffers(1, (GLuint*)&gvh->vboPos);
    glDeleteBuffers(1, (GLuint*)&gvh->vboNorm);

    if (gvh->vboTex > 0)
		glDeleteBuffers(1, (GLuint*)&gvh->vboTex);

    _vhandle_list
        .erase(std::remove_if(_vhandle_list.begin(), _vhandle_list.end(),
                              [gvh](std::unique_ptr<GLVertexHandle> &handle) {
                                  return (handle->vao == gvh->vao);
                              }));

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
    this->_renderer.removeVertex(this);
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
