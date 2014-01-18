#include "Model.hpp"
#include "../Utils/MyException.hpp"
#include "../Utils/HandleFile.hpp"
#include "../String_utils.hpp"
#include "../HandleError.hpp"
#include <string>

using namespace Object;

Model::Model(Vector3D v, Rotation r, std::string model, std::string mtlFileName, float height)
  : myGLWidget(v, r), modelName(model), mtlName(mtlFileName), m_height(height)
{
  m_shader = new Shader;
}

Model::Model(Vector3D v, Rotation r, const char *model, const char *mtlFileName, float height)
  : myGLWidget(v, r, mtlFileName), modelName(model ? model : ""), mtlName(mtlFileName ? mtlFileName : ""), m_height(height)
{
  m_shader = new Shader;
}


Model::~Model()
{
}

void  Model::initializeGL()
{
  if (!loadMaterial()){
      throw MyException(m_texture.getTextureName() + ": Model class needs a valid texture file");
    }
  m_shader->setVertexSource(Shader::getStandardVertexShader(true));
  m_shader->setFragmentSource(Shader::getStandardFragmentShader(true));
  if (!m_shader->load()){
      HandleError::showError("Shader didn't load in Model");
      exit(-1);
    }
  m_uniLoc_projection = glGetUniformLocation(m_shader->getProgramID(), "projection");
  m_uniLoc_modelView = glGetUniformLocation(m_shader->getProgramID(), "modelview");

  FILE  *fd = fopen(modelName.c_str(), "r");

  if (!fd) {
      throw MyException(modelName + ": problem with the model file");
    }

  char line[256];
  int iNumFaces = 0;
  int no_line = 0;
  bool err = false;
  int iAttrBitField = 0;
  float min(0.f), max(0.f);

  std::vector<glm::vec3> vVertices;
  std::vector<glm::vec2> vTexCoords;
  std::vector<glm::vec3> vNormals;

  while (fgets(line, 255, fd)) {
      err = false;
      ++no_line;

      if (strlen(line) <= 1)
        continue;

      std::string sLine;
      std::stringstream ss(line);
      ss >> sLine;

      if (sLine == "#")
        continue;
      // Vertex
      else if (sLine == "v") {
          glm::vec3 vNewVertex;
          int dim = 0;

          while (dim < 3 && ss >> vNewVertex[dim])
            dim++;
          vVertices.push_back(vNewVertex);
          iAttrBitField |= 1;
        }
      // Texture coordinate
      else if (sLine == "vt") {
          glm::vec2 vNewCoord;
          int dim = 0;

          while (dim < 2 && ss >> vNewCoord[dim])
            dim++;
          vTexCoords.push_back(vNewCoord);
          iAttrBitField |= 2;
        }
      // Normal
      else if (sLine == "vn") {
          glm::vec3 vNewNormal;
          int dim = 0;

          while (dim < 3 && ss >> vNewNormal[dim])
            dim++;
          vNewNormal = glm::normalize(vNewNormal);
          vNormals.push_back(vNewNormal);
          iAttrBitField |= 4;
        }
      // Face definition
      else if (sLine == "f") {
          std::string sFaceData;
          // This will run for as many vertex definitions as the face has
          // (for triangle, it's 3)
          while (ss >> sFaceData)
            {
              std::vector<std::string> data = Utility::split<std::string>(sFaceData, "/");
              int iVertIndex = -1, iTexCoordIndex = -1, iNormalIndex = -1;

              // If there were some vertices defined earlier
              if (iAttrBitField & 1) {
                  if (data[0].length() > 0)
                    sscanf(data[0].c_str(), "%d", &iVertIndex);
                  else
                    err = true;
                }
              // If there were some texture coordinates defined earlier
              if (iAttrBitField & 2 && !err) {
                  if (data.size() >= 1) {
                      // Just a check whether face format isn't f v//vn
                      // In that case, data[1] is empty string
                      if (data[1].length() > 0)
                        sscanf(data[1].c_str(), "%d", &iTexCoordIndex);
                      else
                        err = true;
                    }
                  else
                    err = true;
                }
              // If there were some normals defined defined earlier
              if (iAttrBitField & 4 && !err) {
                  if (data.size() >= 2) {
                      if (data[2].length() > 0)
                        sscanf(data[2].c_str(), "%d", &iNormalIndex);
                      else
                        err = true;
                    }
                  else
                    err = true;
                }
              if (err) {
                  fclose(fd);
                  throw MyException(modelName + ": error within the model file at line " + Utility::toString<int>(no_line));
                }

              // Check whether vertex index is within boundaries (indexed from 1)
              if (iVertIndex > 0 && iVertIndex <= (int)vVertices.size()) {
                  GLfloat tmp_v = vVertices[iVertIndex - 1][1];
                  m_vertices.push_back(vVertices[iVertIndex - 1][0]);
                  m_vertices.push_back(tmp_v);
                  m_vertices.push_back(vVertices[iVertIndex - 1][2]);

                  if (tmp_v < min)
                    min = tmp_v;
                  else if (tmp_v > max)
                    max = tmp_v;

                  //vboModelData.addData(&vVertices[iVertIndex - 1], sizeof(glm::vec3));
                }
              if (iTexCoordIndex > 0 && iTexCoordIndex <= (int)vTexCoords.size()) {
                  m_textures.push_back(vTexCoords[iTexCoordIndex - 1][0]);
                  m_textures.push_back(vTexCoords[iTexCoordIndex - 1][1]);

                  //vboModelData.addData(&vTexCoords[iTexCoordIndex - 1], sizeof(glm::vec2));
                }
              if (iNormalIndex > 0 && iNormalIndex <= (int)vNormals.size()) {
                  m_normals.push_back(vNormals[iNormalIndex - 1][0]);
                  m_normals.push_back(vNormals[iNormalIndex - 1][1]);
                  m_normals.push_back(vNormals[iNormalIndex - 1][2]);

                  //vboModelData.addData(&vNormals[iNormalIndex - 1], sizeof(glm::vec3));
                }
            }
          iNumFaces++;
        }
      // Shading model, for now just skip it
      else if (sLine == "s") {
          // Do nothing for now
        }
      // Material specified, skip it again
      else if (sLine == "usemtl") {
          // Do nothing for now
        }
    }

  fclose(fd);
  m_pointsNumber = m_vertices.size() / 3.f;

  float tmp = m_height / (max - min);

  for (auto it = m_vertices.begin(); it != m_vertices.end(); ++it){
      (*it) *= tmp;
    }
  for (auto it = m_normals.begin(); it != m_normals.end(); ++it){
      (*it) *= tmp;
    }

  this->initVertexBufferObject();
  this->initVertexArrayObject();
}

