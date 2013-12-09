#include "DataHandler.hpp"

DataHandler::DataHandler()
{
}

DataHandler::~DataHandler()
{
    while (tex_objects.size() > 0) {
        delete (*tex_objects.begin()).first;
        tex_objects.erase(tex_objects.begin());
    }
    while (col_objects.size() > 0) {
        delete (*col_objects.begin()).first;
        col_objects.erase(col_objects.begin());
    }
}

GLint  DataHandler::addObject(myGLWidget *m)
{
    GLint  tot(-1);
    size_t  tmp;

    if (!m)
        return tot;
    if ((tot = this->findObject(m)) < 0) {
        if ((tmp = m->getColors().size()) > 0) {
            col_objects.push_back(std::pair<myGLWidget*, GLuint>(m, col_objects.size()));
            tot = (GLint)col_objects.size() - 1;
            col_pos.insert(col_pos.end(), m->getPositions().begin(), m->getPositions().end());
            col_datas.insert(col_datas.end(), m->getColors().begin(), m->getColors().end());
            return tot;
        }
        tex_objects.push_back(std::pair<myGLWidget*, GLuint>(m, tex_objects.size()));
        tot = (GLint)tex_objects.size() - 1;
        tex_pos.insert(tex_pos.end(), m->getPositions().begin(), m->getPositions().end());
        tex_datas.insert(tex_datas.end(), m->getTextures().begin(), m->getTextures().end());
    }
    return tot;
}

GLint   DataHandler::findObject(myGLWidget *m)
{
    GLint   pos(0);

    if (m->getColors().size() > 0) {
        for (DataHandlerContainer::iterator it = col_objects.begin(); it != col_objects.end(); ++it) {
            if ((*it).first == m)
                return pos;
            ++pos;
        }
        return -1;
    }
    for (DataHandlerContainer::iterator it = tex_objects.begin(); it != tex_objects.end(); ++it) {
        if ((*it).first == m)
            return pos;
        ++pos;
    }
    return -1;
}

/*void    DataHandler::paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix)
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
}*/
