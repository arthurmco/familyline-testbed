#include "Renderer.hpp"
#include "../config.h"

#include "ShaderManager.hpp"

using namespace familyline::graphics;

GLuint vao_tri = 0, vbo_tri = 0;


Renderer::Renderer()
{
    //InitializeLibraries();
    //Enable depth test
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
}

void Renderer::initialize() {
    this->InitializeShaders();
    sForward->Use();

    /* 	Create a fake texture, so we can render something
	if textures aren't available */

    unsigned int* fake_color = new unsigned int;

    fake_color[0] = 0xffff00ff;
    fake_tex = new Texture(1, 1, GL_RGBA, fake_color);
}

void Renderer::InitializeLibraries()
{

}

void Renderer::InitializeShaders()
{

#define TRY_COMPILE_OR_THROW(shader)					\
    if (!shader->Compile()) {						\
	throw shader_exception("Shader failed to compile", glGetError(), \
			       shader->GetPath(), shader->GetType()); }
    
#define TRY_LINK_OR_THROW(program)					\
    if (!program->Link()) {						\
	char shnum[6];							\
	sprintf(shnum, "%d", program->GetID());				\
	throw shader_exception("Shader failed to link", glGetError(),	\
			       shnum, SHADER_PROGRAM);			\
    }

    
    Shader *sFrag, *sVert;
    Shader *fLines, *vLines;

    sFrag = new Shader{ SHADERS_DIR "Forward.frag", SHADER_PIXEL };
    sVert = new Shader{ SHADERS_DIR "Forward.vert", SHADER_VERTEX };
    fLines = new Shader{ SHADERS_DIR "Lines.frag", SHADER_PIXEL };
    vLines = new Shader{ SHADERS_DIR "Lines.vert", SHADER_VERTEX };

    TRY_COMPILE_OR_THROW(sFrag)
    TRY_COMPILE_OR_THROW(sVert)
    
    sForward = new ShaderProgram{ "forward", sVert, sFrag };
    TRY_LINK_OR_THROW(sForward);
    
    TRY_COMPILE_OR_THROW(fLines);
    TRY_COMPILE_OR_THROW(vLines);

    sLines = new ShaderProgram{ "lines", vLines, fLines };
    TRY_LINK_OR_THROW(sLines);
    
}

#include "TextureManager.hpp"

