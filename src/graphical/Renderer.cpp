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
    srand(time(NULL));
    float cx, cy, cz;
    cx = (rand() % 255) / 255.0f;
    cy = (rand() % 255) / 255.0f;
    cz = (rand() % 255) / 255.0f;
    sForward->SetUniform("color", glm::vec3(cx, cy, cz));

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
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    _win = SDL_CreateWindow("Tribalia", 0, 0, 640, 480,
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

}

void Renderer::InitializeShaders()
{
    Shader *sFrag, *sVert;

    sFrag = new Shader{"shaders/Forward.frag", SHADER_PIXEL};
    sVert = new Shader{"shaders/Forward.vert", SHADER_VERTEX};


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

}

/* Returns true if rendered successfully */
bool Renderer::Render()
{
    glm::mat4 mModel, mView, mProj;
    mView = this->_scenemng->GetCamera()->GetViewMatrix();
    mProj = this->_scenemng->GetCamera()->GetProjectionMatrix();
    mModel = glm::mat4(1.0f);

    sForward->SetUniform("mvp", mProj * mView * mModel);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(vao_tri);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_tri);
    glVertexAttribPointer(
       0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
       3,                  // size
       GL_FLOAT,           // type
       GL_FALSE,           // normalized?
       0,                  // stride
       (void*)0            // array buffer offset
    );
        // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
    glDisableVertexAttribArray(0);


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

    GLuint vbo_pos;

    glGenVertexArrays(1, &vri.vao);
    glBindVertexArray(vri.vao);

    glGenBuffers(1, &vbo_pos);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
    glBufferData(GL_ARRAY_BUFFER, v->Positions.size(), v->Positions.data(),
        GL_STATIC_DRAW);

    glBindVertexArray(0);

    return vri.vao;
}

Renderer::~Renderer()
{
    SDL_Quit();
}
