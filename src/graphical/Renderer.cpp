#include "Renderer.hpp"

using namespace Tribalia::Graphics;

GLuint vao_tri = 0, vbo_tri = 0;


Renderer::Renderer()
{
    //InitializeLibraries();
    //Enable depth test
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    InitializeShaders();
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
    Shader *sFrag, *sVert;
    Shader *fLines, *vLines;

    sFrag = new Shader{"shaders/Forward.frag", SHADER_PIXEL};
    sVert = new Shader{"shaders/Forward.vert", SHADER_VERTEX};
    fLines = new Shader{ "shaders/Lines.frag", SHADER_PIXEL };
    vLines = new Shader{ "shaders/Lines.vert", SHADER_VERTEX };
	
    if (!sFrag->Compile()) {
        throw shader_exception("Shader failed to compile", glGetError(),
            sFrag->GetPath(), sFrag->GetType());
    }

    if (!sVert->Compile()) {
        throw shader_exception("Shader failed to compile", glGetError(),
            sVert->GetPath(), sVert->GetType());
    }

    sForward = new ShaderProgram{sVert, sFrag};
    if (!sForward->Link()) {
        char shnum[6];
        sprintf(shnum, "%d", sForward->GetID());
        throw shader_exception("Shader failed to link", glGetError(),
            shnum, SHADER_PROGRAM);
    }

	if (!fLines->Compile()) {
		throw shader_exception("Shader failed to compile", glGetError(),
			fLines->GetPath(), fLines->GetType());
	}

	if (!vLines->Compile()) {
		throw shader_exception("Shader failed to compile", glGetError(),
			vLines->GetPath(), vLines->GetType());
	}

	sLines = new ShaderProgram{ vLines, fLines };
	if (!sLines->Link()) {
		char shnum[6];
		sprintf(shnum, "%d", sLines->GetID());
		throw shader_exception("Shader failed to link", glGetError(),
			shnum, SHADER_PROGRAM);
	}
}

#include "TextureManager.hpp"

