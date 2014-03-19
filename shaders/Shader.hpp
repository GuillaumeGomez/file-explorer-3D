#ifndef DEF_SHADER
#define DEF_SHADER

#ifdef WIN32
#include <GL/glew.h>
#elif __APPLE__
#define GL3_PROTOTYPES 1
#include <OpenGL/gl3.h>
#else
#define GL3_PROTOTYPES 1
#include <GL/glew.h>
#endif

#include "../glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <string>

class Shader
{
public:
    Shader();
    Shader(Shader const &toCopy);
    Shader(std::string vertexSource, std::string fragmentSource);
    virtual ~Shader();

    Shader      &operator=(Shader const &toCopy);
    bool        load();
    bool        compileShader(GLuint &shader, GLenum type, std::string const &source);
    bool        compileShaderFromFile(GLuint &shader, GLenum type, std::string const &source);
    GLuint      getProgramID() const;
    std::string getError(GLuint);
    void        setVertexSource(std::string);
    void        setFragmentSource(std::string);
    std::string const &getVertexSource() const;
    std::string const &getFragmentSource() const;
    GLint       getUniform(std::string const&);
    void        setUniform(std::string const &s, glm::vec2 const &v);
    void        setUniform(GLint loc, glm::vec2 const &v);
    void        setUniform(std::string const &s, glm::vec3 const &v);
    void        setUniform(GLint loc, glm::vec3 const &v);
    void        setUniform(std::string const &s, glm::vec4 const &v);
    void        setUniform(GLint loc, glm::vec4 const &v);
    void        setUniform(std::string const &s, glm::mat3 const &m);
    void        setUniform(GLint loc, glm::mat3 const &m);
    void        setUniform(std::string const &s, glm::mat4 const &m);
    void        setUniform(GLint loc, glm::mat4 const &m);
    void        setUniform(std::string const &s, int i);
    void        setUniform(GLint loc, int i);
    void        setUniform(std::string const &s, float const &f);
    void        setUniform(GLint loc, float const &f);
    void        bind();

    static std::string getStandardVertexShader(bool hasTexture);
    static std::string getStandardFragmentShader(bool hasTexture);
    static std::string getStandard2DVertexShader(bool hasTexture);
    static std::string getStandard2DFragmentShader(bool hasTexture);
    static void        unbind();

private:
    GLuint m_vertexID;
    GLuint m_fragmentID;
    GLuint m_programID;

    std::string m_vertexSource;
    std::string m_fragmentSource;
};

#endif