void Renderer::SetMaterial(int ID)
{
    Material* m = MaterialManager::GetInstance()->GetMaterial(ID);
    if (!m) {
	return;
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    /* Bind a texture */
    Texture* t = m->GetTexture();

    if (t) {
	glBindTexture(GL_TEXTURE_2D, t->GetHandle());
	sForward->SetUniform("tex_amount", 1.0f);
    }
    else {
	glBindTexture(GL_TEXTURE_2D, fake_tex->GetHandle());
	sForward->SetUniform("tex_amount", 0.0f);
    }

    /* Set materials */
    sForward->SetUniform("diffuse_color", m->GetData()->diffuseColor);
    sForward->SetUniform("ambient_color", m->GetData()->ambientColor);

}

void Renderer::UpdateObjects()
{
    lastCheck++;
    auto objList = _scenemng->GetValidObjects();

    /* Check updates from SceneManager*/
    if (_scenemng->UpdateValidObjects()) {
	    
	auto lightIdx = 0;
	lightCount = 0;

	/* Check for inserted objects */
	for (auto itScene = objList->begin(); itScene != objList->end(); itScene++) {

	    /* Check for inserted meshes */
	    switch ((*itScene)->GetType()) {
	    case SCENE_MESH:
	    {
		auto meshIsNew = true;
		for (auto it2 = _last_IDs.begin(); it2 != _last_IDs.end(); it2++) {
		    if ((*itScene)->GetID() == it2->ID && (*itScene)->GetType() == SCENE_MESH) {
			it2->lastcheck = lastCheck;
			meshIsNew = false;
			break;
		    }
		}
				

		/* Draw the added object */
		Mesh* mes = (Mesh*)(*itScene);
		mes->ApplyTransformations();

		auto vdlist = mes->getAnimator()->getCurrentFrame();
			    

		if (meshIsNew) {
		    auto vidx = 0;
		    for (auto& vd : vdlist) {
			printf("Size: %zu", vd.position.size());

			VertexHandle vhandle = this->addVertexData(
			    vd, mes->getVertexInfo(vidx));

			SceneIDCache sidc;
			sidc.ID = (*itScene)->GetID();
			sidc.lastcheck = lastCheck;
			sidc.vhandle = vhandle;
			sidc.bbvao = this->AddBoundingBox(mes, glm::vec3(1, 0, 0));
			_last_IDs.push_back(sidc);
			printf("???\n");

			vidx++;
		    }
		}
				
	    }
	    break;

	    case SCENE_LIGHT:
	    {
		Light* l = (Light*)(*itScene);
		lri[lightIdx].lightPosition = l->GetPosition();
		int cr, cg, cb;
		l->GetColor(cr, cg, cb);
		lri[lightIdx].lightColor = glm::vec3(cr / 255.0, cg / 255.0, cb / 255.0);
		lri[lightIdx].lightStrength = l->GetStrength();
		lightCount = ++lightIdx;
	    }
	    break;

	    default:
		Log::GetLog()->Warning("renderer",
				       "Unsupported scene object! Skipping...");
		break;
	    }

	}

	/* Check for deleted objects */
	for (auto it2 = _last_IDs.begin(); it2 != _last_IDs.end(); ) {
	    bool isDeleted = true;
			
	    for (auto itScene = objList->begin(); itScene != objList->end(); itScene++) {

		if (it2->ID == (*itScene)->GetID()) {
		    isDeleted = false;
		    break;
		}
	    }

	    if (isDeleted) {
		Log::GetLog()->Write("renderer",
				     "Removing object ID %d from the cache",
				     it2->ID);
		this->removeVertexData(std::move(it2->vhandle));
		this->RemoveBoundingBox(it2->bbvao);
		it2 = _last_IDs.erase(it2);
		//break;
	    } else {
		++it2;
	    }

	}

    }

}


bool Renderer::Render(TerrainRenderer* terr_rend)
{

    glm::mat4 mModel, mView, mProj;
    mView = this->_scenemng->GetCamera()->GetViewMatrix();
    mProj = this->_scenemng->GetCamera()->GetProjectionMatrix();

    sForward->Use();
    sForward->SetUniform("mView", mView);

    for (unsigned int i = 0; i < lightCount; i++) {
	sForward->SetUniformStructArray("lights", i, "position", lri[i].lightPosition);
	sForward->SetUniformStructArray("lights", i, "color", lri[i].lightColor);
	sForward->SetUniformStructArray("lights", i, "strength", lri[i].lightStrength);
    }
    sForward->SetUniform("lightCount", (int)lightCount);
	
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    terr_rend->Render();
		
	
    int material = 0;

    /* Render registered VAOs */
    auto idx = 0;
    for (auto it = _vertices.begin(); it != _vertices.end(); ++it) {
	mModel = *it->vi.worldMat;

	ShaderProgram* shader = it->sp; //TODO: Get vertexinfo
		
	shader->Use();
	shader->SetUniform("mvp", mProj * mView * mModel);
	shader->SetUniform("mModel", mModel);
	
	
//		it->vd->fnShaderSetup(shader, mModel, mView, mProj,
//			_scenemng->GetCamera());

	/*
	  if (it->vd->animationData) {
	  this->UpdateVertexData(it->vd->vboPos,
	  it->vd->animationData->GetVertexRawData(),
	  it->vd->position.size());
	  }
	*/

	material = it->vi.materialID;
	const auto render_format = VertexRenderStyle::Triangles;

	glBindVertexArray(it->vao);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, it->vboPos);
	glVertexAttribPointer(
	    0,                  // attribute 0 (positions)
	    3,                  // size
	    GL_FLOAT,           // type
	    GL_FALSE,           // normalized?
	    0,                  // stride
	    (void*)0            // array buffer offset
	    );

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, it->vboNorm);
	glVertexAttribPointer(
	    1,                  // attribute 1 (normals)
	    3,                  // size
	    GL_FLOAT,           // type
	    GL_FALSE,           // normalized?
	    0,                  // stride
	    (void*)0            // array buffer offset
	    );

	if (it->vboTex) {
	    glEnableVertexAttribArray(2);
	    glBindBuffer(GL_ARRAY_BUFFER, it->vboTex);
	    glVertexAttribPointer(
		2,					// attribute 2 (texcoords)
		2,					// size
		GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

		
	auto drawstyle = (render_format == VertexRenderStyle::Triangles) ?
	    GL_TRIANGLES : GL_LINE_STRIP;

	if (render_format == VertexRenderStyle::PlotLines) {
	    glGetError();

	    glDrawArrays(drawstyle, 0, it->vsize);

	    GLenum err = glGetError();
	    if (err != GL_NO_ERROR) {
		Log::GetLog()->Fatal("renderer", "rendering plot: OpenGL error %#x", err);
	    }
	} else {
	    SetMaterial(material);
	    glGetError();

	    glDrawArrays(drawstyle, 0, it->vsize);
		    
	    GLenum err = glGetError();
	    if (err != GL_NO_ERROR) {
		Log::GetLog()->Fatal("renderer", "rendering mesh: OpenGL error %#x", err);
	    }
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableVertexAttribArray(0);
    }

    if (renderBBs) {
	this->RenderBoundingBoxes();
    }

    return true;
}

void Renderer::UpdateFrames()
{
    //for (auto v : _vertices) {
    //if (v.vd->animator) {
    /* Has animation things */
    //  UpdateVertexData(v.vboPos, v.vd->animationData->GetVertexRawData(),
    //	     v.vd->position.size());
    //printf("Updated mesh %d to frame %d", v.vao, v.vd->animationData->GetActualFrame());
    //}
//	}
}

SceneManager* Renderer::GetSceneManager() const
{
    return this->_scenemng;
}

void Renderer::SetSceneManager(SceneManager* scenemng)
{
    this->_scenemng = scenemng;
}

const VertexHandle Renderer::addVertexData(const VertexData& vdata, const VertexInfo vinfo)
{
    VertexHandle vhandle = {};

    glGenVertexArrays(1, &vhandle.vao);
    glBindVertexArray(vhandle.vao);
    auto animated = false;
	
    glGenBuffers(1, &vhandle.vboPos);
    glBindBuffer(GL_ARRAY_BUFFER, vhandle.vboPos);
    glBufferData(GL_ARRAY_BUFFER, vdata.position.size() * sizeof(glm::vec3),
		 vdata.position.data(), (animated ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vhandle.vboNorm);
    glBindBuffer(GL_ARRAY_BUFFER, vhandle.vboNorm);
    glBufferData(GL_ARRAY_BUFFER, vdata.normals.size() * sizeof(glm::vec3),
		 vdata.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    if (vdata.texcoords.size() > 0) {
	glGenBuffers(1, &vhandle.vboTex);
	glBindBuffer(GL_ARRAY_BUFFER, vhandle.vboTex);
	glBufferData(GL_ARRAY_BUFFER, vdata.texcoords.size() * sizeof(glm::vec2),
		     vdata.texcoords.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);
    }

    glBindVertexArray(0);

    Log::GetLog()->Write("renderer", "Added %zu vertices with VAO %d (VBO %d)",
			 vdata.position.size(), vhandle.vao, vhandle.vboPos);

    vhandle.vsize = vdata.position.size();
    vhandle.vi = vinfo;
    vhandle.sp = ShaderManager::Get("forward");
    
    _vertices.push_back(vhandle);
    
    return vhandle;
}

void Renderer::updateVertexData(VertexHandle vhandle,
				const VertexData& vdata, const VertexInfo vinfo)
{
    assert(vhandle.vao > 0);

    (void)vinfo; // TODO: modify this
    
    glBindBuffer(GL_ARRAY_BUFFER, vhandle.vboPos);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vdata.position.size() * sizeof(glm::vec3),
		    (void*)vdata.position.data());
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::removeVertexData(VertexHandle&& vhandle)
{
    assert(vhandle.vao > 0);
    const auto vao_id = vhandle.vao;
    
    for (auto it = _vertices.begin(); it != _vertices.end(); it++) {
	if (it->vao == vao_id) {
	    if (it->vao_bbox) {
		RemoveBoundingBox(it->vao_bbox);
	    }

	    glDeleteVertexArrays(1, &vao_id);
	    glDeleteBuffers(1, &it->vboPos);
	    glDeleteBuffers(1, &it->vboNorm);

	    if (it->vboTex)
		glDeleteBuffers(1, &it->vboTex);

	    _vertices.erase(it);
	    break;
	}
    }    
}



/* Render object bounding boxes */
void Renderer::RenderBoundingBoxes()
{
    sLines->Use();
    for (auto it = _bb_vaos.begin(); it != _bb_vaos.end(); it++) {
	sLines->SetUniform("mvp",
			   _scenemng->GetCamera()->GetProjectionMatrix() *_scenemng->GetCamera()->GetViewMatrix() * (*it->vi.worldMat));

	glBindVertexArray(it->vao);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, it->vboPos);
	glVertexAttribPointer(
	    0,                  // attribute 0 (positions)
	    3,                  // size
	    GL_FLOAT,           // type
	    GL_FALSE,           // normalized?
	    0,                  // stride
	    (void*)0            // array buffer offset
	    );

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, it->vboNorm);	// use normal space for colors
	glVertexAttribPointer(
	    1,                  // attribute 1 (colors)
	    3,                  // size
	    GL_FLOAT,           // type
	    GL_FALSE,           // normalized?
	    0,                  // stride
	    (void*)0            // array buffer offset
	    );

	glGetError();
	glDrawArrays(GL_LINE_STRIP, 0, it->vsize);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
	    Log::GetLog()->Fatal("renderer", "OpenGL error %#x", err);
	}
    }
}

int Renderer::AddBoundingBox(Mesh* m, glm::vec3 color)
{
    VertexHandle vri = {};
    VertexHandle* vri_mesh = nullptr;

    /* Find the original mesh in our list */
    for (auto& vv : _vertices) {
	if (vv.vi.worldMat == m->GetModelMatrixPointer()) {
	    vri_mesh = &vv;
	    break;
	}
    }

    /* Generate bb points */

    m->GenerateBoundingBox();
    BoundingBox bb = m->GetBoundingBox();
    auto vdata = new VertexData;
    vdata->position.push_back(glm::vec3(bb.minX, bb.minY, bb.minZ));
    vdata->position.push_back(glm::vec3(bb.minX, bb.maxY, bb.minZ));
    vdata->position.push_back(glm::vec3(bb.maxX, bb.maxY, bb.minZ));
    vdata->position.push_back(glm::vec3(bb.minX, bb.minY, bb.minZ));
    vdata->position.push_back(glm::vec3(bb.maxX, bb.minY, bb.minZ));
    vdata->position.push_back(glm::vec3(bb.maxX, bb.maxY, bb.minZ));

    vdata->position.push_back(glm::vec3(bb.minX, bb.maxY, bb.minZ));
    vdata->position.push_back(glm::vec3(bb.minX, bb.maxY, bb.maxZ));
    vdata->position.push_back(glm::vec3(bb.maxX, bb.maxY, bb.maxZ));
    vdata->position.push_back(glm::vec3(bb.minX, bb.maxY, bb.minZ));
    vdata->position.push_back(glm::vec3(bb.maxX, bb.maxY, bb.minZ));
    vdata->position.push_back(glm::vec3(bb.maxX, bb.maxY, bb.maxZ));

    vdata->position.push_back(glm::vec3(bb.minX, bb.minY, bb.maxZ));
    vdata->position.push_back(glm::vec3(bb.minX, bb.maxY, bb.maxZ));
    vdata->position.push_back(glm::vec3(bb.maxX, bb.maxY, bb.maxZ));
    vdata->position.push_back(glm::vec3(bb.minX, bb.minY, bb.maxZ));
    vdata->position.push_back(glm::vec3(bb.maxX, bb.minY, bb.maxZ));
    vdata->position.push_back(glm::vec3(bb.maxX, bb.maxY, bb.maxZ));

    vri.vsize = vdata->position.size();
	
    for (size_t i = 0; i < vdata->position.size(); i++) {
	// Use normals for color, so we can use the same struct.
	vdata->normals.push_back(color);
    }

    vri.vi.worldMat = m->GetModelMatrixPointer();

    glGenVertexArrays(1, &vri.vao);
    glBindVertexArray(vri.vao);

    glGenBuffers(1, &vri.vboPos);
    glBindBuffer(GL_ARRAY_BUFFER, vri.vboPos);
    glBufferData(GL_ARRAY_BUFFER, vdata->position.size() * sizeof(glm::vec3),
		 vdata->position.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &vri.vboNorm);
    glBindBuffer(GL_ARRAY_BUFFER, vri.vboNorm);
    glBufferData(GL_ARRAY_BUFFER, vdata->normals.size() * sizeof(glm::vec3),
		 vdata->normals.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    Log::GetLog()->Write("renderer", "Added bounding box of mesh vao %d with VAO %d (VBO %d)",
			 vri_mesh ? vri_mesh->vao : -1, vri.vao, vri.vboPos);

    if (vri_mesh) {
	vri_mesh->vao_bbox = vri.vao;
    }

    _bb_vaos.push_back(vri);
    return vri.vao;
}

void Renderer::RemoveBoundingBox(GLuint vao)
{
    VertexHandle vri;
    for (auto it = _bb_vaos.begin(); it != _bb_vaos.end(); it++) {
	if (it->vao == vao) {
	    glDeleteVertexArrays(1, &vao);
	    glDeleteBuffers(1, &it->vboPos);
	    glDeleteBuffers(1, &it->vboNorm);
	    _bb_vaos.erase(it);
	    break;
	}
    }
}

void Renderer::SetBoundingBox(bool b) {

    renderBBs = b;
    if (b != renderBBs) {
	glLineWidth(4.0f);
    }


}


Renderer::~Renderer()
{
    SDL_Quit();
}