void  Model::paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix)
{
  glUseProgram(m_shader->getProgramID());

  glBindVertexArray(m_vaoID);

  glUniformMatrix4fv(m_uniLoc_projection, 1, GL_FALSE, glm::value_ptr(proj_matrix));

  glm::mat4 tmp = glm::translate(view_matrix, glm::vec3(m_pos.x(), m_pos.y(), m_pos.z()));

  if (m_rot.rotation() != 0.f && (m_rot.x() != 0.f || m_rot.y() != 0.f || m_rot.z() != 0.f))
    tmp = glm::rotate(tmp, m_rot.rotation(), glm::vec3(m_rot.x(), m_rot.y(), m_rot.z()));
  glUniformMatrix4fv(m_uniLoc_modelView, 1, GL_FALSE, glm::value_ptr(tmp));

  m_texture.bind();
  glDrawArrays(GL_TRIANGLES, 0, m_pointsNumber);
  m_texture.unbind();

  /*glBindVertexArray(0);
  glUseProgram(0);*/
}

bool Model::loadMaterial()
{
  // For now, we'll just look for ambient texture, i.e. line that begins with map_Ka
  FILE *fd = fopen(mtlName.c_str(), "r");

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
          std::vector<std::string> tmp_v = Utility::split<std::string>(mtlName, "/");
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
