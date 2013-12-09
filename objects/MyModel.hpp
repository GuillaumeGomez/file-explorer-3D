#ifndef MYMODEL_HPP
#define MYMODEL_HPP

#include "myGLWidget.hpp"

#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>


namespace Object
{
  class MyModel : public myGLWidget
  {
  public:
    MyModel(std::string file, QVector3D pos, Rotation rot);
    virtual ~MyModel();
    virtual void initializeGL(){}

  private:
    FbxManager* lSdkManager;
    FbxScene* lScene;
  };
}

#endif // MYMODEL_HPP
