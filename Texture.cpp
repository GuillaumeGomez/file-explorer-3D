#include "Texture.hpp"
#include "../MyWindow.hpp"

Texture::Texture(GLuint id)
    : m_id(id), m_textureName(""), m_largeur(0), m_hauteur(0), m_format(0), m_formatInterne(0),
      m_textureVide(false), m_repeat(false)
{
}

Texture::Texture(std::string fichierImage)
    : m_id(0), m_textureName(fichierImage), m_largeur(0), m_hauteur(0), m_format(0), m_formatInterne(0),
      m_textureVide(false), m_repeat(false)
{
}

Texture::Texture(GLuint largeur, GLuint hauteur, GLenum format, GLenum formatInterne, bool textureVide)
    : m_id(0), m_textureName(""), m_largeur(largeur), m_hauteur(hauteur), m_format(format),
      m_formatInterne(formatInterne), m_textureVide(textureVide), m_repeat(false)
{
}

Texture::~Texture()
{
    this->destroy();
}

bool  Texture::load()
{
    if (m_id)
        return true;
    this->destroy();
    if (m_textureName.empty()){
        if (m_largeur == 0 || m_hauteur == 0)
            return false;
        glGenTextures(1, &m_id);

        // Verrouillage
        glBindTexture(GL_TEXTURE_2D, m_id);

        if (m_repeat) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, m_formatInterne, m_largeur, m_hauteur, 0, m_format, GL_UNSIGNED_BYTE, 0);

        // Deverrouillage
        glBindTexture(GL_TEXTURE_2D, 0);
        return true;
    }
    m_id = MyWindow::loadTexture(m_textureName, true, &m_largeur, &m_hauteur);
    return m_id != 0;
}

bool  Texture::hasBeenLoaded() const
{
    return m_id != 0;
}

GLuint  Texture::getTextureID() const
{
    return m_id;
}

void  Texture::setTextureID(GLuint i)
{
    //this->destroy();
    m_id = i;
}

void  Texture::setTexture(std::string s)
{
    if (s == m_textureName)
        return;
    m_textureName = s;
    if (m_id){
        this->destroy();
        this->load();
    }
}

std::string const &Texture::getTextureName() const
{
    return m_textureName;
}

GLuint  Texture::height() const
{
    return m_hauteur;
}

GLuint  Texture::width() const
{
    return m_largeur;
}

void  Texture::bind()
{
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void  Texture::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void  Texture::bind(GLuint i)
{
    glBindTexture(GL_TEXTURE_2D, i);
}

void  Texture::destroy()
{
    if (!m_id)
        return;
    MyWindow::destroyTexture(m_id);
    m_id = 0;
}

void    Texture::setRepeat(bool b){
    m_repeat = b;
}

bool    Texture::isRepeating() const
{
    return m_repeat;
}