void Renderer::SetMaterial(int ID)
{
    Material* m = MaterialManager::GetInstance()->GetMaterial(ID);
    if (!m) {
        printf("Cannot found mat id %d\n", ID);
        return;
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    /* Bind a texture */
    Texture* t = m->GetTexture();

    if (t) {
	glBindTexture(GL_TEXTURE_2D, t->GetHandle());
	sForward->SetUniform("tex_amount", 1.0f);
    } else {
	glBindTexture(GL_TEXTURE_2D, fake_tex->GetHandle());
	sForward->SetUniform("tex_amount", 0.0f);
    }

    /* Set materials */
    sForward->SetUniform("diffuse_color", m->GetData()->diffuseColor);
    sForward->SetUniform("ambient_color", m->GetData()->ambientColor);

}

struct SceneIDCache {
    int ID;
    int lastcheck;
    GLuint vao;
    GLuint bbvao;
};

int lastCheck = 0;
std::vector<SceneIDCache> _last_IDs;

LightRenderInfo lri;

void Renderer::UpdateObjects()
{
	lastCheck++;
	auto objList = _scenemng->GetValidObjects();

	/* Check updates from SceneManager*/
	if (_scenemng->UpdateValidObjects()) {

	    /* Check for inserted objects */
	    for (auto itScene = objList->begin(); itScene != objList->end(); itScene++) {
		bool objExists = false;
			/* Check for inserted meshes */
		switch ((*itScene)->GetType()) {
		case SCENE_MESH:
		    for (auto it2 = _last_IDs.begin(); it2 != _last_IDs.end(); it2++) {
			if ((*itScene)->GetID() == it2->ID && (*itScene)->GetType() == SCENE_MESH) {
			    it2->lastcheck = lastCheck;
			    objExists = true;
			    break;
			}
		    }
		    break;
		    /* Check for inserted lights*/
		case SCENE_LIGHT:
		    for (int i = 0; i < lri.lightCount; i++) {
			if (lri.lightIDs[i] == (*itScene)->GetID()) {
			    objExists = true;
			    break;
			}
		    }
		    break;
		}
		
		
		if (objExists) continue;
		/* Draw the added object */
		if ((*itScene)->GetType() == SCENE_MESH) {
		    
		    /* Draw the added object */
		    Mesh* mes = (Mesh*)(*itScene);
		    mes->ApplyTransformations();
		    
		    int vaon = this->AddVertexData(mes->GetVertexData(),
						   mes->GetModelMatrixPointer());
		    
		    SceneIDCache sidc;
		    sidc.ID = (*itScene)->GetID();
		    sidc.lastcheck = lastCheck;
		    sidc.vao = vaon;
		    sidc.bbvao = this->AddBoundingBox(mes, glm::vec3(1,0,0));
		    _last_IDs.push_back(sidc);
		}
		else if ((*itScene)->GetType() == SCENE_LIGHT) {
		    
		    /*** TODO: Sort lights by strength and send only the most strong ones ***/
		    if (lri.lightCount > 4) {
			Log::GetLog()->Warning("Maximum number of lights per render exceeded.");
			continue;
		    }
		    
		    Light* light = (Light*)(*itScene);
		    Log::GetLog()->Write("Renderer added light %s", light->GetName());
		    int lR, lG, lB;
		    light->GetColor(lR, lG, lB);
		    lri.lightColors[lri.lightCount] = glm::vec3(lR / 255.0f, lG / 255.0f, lB / 255.0f);
		    lri.lightPositions[lri.lightCount] = light->GetPosition();
		    lri.lightStrengths[lri.lightCount] = light->GetStrength();
		    lri.lightIDs[lri.lightCount] = light->GetID();
		    lri.lightCount++;
		    
		}
		else {
		    Log::GetLog()->Warning("Unsupported scene object! Skipping...");
		    continue;
		}
	    }
	    
	    
	    
	    /* Check for deleted objects */
	    for (auto it2 = _last_IDs.begin(); it2 != _last_IDs.end(); ++it2) {
			bool isDeleted = true;
			for (auto itScene = objList->begin(); itScene != objList->end(); itScene++) {
			    
			    if (it2->ID == (*itScene)->GetID()) {
				isDeleted = false;
				break;
			    }
			}
			
			if (isDeleted) {
			    Log::GetLog()->Write("Removing object ID %d from the cache",
						 it2->ID);
			    this->RemoveVertexData(it2->vao);
			    this->RemoveBoundingBox(it2->bbvao);
			    _last_IDs.erase(it2);
			    break;
			}

	    }
	    
	}
	
}


bool Renderer::Render()
{

    glm::mat4 mModel, mView, mProj;
    mView = this->_scenemng->GetCamera()->GetViewMatrix();
    mProj = this->_scenemng->GetCamera()->GetProjectionMatrix();

    sForward->Use();
    sForward->SetUniform("mView", mView);

    sForward->SetUniform("lightCount", lri.lightCount);
    sForward->SetUniformArray("lightStrenghts", 4, lri.lightStrengths);
    sForward->SetUniformArray("lightPositions", 4, lri.lightPositions);
    sForward->SetUniformArray("lightColors", 4, lri.lightColors);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int material = 0;

    /* Render registered VAOs */
    for (auto it = _vertices.begin(); it != _vertices.end(); ++it) {
        mModel = *it->worldMat;
        sForward->SetUniform("mvp", mProj * mView * mModel);
        sForward->SetUniform("mModel", mModel);


	if (it->vd->animationData) {
	    this->UpdateVertexData(it->vd->vbo_pos, it->vd->animationData->GetVertexRawData(),
				   it->vd->Positions.size());
	}
		

        if (!it->vd->MaterialIDs.empty())
            material = it->vd->MaterialIDs[0];


        glBindVertexArray(it->vao);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, it->vbo_pos);
		glVertexAttribPointer(
           0,                  // attribute 0 (positions)
           3,                  // size
           GL_FLOAT,           // type
           GL_FALSE,           // normalized?
           0,                  // stride
           (void*)0            // array buffer offset
        );

        glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, it->vbo_norm);
	glVertexAttribPointer(
           1,                  // attribute 1 (normals)
           3,                  // size
           GL_FLOAT,           // type
           GL_FALSE,           // normalized?
           0,                  // stride
           (void*)0            // array buffer offset
        );

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, it->vbo_tex);
	glVertexAttribPointer(
	    2,					// attribute 2 (texcoords)
	    2,					// size
	    GL_FLOAT, GL_FALSE, 0, (void*)0);

        /* Draw the triangles */
        for (int matidx = 0; matidx < 9; matidx++) {
            int start = it->material_offsets[matidx];
            int end = it->material_offsets[matidx+1];

            if (!it->vd->MaterialIDs.empty())
                material = it->vd->MaterialIDs[start];
            SetMaterial(material);
	    
            if (end < 0) {
                end = it->vd->Positions.size();
                matidx = 0xff; //force exit
            }

            glGetError();
            glDrawArrays(GL_TRIANGLES, start, end-start);

            GLenum err = glGetError();
            if (err != GL_NO_ERROR) {
                Log::GetLog()->Fatal("OpenGL error %#x", err);
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
    for (auto v : _vertices) {
	if (v.vd->animationData) {
	    /* Has animation things */
	    UpdateVertexData(v.vbo_pos, v.vd->animationData->GetVertexRawData(),
			     v.vd->Positions.size());
	    //printf("Updated mesh %d to frame %d", v.vao, v.vd->animationData->GetActualFrame());
	}
    }
}

SceneManager* Renderer::GetSceneManager() const
{
    return this->_scenemng;
}

void Renderer::SetSceneManager(SceneManager* scenemng)
{
    this->_scenemng = scenemng;
}


/* Add vertex data structure. Returns its VAO ID */
GLint Renderer::AddVertexData(VertexData* v, glm::mat4* worldMatrix)
{
    /*  We transmit the worldMatrix as a pointer to allows you to
        change it without shit like Get... or Set... functions.

        Well, this is a fucking game engine. We need speed
    */

    VertexRenderInfo vri;
    vri.worldMat = worldMatrix;
    vri.vd = v;

    glGenVertexArrays(1, &vri.vao);
    glBindVertexArray(vri.vao);

    glGenBuffers(1, &vri.vbo_pos);
    glBindBuffer(GL_ARRAY_BUFFER, vri.vbo_pos);
    glBufferData(GL_ARRAY_BUFFER, v->Positions.size() * sizeof(glm::vec3),
		 v->animationData ? v->animationData->GetVertexRawData() : v->Positions.data(),
		 v->animationData ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vri.vbo_norm);
    glBindBuffer(GL_ARRAY_BUFFER, vri.vbo_norm);
    glBufferData(GL_ARRAY_BUFFER, v->Normals.size() * sizeof(glm::vec3),
		 v->Normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    
    glGenBuffers(1, &vri.vbo_tex);
    glBindBuffer(GL_ARRAY_BUFFER, vri.vbo_tex);
    glBufferData(GL_ARRAY_BUFFER, v->TexCoords.size() * sizeof(glm::vec2),
		 v->TexCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    /* Log::GetLog()->Write("Added vertices with VAO %d (VBO %d)", vri.vao,
        vri.vbo_pos); */

    /* Store material vertex starts */
    int matidx = 0;
    int actualidx = -1;
    int i = 0;
    for (auto matit = v->MaterialIDs.begin();
	 matit != v->MaterialIDs.end();
	 matit++) {
        if (*matit != actualidx) {
            actualidx = *matit;
            vri.material_offsets[matidx++] = i;
        }
        i++;
    }

    vri.material_offsets[matidx] = -1;
    v->vbo_pos = vri.vbo_pos;
	
    _vertices.push_back(vri);
    return vri.vao;
}

void Renderer::UpdateVertexData(int vbo, glm::vec3* data, size_t count)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(glm::vec3), (void*)data);
}

void Renderer::RemoveVertexData(GLuint vaoid)
{
    VertexRenderInfo vri;
    for (auto it = _vertices.begin(); it != _vertices.end(); it++){
        if (it->vao == vaoid) {
			if (it->vao_bbox) {
				RemoveBoundingBox(it->vao_bbox);
			}

            glDeleteVertexArrays(1, &vaoid);
            glDeleteBuffers(1, &it->vbo_pos);
			glDeleteBuffers(1, &it->vbo_norm);
			glDeleteBuffers(1, &it->vbo_tex);
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
			_scenemng->GetCamera()->GetProjectionMatrix() *_scenemng->GetCamera()->GetViewMatrix() * *it->worldMat);

		glBindVertexArray(it->vao);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, it->vbo_pos);
		glVertexAttribPointer(
			0,                  // attribute 0 (positions)
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, it->vbo_norm);	// use normal space for colors
		glVertexAttribPointer(
			1,                  // attribute 1 (colors)
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glGetError();
		glDrawArrays(GL_LINE_STRIP, 0, it->vd->Positions.size());

		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			Log::GetLog()->Fatal("OpenGL error %#x", err);
		}
	}
}

