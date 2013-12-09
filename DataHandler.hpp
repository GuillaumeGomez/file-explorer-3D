#ifndef DATAHANDLER_HPP
#define DATAHANDLER_HPP

#include "myGLWidget.hpp"
#include <map>

typedef std::vector<std::pair<myGLWidget*, GLuint> > DataHandlerContainer;

class DataHandler
{
public:
    DataHandler();
    virtual ~DataHandler();
    GLint   addObject(myGLWidget*);
    GLint   findObject(myGLWidget*);

private:
    DataHandlerContainer    tex_objects;
    DataHandlerContainer    col_objects;
    std::vector<GLfloat>    col_pos, col_datas, tex_pos, tex_datas;
};

#endif // DATAHANDLER_HPP
