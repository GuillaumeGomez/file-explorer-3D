#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "myGLWidget.hpp"

class Text;
class Plane;

namespace Object
{
    class Button : public myGLWidget
    {
    public:
        Button(std::string text, Color text_color, Color background_color, Vector3D, float width, float height, float font_size = 0.2f);
        Button(const char *text, Color text_color, Color background_color, Vector3D, float width, float height, float font_size = 0.2f);
        virtual ~Button();
        virtual void    initializeGL();
        virtual void    paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix);
        void            setBackgroundColor(Color);
        Color const     &getBackgroundColor() const;
        void            setBackgroundTexture(std::string);
        void            setBackgroundTexture(const char*);

    protected:
        void    createButton(std::string &t, Color &c, float &width, float &height, float &font_size);

        Text    *m_buttonText;
        float   width;
        float   height;
    };
}

#endif // BUTTON_HPP
