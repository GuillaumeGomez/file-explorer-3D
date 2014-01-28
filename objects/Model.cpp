#include "Model.hpp"
#include "../Utils/MyException.hpp"
#include "../Utils/HandleFile.hpp"
#include "../String_utils.hpp"
#include "../HandleError.hpp"
#include "../shaders/ShaderHandler.hpp"
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace Object;

Model::Model(Vector3D v, Rotation r, std::string model, float height)
  : myGLWidget(v, r), modelName(model), m_height(height)
{
}

Model::Model(Vector3D v, Rotation r, const char *model, float height)
  : myGLWidget(v, r), modelName(model ? model : ""), m_height(height)
{
}


Model::~Model()
{
}

void  Model::initializeGL()
{
  /*if (!loadMaterial()){
      throw MyException(m_texture.getTextureName() + ": Model class needs a valid texture file");
    }*/
  if (!loadFile()){
      throw MyException(modelName + ": Model class needs a valid model file");
    }
  m_shader = ShaderHandler::getInstance()->createShader(Shader::getStandardVertexShader(true),
                                                        Shader::getStandardFragmentShader(true));
  if (!m_shader){
      HandleError::showError("Shader didn't load in Model");
      exit(-1);
    }
  m_uniLoc_projection = glGetUniformLocation(m_shader->getProgramID(), "projection");
  m_uniLoc_modelView = glGetUniformLocation(m_shader->getProgramID(), "modelview");
  m_uniloc_rot = glGetUniformLocation(m_shader->getProgramID(), "_rot");
  m_uniloc_pos = glGetUniformLocation(m_shader->getProgramID(), "_pos");

  m_pointsNumber = m_vertices.size() / 3.f;

  this->initVertexBufferObject();
  this->initVertexArrayObject();
}

void  Model::paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix)
{
  glUseProgram(m_shader->getProgramID());

  glBindVertexArray(m_vaoID);

  glUniformMatrix4fv(m_uniLoc_projection, 1, GL_FALSE, glm::value_ptr(proj_matrix));

  glUniformMatrix4fv(m_uniLoc_modelView, 1, GL_FALSE, glm::value_ptr(view_matrix));
  glUniform3fv(m_uniloc_pos, 1, glm::value_ptr(glm::vec3(m_pos.x(), m_pos.y(), m_pos.z())));
  //if (m_rot.rotation() != 0.f && (m_rot.x() != 0.f || m_rot.y() != 0.f || m_rot.z() != 0.f))
    glUniform4fv(m_uniloc_rot, 1, glm::value_ptr(glm::vec4(m_rot.x(), m_rot.y(), m_rot.z(), m_rot.rotation())));

  for (unsigned int i = 0; i < iMeshSizes.size(); ++i) {
      int iMatIndex = iMaterialIndices[i];
      if (iMatIndex < (int)tTextures.size())
        tTextures[iMatIndex].bind();
      glDrawArrays(GL_TRIANGLES, iMeshStartIndices[i], iMeshSizes[i]);
      if (iMatIndex < (int)tTextures.size())
        tTextures[iMatIndex].unbind();
    }
  //m_texture.bind();
  //glDrawArrays(GL_TRIANGLES, 0, m_pointsNumber);
  //m_texture.unbind();

  /*glBindVertexArray(0);
  glUseProgram(0);*/
}

bool Model::loadMaterial()
{
  // For now, we'll just look for ambient texture, i.e. line that begins with map_Ka
  FILE *fd = 0;//fopen(mtlName.c_str(), "r");

  if (!fd)
    return false;

  char line[256];

  while (fgets(line, 255, fd))
    {
      std::stringstream ss(line);
      std::string sType;
      ss >> sType;
      if (sType == "map_Ka") {
          std::string sLine = line;
          // Take the rest of line as texture name, remove newline character from end
          int from = sLine.find("map_Ka") + 6 + 1;

          std::string sTextureName = sLine.substr(from, sLine.length() - from - 1);
          std::vector<std::string> tmp_v = Utility::split<std::string>(""/*mtlName*/, "/");
#ifndef WIN32
          sTextureName = Utility::replace<std::string>(sTextureName, "\r", "");
#endif
          if (tmp_v.size() > 1){
              tmp_v.pop_back();
              m_texture.setTexture(Utility::join<std::string>(tmp_v, "/") + "/" + sTextureName);
            } else {
              m_texture.setTexture(sTextureName);
            }
          break;
        }
    }
  fclose(fd);
  return m_texture.load();
}

