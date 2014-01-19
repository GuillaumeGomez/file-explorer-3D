#ifndef MODEL_HPP
#define MODEL_HPP

#include "../myGLWidget.hpp"

namespace Object
{
  class Model : public myGLWidget
  {
  public:
    Model(Vector3D, Rotation, std::string model, float height = 2.f);
    Model(Vector3D, Rotation, const char *model, float height = 2.f);
    virtual   ~Model();
    void      initializeGL();
    void      paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix);
    virtual std::string getClassName() const;

  protected:
    bool  loadMaterial();
    bool  loadFile();

    std::string modelName;
    float       m_height;

    std::vector<int> iMeshStartIndices;
    std::vector<int> iMeshSizes;
    std::vector<int> iMaterialIndices;
    std::vector<Texture> tTextures;
    int iNumMaterials;
  };
}

#endif // MODEL_HPP
