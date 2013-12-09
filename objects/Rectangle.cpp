#include "Rectangle.hpp"
#include <cstring>
#include <cmath>

#include "../String_utils.hpp"
#include "../HandleSDL.hpp"
#include "../HandleError.hpp"
#include "../Camera.hpp"

using namespace Object;

Rectangle::Rectangle(Vector3D p, Rotation r, Color c, float sizex, float sizey, float sizez)
    : myGLWidget(p, r, c), sizeX(sizex), sizeY(sizey), sizeZ(sizez)
{
    m_shader = new Shader;
}

Rectangle::Rectangle(Vector3D p, Rotation r, std::string tex, float sizex, float sizey, float sizez)
    : myGLWidget(p, r, tex), sizeX(sizex), sizeY(sizey), sizeZ(sizez)
{
    m_shader = new Shader;
}

void Rectangle::initializeGL()
{
    loadTexture();

    std::string vert;
    std::string frag;

    if (m_hasTexture) {
        vert =
                "#version 150 core\n"

                "in vec3 in_Vertex;\n"
                "in vec2 in_TexCoord0;\n"

                "uniform mat4 projection;\n"
                "uniform mat4 modelview;\n"

                "out vec2 coordTexture;\n"

                "void main()\n"
                "{\n"
                "gl_Position = projection * modelview * vec4(in_Vertex, 1.0);\n"
                "coordTexture = in_TexCoord0;\n"
                "}";

        frag =
                "#version 150 core\n"

                "in vec2 coordTexture;\n"

                "uniform sampler2D tex;\n"

                "out vec4 out_Color;\n"

                "void main()\n"
                "{\n"
                "out_Color = texture(tex, coordTexture);\n"
                "}";
    } else {
        vert = "#version 150 core\n"
                "in vec3 in_Vertex;\n"
                "in vec3 in_Color;\n"

                "uniform mat4 projection;\n"
                "uniform mat4 modelview;\n"

                "out vec3 color;\n"
                "void main()\n"
                "{\n"
                "gl_Position = projection * modelview * vec4(in_Vertex, 1.0);\n"
                "color = in_Color;\n"
                "}";
        frag =
                "#version 150 core\n"

                "in vec3 color;\n"

                "out vec4 out_Color;\n"
                "void main()\n"
                "{\n"
                "out_Color = vec4(color, 1.0);\n"
                "}";
    }

    m_shader->setVertexSource(vert);
    m_shader->setFragmentSource(frag);
    if (!m_shader->load()){
        HandleError::showError("Shader didn't load");
        exit(-1);
    }
    m_uniLoc_projection = glGetUniformLocation(m_shader->getProgramID(), "projection");
    m_uniLoc_modelView = glGetUniformLocation(m_shader->getProgramID(), "modelview");

    sizeX /= 2.f;
    sizeY /= 2.f;
    sizeZ /= 2.f;

    GLfloat verticesTmp[] = {-sizeX, -sizeY, -sizeZ,
                             sizeX, -sizeY, -sizeZ,
                             sizeX, sizeY, -sizeZ,
                             sizeX, sizeY, -sizeZ,
                             -sizeX, sizeY, -sizeZ,
                             -sizeX, -sizeY, -sizeZ,

                             -sizeX, -sizeY, sizeZ,
                             sizeX, -sizeY, sizeZ,
                             sizeX, sizeY, sizeZ,
                             sizeX, sizeY, sizeZ,
                             -sizeX, sizeY, sizeZ,
                             -sizeX, -sizeY, sizeZ,

                             -sizeX, sizeY, sizeZ,
                             -sizeX, sizeY, -sizeZ,
                             -sizeX, -sizeY, -sizeZ,
                             -sizeX, -sizeY, -sizeZ,
                             -sizeX, -sizeY, sizeZ,
                             -sizeX, sizeY, sizeZ,

                             sizeX, sizeY, sizeZ,
                             sizeX, sizeY, -sizeZ,
                             sizeX, -sizeY, -sizeZ,
                             sizeX, -sizeY, -sizeZ,
                             sizeX, -sizeY, sizeZ,
                             sizeX, sizeY, sizeZ,

                             -sizeX, -sizeY, -sizeZ,
                             sizeX, -sizeY, -sizeZ,
                             sizeX, -sizeY, sizeZ,
                             sizeX, -sizeY, sizeZ,
                             -sizeX, -sizeY, sizeZ,
                             -sizeX, -sizeY, -sizeZ,

                             -sizeX, sizeY, -sizeZ,
                             sizeX, sizeY, -sizeZ,
                             sizeX, sizeY, sizeZ,
                             sizeX, sizeY, sizeZ,
                             -sizeX, sizeY, sizeZ,
                             -sizeX, sizeY, -sizeZ
                            };

    for (unsigned int i(0); i < sizeof(verticesTmp) / sizeof(verticesTmp[0]); ++i){
        m_vertices.push_back(verticesTmp[i]);
    }

    if (!m_hasTexture){
        /*GLfloat couleursTmp[] = {m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),
                                 m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),
                                 m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),

                                 m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),
                                 m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),
                                 m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),

                                 m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),
                                 m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),
                                 m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),

                                 m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),
                                 m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),
                                 m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),

                                 m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),
                                 m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),
                                 m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),

                                 m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),
                                 m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),
                                 m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue()};*/

        unsigned int size = m_vertices.size() / 3;
        for (unsigned int i(0); i < size; ++i){
            m_couleurs.push_back(m_color.red());
            m_couleurs.push_back(m_color.green());
            m_couleurs.push_back(m_color.blue());
        }
    }

    else{
        GLfloat coordTextureTmp[] = {0.0f, 0.0f,
                                     1.0f, 0.0f,
                                     1.0f, 1.0f,
                                     1.0f, 1.0f,
                                     0.0f, 1.0f,
                                     0.0f, 0.0f,

                                     0.0f, 0.0f,
                                     1.0f, 0.0f,
                                     1.0f, 1.0f,
                                     1.0f, 1.0f,
                                     0.0f, 1.0f,
                                     0.0f, 0.0f,

                                     0.0f, 1.0f,
                                     1.0f, 1.0f,
                                     1.0f, 0.0f,
                                     1.0f, 0.0f,
                                     0.0f, 0.0f,
                                     0.0f, 1.0f,

                                     0.0f, 1.0f,
                                     1.0f, 1.0f,
                                     1.0f, 0.0f,
                                     1.0f, 0.0f,
                                     0.0f, 0.0f,
                                     0.0f, 1.0f,

                                     0.0f, 1.0f,
                                     1.0f, 1.0f,
                                     1.0f, 0.0f,
                                     1.0f, 0.0f,
                                     0.0f, 0.0f,
                                     0.0f, 1.0f,

                                     0.0f, 1.0f,
                                     1.0f, 1.0f,
                                     1.0f, 0.0f,
                                     1.0f, 0.0f,
                                     0.0f, 0.0f,
                                     0.0f, 1.0f
                                    };

        for (unsigned int i(0); i < sizeof(coordTextureTmp) / sizeof(coordTextureTmp[0]); ++i){
            m_textures.push_back(coordTextureTmp[i]);
        }
    }

    m_pointsNumber = m_vertices.size() / 3;

    this->initVertexBufferObject();
    this->initVertexArrayObject();
}

void  Rectangle::paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix)
{
    // Activation du shader
    glUseProgram(m_shader->getProgramID());

    glBindVertexArray(m_vaoID);

    // Envoi des matrices
    glUniformMatrix4fv(m_uniLoc_projection, 1, GL_FALSE, glm::value_ptr(proj_matrix));

    glm::mat4 tmp = glm::translate(view_matrix, glm::vec3(m_pos.x(), m_pos.y(), m_pos.z()));

    if (m_rot.getRotation() != 0.f && (m_rot.getRotX() != 0.f || m_rot.getRotY() != 0.f || m_rot.getRotZ() != 0.f))
        tmp = glm::rotate(tmp, m_rot.getRotation(), glm::vec3(m_rot.getRotX(), m_rot.getRotY(), m_rot.getRotZ()));
    glUniformMatrix4fv(m_uniLoc_modelView, 1, GL_FALSE, glm::value_ptr(tmp));

    // Rendu
    if (m_hasTexture){
        m_texture.bind();
        glDrawArrays(GL_TRIANGLES, 0, m_pointsNumber);
        m_texture.unbind();
    }
    else{
        glDrawArrays(GL_TRIANGLES, 0, m_pointsNumber);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}