string Model::getClassName() const
{
  return std::string("Model");
}

bool  Model::loadFile()
{
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(modelName,
                                           aiProcess_CalcTangentSpace       |
                                           aiProcess_Triangulate            |
                                           aiProcess_JoinIdenticalVertices  |
                                           aiProcess_SortByPType);

  if (!scene) {
      HandleError::showError("Error Importing Asset");
      return false;
    }

  const int iVertexTotalSize = sizeof(aiVector3D)*2+sizeof(aiVector2D);

  int iTotalVertices = 0;
  float min = 0.f, max = 0.f;

  for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
      aiMesh* mesh = scene->mMeshes[i];
      int iMeshFaces = mesh->mNumFaces;
      iMaterialIndices.push_back(mesh->mMaterialIndex);
      int iSizeBefore = m_vertices.size() * sizeof(GLfloat) + m_textures.size() * sizeof(GLfloat) + m_normals.size() * sizeof(GLfloat);

      iMeshStartIndices.push_back(iSizeBefore/iVertexTotalSize);
      for (int j = 0; j < iMeshFaces; ++j) {
          const aiFace& face = mesh->mFaces[j];

          if (face.mIndices)
            for (int k = 0; k < 3; ++k) {
                aiVector3D pos = mesh->mVertices[face.mIndices[k]];
                aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[k]];
                aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[face.mIndices[k]] : aiVector3D(1.0f, 1.0f, 1.0f);

                m_vertices.push_back(pos.x);
                m_vertices.push_back(pos.y);
                m_vertices.push_back(pos.z);
                m_textures.push_back(uv.x);
                m_textures.push_back(uv.y);
                m_normals.push_back(normal.x);
                m_normals.push_back(normal.y);
                m_normals.push_back(normal.z);

                if (max < pos.y)
                  max = pos.y;
                else if (min > pos.y)
                  min = pos.y;
              }
        }
      int iMeshVertices = mesh->mNumVertices;
      iTotalVertices += iMeshVertices;
      int current_size = m_vertices.size() * sizeof(GLfloat) + m_textures.size() * sizeof(GLfloat) + m_normals.size() * sizeof(GLfloat);
      iMeshSizes.push_back((current_size - iSizeBefore) / iVertexTotalSize);
    }

  iNumMaterials = scene->mNumMaterials;

  vector<int> materialRemap(iNumMaterials);

  for (int i = 0; i < iNumMaterials; ++i){
      const aiMaterial* material = scene->mMaterials[i];
      int texIndex = 0;
      aiString path;  // filename

      if (material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path) == AI_SUCCESS)
        {
          vector<string> tmpr = Utility::split<string>(modelName, "/");

          tmpr.pop_back();
          string sDir = Utility::join<string>(tmpr, "/");
          sDir += "/";
          string sTextureName = path.data;
          string sFullPath = sDir + sTextureName;
          int iTexFound = -1;
          for (unsigned int j = 0; j < tTextures.size(); ++j)
            {
              if (sFullPath == tTextures[j].getTextureName())
                {
                  iTexFound = j;
                  break;
                }
            }
          if (iTexFound != -1)
            materialRemap[i] = iTexFound;
          else
            {
              Texture tNew(sFullPath);
              tNew.load();
              materialRemap[i] = tTextures.size();
              tTextures.push_back(tNew);
            }
        }
    }

  for (unsigned int i = 0; i < iMeshSizes.size(); ++i) {
      int iOldIndex = iMaterialIndices[i];
      iMaterialIndices[i] = materialRemap[iOldIndex];
    }

  float tmp = m_height / (max - min);

  for (auto it = m_vertices.begin(); it != m_vertices.end(); ++it){
      (*it) *= tmp;
    }
  for (auto it = m_normals.begin(); it != m_normals.end(); ++it){
      (*it) *= tmp;
    }

  return true;
}
