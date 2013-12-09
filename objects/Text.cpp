#include "Text.hpp"
#include "../MyWindow.hpp"
#include "../String_utils.hpp"
#include "../HandleError.hpp"
#include <cmath>

#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

using namespace Object;

Text::Text(string text, Color c, float x, float y, float font_size)
    : myGLWidget(Vector3D(x, y, 0.f), Rotation(), c), m_text(text), m_font_size(font_size)
{
    GLuint  tex(0);

    if (m_text.length() > 0) {
        MyWindow::createTextTexture(m_text.c_str(), &tex, 0, c);
        this->setTexture(tex);
    } else {
        m_hasTexture = true;
    }
    //m_texture.setTextureID(tex);

    m_size = 0.017f * m_text.length();

    m_shader = new Shader;
    m_render2D = true;
}

Text::Text(const char *text, Color c, float x, float y, float font_size)
    : myGLWidget(Vector3D(x, y, 0.f), Rotation(), c), m_text(text ? text : ""), m_font_size(font_size)
{
    GLuint  tex(0);

    if (m_text.length() > 0) {
        MyWindow::createTextTexture(m_text.c_str(), &tex, 0, c);
        this->setTexture(tex);
    } else {
        m_hasTexture = true;
    }
    //m_texture.setTextureID(tex);

    m_size = 0.017f * m_text.length();

    m_shader = new Shader;
    m_render2D = true;
}

Text::Text(string text, Color c, Vector3D v, Rotation r, float font_size)
    : myGLWidget(v, r, c), m_text(text), m_font_size(font_size)
{
    GLuint  tex(0);

    if (m_text.length() > 0) {
        MyWindow::createTextTexture(m_text.c_str(), &tex, 0, c);
        this->setTexture(tex);
    } else {
        m_hasTexture = true;
    }
    //m_texture.setTextureID(tex);

    m_size = 5 * m_text.length();

    m_shader = new Shader;
}

Text::Text(const char *text, Color c, Vector3D v, Rotation r, float font_size)
    : myGLWidget(v, r, c), m_text(text ? text : ""), m_font_size(font_size)
{
    GLuint  tex(0);

    if (m_text.length() > 0) {
        MyWindow::createTextTexture(m_text.c_str(), &tex, 0, c);
        this->setTexture(tex);
    } else {
        m_hasTexture = true;
    }
    //m_texture.setTextureID(tex);

    m_size = 5 * m_text.length();

    m_shader = new Shader;
}


Text::Text(GLuint tex_id, float text_length, Vector3D v, Rotation r, float font_size)
    : myGLWidget(v, r, Color()), m_text(""), m_font_size(font_size)
{
    this->setTexture(tex_id);
    //m_texture.setTextureID(tex_id);

    m_size = text_length;

    m_shader = new Shader;
}

Text::~Text()
{
}

GLfloat Text::getSize() const
{
    return m_size;
}

void    Text::initializeGL()
{
    std::string vert;
    std::string frag;

    if (m_render2D) {
        vert =
                "#version 150 core\n"

                "in vec2 in_Vertex;\n"
                "in vec2 in_TexCoord0;\n"
                "out vec2 coordTexture;\n"

                "uniform mat4 projection;\n"

                "void main(){\n"
                "gl_Position.xy = in_Vertex;\n"
                "coordTexture = in_TexCoord0;\n"
                "}";
        frag =
                "#version 150 core\n"

                "in vec2 coordTexture;\n"
                "uniform sampler2D tex;\n"
                "out vec4 out_Color;\n"

                "void main(){\n"
                "out_Color = texture(tex, coordTexture);\n"
                "}\n";
    } else {
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
    }

    m_shader->setVertexSource(vert);
    m_shader->setFragmentSource(frag);
    if (!m_shader->load()){
        HandleError::showError("Shader didn't load in Text");
        exit(-1);
    }
    m_uniLoc_projection = glGetUniformLocation(m_shader->getProgramID(), "projection");
    m_uniLoc_modelView = glGetUniformLocation(m_shader->getProgramID(), "modelview");

    if (m_render2D) {
        GLfloat verticesTmp[] = {m_pos.x(), m_font_size + m_pos.y(),
                                 m_size + m_pos.x(), m_font_size + m_pos.y(),
                                 m_pos.x(), m_pos.y(),
                                 m_size + m_pos.x(), m_pos.y()};
        for (unsigned int i(0); i < sizeof(verticesTmp) / sizeof(verticesTmp[0]); ++i){
            m_vertices.push_back(verticesTmp[i]);
        }
    } else {
        GLfloat verticesTmp[] = {
            m_size / -2.f, m_font_size / 2.f, 0.f,
            m_size / 2.f, m_font_size / 2.f, 0.f,
            m_size / -2.f, m_font_size / -2.f, 0.f,
            m_size / 2.f, m_font_size / -2.f, 0.f
        };
        for (unsigned int i(0); i < sizeof(verticesTmp) / sizeof(verticesTmp[0]); ++i){
            m_vertices.push_back(verticesTmp[i]);
        }
    }
    GLfloat coordTextureTmp[] = {0.f, 0.f,
                                 1.f, 0.f,
                                 0.f, 1.f,
                                 1.f, 1.f};

    for (unsigned int i(0); i < sizeof(coordTextureTmp) / sizeof(coordTextureTmp[0]); ++i){
        m_textures.push_back(coordTextureTmp[i]);
    }
    if (!m_render2D)
        m_pointsNumber = m_vertices.size() / 3;
    else
        m_pointsNumber = m_vertices.size() / 2;

    this->initVertexBufferObject(GL_DYNAMIC_DRAW);
    this->initVertexArrayObject();
}

void    Text::paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix)
{
    // Activation du shader
    glUseProgram(m_shader->getProgramID());

    glBindVertexArray(m_vaoID);

    // Envoi des matrices
    if (!m_render2D) {
        glUniformMatrix4fv(m_uniLoc_projection, 1, GL_FALSE, glm::value_ptr(proj_matrix));
        glm::mat4 tmp = glm::translate(view_matrix, glm::vec3(m_pos.x(), m_pos.y(), m_pos.z()));

        glUniformMatrix4fv(m_uniLoc_modelView, 1, GL_FALSE, glm::value_ptr(tmp));
    } else {

    }

    m_texture.bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, m_pointsNumber);
    m_texture.unbind();

    glBindVertexArray(0);
    glUseProgram(0);
}

void    Text::initializeGLNoList()
{
}

void    Text::setText(const char *s)
{
    if (s && s != m_text) {
        GLuint tex(0);

        m_texture.destroy();
        MyWindow::createTextTexture(s, &tex, 0, this->getColor());
        this->setTexture(tex);
        m_text = s;
        //m_texture.setTextureID(tex);

        m_size = 0.017f * m_text.length();
        GLfloat verticesTmp[] = {m_pos.x(), m_font_size + m_pos.y(),
                                 m_size + m_pos.x(), m_font_size + m_pos.y(),
                                 m_pos.x(), m_pos.y(),
                                 m_size + m_pos.x(), m_pos.y()};

        m_vertices.clear();
        unsigned int size(sizeof(verticesTmp) / sizeof(verticesTmp[0]));

        for (unsigned int i(0); i < size; ++i){
            m_vertices.push_back(verticesTmp[i]);
        }
        this->updateVertexBufferObject(&m_vertices[0], m_vertices.size() * sizeof(m_vertices[0]), 0);
    }
}

void    Text::setText(std::string s)
{
    this->setText(s.c_str());
}
