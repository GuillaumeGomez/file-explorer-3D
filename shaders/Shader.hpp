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

    static std::string getStandardVertexShader(bool hasTexture);
    static std::string getStandardFragmentShader(bool hasTexture);

private:
    GLuint m_vertexID;
    GLuint m_fragmentID;
    GLuint m_programID;

    std::string m_vertexSource;
    std::string m_fragmentSource;
};

#endif