int Renderer::AddBoundingBox(Mesh* m, glm::vec3 color)
{
	VertexRenderInfo vri;
	VertexRenderInfo* vri_mesh = nullptr;

	/* Find the original mesh in our list */
	for (auto& vv : _vertices) {
		if (vv.vd->meshptr == (uintptr_t)m) {
			vri_mesh = &vv;
			break;
		}
	}

	/* Generate bb points */

	m->GenerateBoundingBox();
	BoundingBox bb = m->GetBoundingBox();
	vri.vd = new VertexData{};
	vri.vd->Positions.push_back(glm::vec3(bb.minX, bb.minY, bb.minZ));
	vri.vd->Positions.push_back(glm::vec3(bb.minX, bb.maxY, bb.minZ));
	vri.vd->Positions.push_back(glm::vec3(bb.maxX, bb.maxY, bb.minZ));
	vri.vd->Positions.push_back(glm::vec3(bb.minX, bb.minY, bb.minZ));
	vri.vd->Positions.push_back(glm::vec3(bb.maxX, bb.minY, bb.minZ));
	vri.vd->Positions.push_back(glm::vec3(bb.maxX, bb.maxY, bb.minZ));

	vri.vd->Positions.push_back(glm::vec3(bb.minX, bb.maxY, bb.minZ));
	vri.vd->Positions.push_back(glm::vec3(bb.minX, bb.maxY, bb.maxZ));
	vri.vd->Positions.push_back(glm::vec3(bb.maxX, bb.maxY, bb.maxZ));
	vri.vd->Positions.push_back(glm::vec3(bb.minX, bb.maxY, bb.minZ));
	vri.vd->Positions.push_back(glm::vec3(bb.maxX, bb.maxY, bb.minZ));
	vri.vd->Positions.push_back(glm::vec3(bb.maxX, bb.maxY, bb.maxZ));

	vri.vd->Positions.push_back(glm::vec3(bb.minX, bb.minY, bb.maxZ));
	vri.vd->Positions.push_back(glm::vec3(bb.minX, bb.maxY, bb.maxZ));
	vri.vd->Positions.push_back(glm::vec3(bb.maxX, bb.maxY, bb.maxZ));
	vri.vd->Positions.push_back(glm::vec3(bb.minX, bb.minY, bb.maxZ));
	vri.vd->Positions.push_back(glm::vec3(bb.maxX, bb.minY, bb.maxZ));
	vri.vd->Positions.push_back(glm::vec3(bb.maxX, bb.maxY, bb.maxZ));

	for (size_t i = 0; i < vri.vd->Positions.size(); i++) {
		// Use normals for color, so we can use the same struct.
		vri.vd->Normals.push_back(color);
	}

	vri.worldMat = m->GetModelMatrixPointer();

	glGenVertexArrays(1, &vri.vao);
	glBindVertexArray(vri.vao);

	glGenBuffers(1, &vri.vbo_pos);
	glBindBuffer(GL_ARRAY_BUFFER, vri.vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, vri.vd->Positions.size() * sizeof(glm::vec3),
		vri.vd->Positions.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &vri.vbo_norm);
	glBindBuffer(GL_ARRAY_BUFFER, vri.vbo_norm);
	glBufferData(GL_ARRAY_BUFFER, vri.vd->Normals.size() * sizeof(glm::vec3),
		vri.vd->Normals.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	Log::GetLog()->Write("Added bounding box of mesh vao %d with VAO %d (VBO %d)", 
		vri_mesh ? vri_mesh->vao : -1, vri.vao, vri.vbo_pos);

	if (vri_mesh) {
		vri_mesh->vao_bbox = vri.vao;
	}

	_bb_vaos.push_back(vri);
	return vri.vao;
}

void Renderer::RemoveBoundingBox(GLuint vao)
{
    VertexRenderInfo vri;
    for (auto it = _bb_vaos.begin(); it != _bb_vaos.end(); it++) {
	if (it->vao == vao) {
	    glDeleteVertexArrays(1, &vao);
	    glDeleteBuffers(1, &it->vbo_pos);
	    glDeleteBuffers(1, &it->vbo_norm);
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
