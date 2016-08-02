#include "Renderer.hpp"

using namespace Tribalia::Graphics;

GLuint vao_tri, vbo_tri;


Renderer::Renderer()
{
    InitializeLibraries();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    /*** TEST: Draw a triangle ***/
    glGenVertexArrays(1, &vao_tri);
    glBindVertexArray(vao_tri);

    glGenBuffers(1, &vbo_tri);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_tri);

    static const GLfloat triangle_data[] =
        {
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 0.0f,
            0.0f,  1.0f, 0.0f,
        };

    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_data), triangle_data,
        GL_STATIC_DRAW);

    glBindVertexArray(0);
    /*** END TEST ***/

    InitializeShaders();
    sForward->Use();

	/* 	Create a fake texture, so we can render something
		if textures aren't available */

	unsigned int* fake_color = new unsigned int;
	*fake_color = 0xff00ff00;
	fake_tex = new Texture(1, 1, GL_RGB, fake_color);

}

void Renderer::InitializeLibraries()
{
    int sdl_ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    if (sdl_ret != 0) {
        /* Video hasn't been initialized correctly */
        char err[1024] = "SDL wasn't initialized correctly: ";
        strcat(err, SDL_GetError());
        throw renderer_exception(err, sdl_ret);
    }

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,    8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,  8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,   8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,  16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    _win = SDL_CreateWindow("Tribalia", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480,
        SDL_WINDOW_OPENGL);
    if (!_win) {
        char err[1024] = "Window creation error: ";
        strcat(err, SDL_GetError());
        SDL_Quit();
        throw renderer_exception(err, -10);
    }

    _glctxt = SDL_GL_CreateContext(_win);

    if (_glctxt == NULL) {
        char err[1024] = "OpenGL context creation error: ";
        strcat(err, SDL_GetError());
        SDL_Quit();
        throw renderer_exception(err, -11);
    }

    GLenum glewStatus = glewInit();

    if (glewStatus != GLEW_OK) {
        char err[1024] = "GLEW initialization error: ";
        strcat(err, (char*)glewGetErrorString(glewStatus));
        SDL_Quit();
        throw renderer_exception(err, glewStatus);
    }

    //Enable depth test
    glEnable(GL_DEPTH_TEST);

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

void Renderer::SetMaterial(int ID)
{
    Material* m = MaterialManager::GetInstance()->GetMaterial(ID);
    if (!m) {
        printf("Cannot found mat id %d\n", ID);
        return;
    }

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
};

int lastCheck = 0;
std::vector<SceneIDCache> _last_IDs;

void Renderer::CheckUpdatedObjects()
{
	lastCheck++;

	/* Check updates from SceneManager*/
	if (_scenemng->UpdateValidObjects()) {

		auto objList = _scenemng->GetValidObjects();

		/* Check for inserted objects */
		for (auto itScene = objList->begin(); itScene != objList->end(); itScene++) {
			bool objExists = false;
			for (auto it2 = _last_IDs.begin(); it2 != _last_IDs.end(); it2++) {
				if ((*itScene)->GetID() == it2->ID) {
					it2->lastcheck = lastCheck;
					objExists = true;
					break;
				}
			}

			if (objExists) continue;
			/* Draw the added object */
			Mesh* mes = (Mesh*)(*itScene);
			if ((*itScene)->GetType() != SCENE_MESH) {
				Log::GetLog()->Warning("Not a mesh, but mesh is the only "
					"type of scene object we have now! Skipping...");
				continue;
			}

			/* Object doesn't exist on renderer */
			Log::GetLog()->Write("Renderer added object '%s' (id %d)",
				(*itScene)->GetName(), (*itScene)->GetID());
			mes->ApplyTransformations();

			int vaon = this->AddVertexData(mes->GetVertexData(),
				mes->GetModelMatrixPointer());

			SceneIDCache sidc;
			sidc.ID = (*itScene)->GetID();
			sidc.lastcheck = lastCheck;
			sidc.vao = vaon;

			_last_IDs.push_back(sidc);
			AddBoundingBox(mes, glm::vec3(1, 0, 0));
		}

		/* Check for deleted objects */
		int deleted_num = 0, di = 0;
	deleted_check:
		for (auto itScene = objList->begin(); itScene != objList->end(); itScene++) {
			bool isDeleted = true;
			SceneIDCache* sic;
			for (auto it2 = _last_IDs.begin() + deleted_num; it2 != _last_IDs.end(); ++it2) {
				if (it2->ID == (*itScene)->GetID()) {
					isDeleted = false;
					sic = &(*it2);
					break;
				}
				di++;
			}

			if (isDeleted) {
				Log::GetLog()->Write("Removing object ID %d from the cache",
					(*itScene)->GetID());
				this->RemoveVertexData(sic->vao);
				deleted_num = di;
				_last_IDs.erase(_last_IDs.begin() + di);

				if (_last_IDs.empty())
					break;
			}

		}

	}
}


bool Renderer::Render() 
{
	this->CheckUpdatedObjects();
    glm::mat4 mModel, mView, mProj;
    mView = this->_scenemng->GetCamera()->GetViewMatrix();
    mProj = this->_scenemng->GetCamera()->GetProjectionMatrix();

    sForward->Use();
    sForward->SetUniform("mView", mView);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int material = 0;

    /* Render registered VAOs */
    for (auto it = _vertices.begin(); it != _vertices.end(); ++it) {
        mModel = *it->worldMat;
        sForward->SetUniform("mvp", mProj * mView * mModel);
        sForward->SetUniform("mModel", mModel);

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

    SDL_GL_SwapWindow(_win);
    return true;
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
        v->Positions.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &vri.vbo_norm);
    glBindBuffer(GL_ARRAY_BUFFER, vri.vbo_norm);
    glBufferData(GL_ARRAY_BUFFER, v->Normals.size() * sizeof(glm::vec3),
        v->Normals.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &vri.vbo_tex);
	glBindBuffer(GL_ARRAY_BUFFER, vri.vbo_tex);
	glBufferData(GL_ARRAY_BUFFER, v->TexCoords.size() * sizeof(glm::vec2),
		v->TexCoords.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    Log::GetLog()->Write("Added vertices with VAO %d (VBO %d)", vri.vao,
        vri.vbo_pos);

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

    _vertices.push_back(vri);
    return vri.vao;
}

void Renderer::RemoveVertexData(GLuint vaoid)
{
    VertexRenderInfo vri;
    for (auto it = _vertices.begin(); it != _vertices.end(); it++){
        if (it->vao == vaoid) {
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

	for (int i = 0; i < vri.vd->Positions.size(); i++) {
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

	Log::GetLog()->Write("Added bounding box with VAO %d (VBO %d)", vri.vao,
		vri.vbo_pos);
	
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

void Renderer::GetWindowSize(int& width, int& height)
{
    SDL_GetWindowSize(this->_win, &width, &height);
}

void Renderer::SetBoundingBox(bool b) { renderBBs = b; }

Renderer::~Renderer()
{
    SDL_Quit();
}
