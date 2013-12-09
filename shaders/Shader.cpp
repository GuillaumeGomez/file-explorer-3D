#include "Shader.hpp"
#include <iostream>
#include <fstream>

#include "../HandleError.hpp"

Shader::Shader() : m_vertexID(0), m_fragmentID(0), m_programID(0), m_vertexSource(), m_fragmentSource()
{
}

Shader::Shader(Shader const &toCopy)
    : m_vertexID(0), m_fragmentID(0), m_programID(0), m_vertexSource(), m_fragmentSource()
{
    m_vertexSource = toCopy.m_vertexSource;
    m_fragmentSource = toCopy.m_fragmentSource;

    load();
}

Shader::Shader(std::string vertexSource, std::string fragmentSource)
    : m_vertexID(0), m_fragmentID(0), m_programID(0), m_vertexSource(vertexSource), m_fragmentSource(fragmentSource)
{
}

Shader::~Shader()
{
    glDeleteShader(m_vertexID);
    glDeleteShader(m_fragmentID);
    glDeleteProgram(m_programID);
}

Shader& Shader::operator=(Shader const &toCopy)
{
    m_vertexSource = toCopy.m_vertexSource;
    m_fragmentSource = toCopy.m_fragmentSource;

    load();
    return *this;
}

bool  Shader::load()
{
    if (glIsShader(m_vertexID) == GL_TRUE)
        glDeleteShader(m_vertexID);
    if (glIsShader(m_fragmentID) == GL_TRUE)
        glDeleteShader(m_fragmentID);
    if (glIsProgram(m_programID) == GL_TRUE)
        glDeleteProgram(m_programID);

    // Compilation des shaders
    if (m_vertexSource.find('\n') != std::string::npos){
        if (!compileShader(m_vertexID, GL_VERTEX_SHADER, m_vertexSource))
            return false;
    }
    else{
        if (!compileShaderFromFile(m_vertexID, GL_VERTEX_SHADER, m_vertexSource))
            return false;
    }
    if (m_fragmentSource.find('\n') != std::string::npos){
        if (!compileShader(m_fragmentID, GL_FRAGMENT_SHADER, m_fragmentSource))
            return false;
    }
    else{
        if (!compileShaderFromFile(m_fragmentID, GL_FRAGMENT_SHADER, m_fragmentSource))
            return false;
    }

    // Creation du programme
    m_programID = glCreateProgram();

    // Association des shaders
    glAttachShader(m_programID, m_vertexID);
    glAttachShader(m_programID, m_fragmentID);

    // Verrouillage des entrees shader
    glBindAttribLocation(m_programID, 0, "in_Vertex");
    glBindAttribLocation(m_programID, 1, "in_Color");
    glBindAttribLocation(m_programID, 2, "in_TexCoord0");

    // Linkage du programme
    glLinkProgram(m_programID);

    // Verification du linkage
    GLint erreurLink(0);
    glGetProgramiv(m_programID, GL_LINK_STATUS, &erreurLink);

    if (erreurLink != GL_TRUE)
    {
        std::string s = getError(m_programID);
        std::cout << s << std::endl;

        glDeleteProgram(m_programID);
        return false;
    }
    return true;
}

bool  Shader::compileShaderFromFile(GLuint &shader, GLenum type, const std::string &source)
{
    std::ifstream fd(source.c_str());
    int           length;
    char          *buffer;
    std::string   datas;

    if (!fd.is_open())
        return false;

    fd.seekg(0, fd.end);
    length = fd.tellg();
    fd.seekg(0, fd.beg);

    buffer = new char[length + 1];

    fd.read(buffer, length);
    fd.close();
    buffer[length] = 0;
    datas = buffer;
    delete[] buffer;
    return this->compileShader(shader, type, datas);
}

bool Shader::compileShader(GLuint &shader, GLenum type, std::string const &source)
{
    shader = glCreateShader(type);

    if (shader == 0)
    {
        HandleError::showError("Erreur, le type de shader n'existe pas");
        return false;
    }

    const GLchar* chaineCodeSource = source.c_str();
    // Envoi du code source au shader
    glShaderSource(shader, 1, &chaineCodeSource, 0);

    glCompileShader(shader);

    GLint erreurCompilation(0);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &erreurCompilation);

    if (erreurCompilation != GL_TRUE)
    {
        std::string err = getError(shader);
        if (type == GL_VERTEX_SHADER)
            HandleError::showError("vertex error: " + err);
        else
            HandleError::showError("fragment error: " + err);

        glDeleteShader(shader);
        return false;
    }
    return true;
}

std::string Shader::getError(GLuint t)
{
    std::string err;
    GLint errorSize(0);
    char *error;

    glGetShaderiv(t, GL_INFO_LOG_LENGTH, &errorSize);

    error = new char[errorSize + 1];
    glGetShaderInfoLog(t, errorSize, &errorSize, error);
    error[errorSize] = '\0';
    err = error;
    delete[] error;
    return err;
}

GLuint Shader::getProgramID() const
{
    return m_programID;
}

void  Shader::setVertexSource(std::string s)
{
    m_vertexSource = s;
}

void  Shader::setFragmentSource(std::string s)
{
    m_fragmentSource = s;
}
