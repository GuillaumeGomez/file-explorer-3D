#include "SkyBox.hpp"
#include "../MyWindow.hpp"
#include "../HandleError.hpp"
#include "../String_utils.hpp"
#include "../Camera.hpp"
#include <GL/glew.h>

#include <map>
#include <string>

using namespace std;
using namespace Object;

SkyBox::SkyBox(float size) : Cube(Vector3D(0.f, 0.f, 0.f), Rotation(), Color(), size)
{
    m_hasTexture = true;
    m_shader = new Shader;
    m_pointsNumber = 0;

    /*textures[0] = HandleSDL::loadTexture("./textures/skybox/west.jpg", true, true);
  textures[1] = HandleSDL::loadTexture("./textures/skybox/sky.jpg", true, true);
  textures[2] = HandleSDL::loadTexture("./textures/skybox/east.jpg", true, true);
  textures[3] = HandleSDL::loadTexture("./textures/skybox/ground.jpg", true, true);
  textures[4] = HandleSDL::loadTexture("./textures/skybox/north.jpg", true, true);
  textures[5] = HandleSDL::loadTexture("./textures/skybox/south.jpg", true, true);*/

    glGenTextures(1, &cube_map_texture_ID);

    glEnable(GL_TEXTURE_CUBE_MAP_ARB);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, cube_map_texture_ID);

#ifdef WIN32
    std::string tex[] = {"./textures/skybox/west.jpg", "./textures/skybox/east.jpg", "./textures/skybox/ground.jpg",
                         "./textures/skybox/sky.jpg", "./textures/skybox/north.jpg", "./textures/skybox/south.jpg"};
#else
    std::string tex[] = {"/windows/Users/imperio/Projects/My_World_SDL/My_World_SDL/My_World_SDL/textures/skybox/west.jpg",
                         "/windows/Users/imperio/Projects/My_World_SDL/My_World_SDL/My_World_SDL/textures/skybox/east.jpg",
                         "/windows/Users/imperio/Projects/My_World_SDL/My_World_SDL/My_World_SDL/textures/skybox/ground.jpg",
                         "/windows/Users/imperio/Projects/My_World_SDL/My_World_SDL/My_World_SDL/textures/skybox/sky.jpg",
                         "/windows/Users/imperio/Projects/My_World_SDL/My_World_SDL/My_World_SDL/textures/skybox/north.jpg",
                         "/windows/Users/imperio/Projects/My_World_SDL/My_World_SDL/My_World_SDL/textures/skybox/south.jpg"};
#endif
    MyWindow::createSkyBoxTextures(tex);
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
}

SkyBox::~SkyBox()
{
    /*for (int i = 0; i < 6; ++i)
    glDeleteTextures(1, &textures[i]);*/
    glDeleteTextures(1, &cube_map_texture_ID);
}

void  SkyBox::initializeGL()
{
    std::string vert;
    std::string frag;

    vert =
            "#version 330\n"

            "in vec3 vertor;\n"
            "out vec3 texCoord;\n"
            "uniform mat4 PVM;\n"

            "void main(){\n"
            "gl_Position = PVM * vec4(vertor, 1.0);\n"
            "texCoord = vertor;\n"
            "}";

    frag =
            "#version 330\n"

            "in vec3 texCoord;\n"
            "out vec4 fragColor;\n"
            "uniform samplerCube cubemap;\n"

            "void main (void){\n"
            "fragColor = texture(cubemap, texCoord);\n"
            "};";
    m_shader->setVertexSource(vert);
    m_shader->setFragmentSource(frag);
    if (!m_shader->load()){
        HandleError::showError("Shader didn't load in Skybox");
        exit(-1);
    }

    GLfloat tmpVertices[] = {
        -1.f,-1.f,-1.f, -1.f,1.f,-1.f, -1.f,-1.f,1.f, -1.f,1.f,1.f,
        1.f,-1.f,-1.f, 1.f,-1.f,1.f, 1.f,1.f,-1.f, 1.f,1.f,1.f,
        -1.f,-1.f,-1.f, -1.f,-1.f,1.f, 1.f,-1.f,-1.f, 1.f,-1.f,1.f,
        -1.f,1.f,-1.f, 1.f,1.f,-1.f, -1.f,1.f,1.f, 1.f,1.f,1.f,
        -1.f,-1.f,-1.f, 1.f,-1.f,-1.f, -1.f,1.f,-1.f, 1.f,1.f,-1.f,
        -1.f,-1.f,1.f, -1.f,1.f,1.f, 1.f,-1.f,1.f, 1.f,1.f,1.f};

    int max = sizeof(tmpVertices) / sizeof(tmpVertices[0]);

    for (int i(0); i < max; ++i){
        m_vertices.push_back(tmpVertices[i]);
        m_textures.push_back(tmpVertices[i]);
    }
    m_pointsNumber = m_vertices.size() / 3;

    PVM = glGetUniformLocation(m_shader->getProgramID(), "PVM");
    vertex = glGetAttribLocation(m_shader->getProgramID(), "vertor");

    this->initVertexBufferObject();
    this->initVertexArrayObject();
}

void  SkyBox::initVertexBufferObject()
{
    m_verticesSize = m_vertices.size() * sizeof(GLfloat);
    m_texturesSize = m_textures.size() * sizeof(GLfloat);

    glGenBuffers(1, &m_vboID);
    //verrouillage de l'objet
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

    // Allocation de la memoire video
    glBufferData(GL_ARRAY_BUFFER, m_verticesSize + m_texturesSize, 0, GL_STATIC_DRAW);

    // Transfert des donnees
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_verticesSize, &m_vertices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, m_verticesSize, m_texturesSize, &m_textures[0]);

    glEnableVertexAttribArray(vertex);
    glVertexAttribPointer(vertex, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Deverrouillage de l'objet
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void  SkyBox::bindVertexBufferObject()
{
    // Verrouillage du VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

    // Acces aux vertices dans la memoire video
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);

    //envoi des coordonnees des textures
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(m_verticesSize));
    glEnableVertexAttribArray(2);
    // Deverrouillage du VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void  SkyBox::initializeGLNoList()
{
}

void  SkyBox::paintGL(float &x, float &y)
{
    glUseProgram(m_shader->getProgramID());
    glDepthMask(GL_FALSE);
    glBindVertexArray(m_vaoID);

    glm::mat4 RotateX = glm::rotate(glm::mat4(1.0f), x, glm::vec3(-1.0f, 0.0f, 0.0f));
    glm::mat4 RotateY = glm::rotate(RotateX, y, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 M = Camera::getProjectionMatrix() * RotateY;

    glUniformMatrix4fv(PVM, 1, GL_FALSE, glm::value_ptr(M));

    glEnable(GL_TEXTURE_CUBE_MAP_ARB);
    m_texture.bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, m_pointsNumber);
    m_texture.unbind();
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);

    glDepthMask(GL_TRUE);

    glBindVertexArray(0);
    glUseProgram(0);
}
