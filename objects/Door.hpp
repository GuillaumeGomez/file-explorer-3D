#ifndef DOOR_HPP
#define DOOR_HPP

#include "../myGLWidget.hpp"

namespace Object
{
    class Door : public myGLWidget
    {
    public:
        Door(Vector3D, Rotation, const char*);
        virtual ~Door();
        void    initializeGL(bool invert);
        void    initializeGL();
        void    initializeGLNoList();
        void    paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix);
        std::string const &getName() const;
        float const       &getSize() const;
        float const       &getHeight() const;

    private:
        std::string m_name;
        float       m_size;
        float       m_height;
    };
}

#endif // DOOR_HPP
