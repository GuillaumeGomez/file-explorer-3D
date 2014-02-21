#include "Model.hpp"
#include "../Utils/MyException.hpp"
#include "../Utils/HandleFile.hpp"
#include "../String_utils.hpp"
#include "../HandleError.hpp"
#include "../shaders/ShaderHandler.hpp"
#include <string>

#include <assimp/matrix4x4.h>

using namespace Object;

Model::Model(Vector3D v, Rotation r, std::string model, float height)
  : myGLWidget(v, r), modelName(model), m_height(height)
{
  m_scene = 0;
  dtime = 0.f;
}

Model::Model(Vector3D v, Rotation r, const char *model, float height)
  : myGLWidget(v, r), modelName(model ? model : ""), m_height(height)
{
  m_scene = 0;
  dtime = 0.f;
}


Model::~Model()
{
  /*for(auto it = tTextures.begin(); it != tTextures.end(); ++it)
   (*it).destroy();*/
  /*for (auto it : m_Textures)
    delete it;*/
  if (m_scene)
    delete m_scene;
}

void  Model::initializeGL()
{
  m_scene = new SceneAnimator;
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(modelName,
                                           aiProcess_CalcTangentSpace       |
                                           aiProcess_Triangulate            |
                                           aiProcess_JoinIdenticalVertices  |
                                           aiProcess_SortByPType);

  if (!scene) {
      HandleError::showError("Error Importing Asset");
      exit(-1);
    }
  if (!this->load(scene)) {
      //aiReleaseImport(scene);
      HandleError::showError("Couldn't init animated model");
      exit(-1);
    }
  //aiReleaseImport(scene);

  //clear(); to destroy any previous created stuff ?
  /*if (!loadMaterial()){
      throw MyException(m_texture.getTextureName() + ": Model class needs a valid texture file");
    }*/
  //old stuff
  /*if (!loadFile()){
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
  this->initVertexArrayObject();*/
}

void  Model::update(const float &f)
{
  dtime += f;
}

void  Model::paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix)
{
  glUseProgram(m_shader->getProgramID());

  glBindVertexArray(m_vaoID);

  glUniformMatrix4fv(m_uniLoc_projection, 1, GL_FALSE, glm::value_ptr(proj_matrix));

  glUniform3fv(m_uniloc_pos, 1, glm::value_ptr(glm::vec3(m_pos.x(), m_pos.y(), m_pos.z())));
  glUniform4fv(m_uniloc_rot, 1, glm::value_ptr(glm::vec4(m_rot.x(), m_rot.y(), m_rot.z(), m_rot.rotation())));

  /*for (std::pair<GLuint, GLuint> it : m_index) {
      glDrawArrays(GL_TRIANGLE_STRIP, it.first, it.second / 3);
    }*/

  std::vector<glm::mat4> transformed_bones = m_scene->GetTransforms(dtime);

  int i = 0;

  for (std::pair<GLuint, GLuint> it : m_index) {
      if (i < transformed_bones.size()) {
          glm::mat4 tmp = view_matrix * transformed_bones[i++];
          glUniformMatrix4fv(m_uniLoc_modelView, 1, GL_FALSE, glm::value_ptr(tmp));
        }
      glDrawElements(GL_TRIANGLES, it.first, GL_UNSIGNED_INT, (void*)it.second);
    }

  /*for (unsigned int i = 0; i < iMeshSizes.size(); ++i) {
      int iMatIndex = iMaterialIndices[i];
      if (iMatIndex <(int)tTextures.size())
        tTextures[iMatIndex].bind();
      glDrawArrays(GL_TRIANGLES, iMeshStartIndices[i], iMeshSizes[i]);
      if (iMatIndex <(int)tTextures.size())
        tTextures[iMatIndex].unbind();
    }*/
}

/*void ExtractMaterials(Batch& batch, const aiMaterial* pcMat){
  Material* mat = batch.GetMaterial();
  mat->Diffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
  aiGetMaterialColor(pcMat,AI_MATKEY_COLOR_DIFFUSE, (aiColor4D*)&mat->Diffuse);
  mat->Specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
  aiGetMaterialColor(pcMat,AI_MATKEY_COLOR_SPECULAR, (aiColor4D*)&mat->Specular);
  mat->Ambient = vec4(1.0f, 1.0f, 1.0f, 1.0f);
  aiGetMaterialColor(pcMat,AI_MATKEY_COLOR_AMBIENT,(aiColor4D*)&mat->Ambient);
  mat->Emissive = vec4(1.0f, 1.0f, 1.0f, 1.0f);
  aiGetMaterialColor(pcMat,AI_MATKEY_COLOR_EMISSIVE, (aiColor4D*)&mat->Emissive);
  mat->Power =1.0f;
  aiGetMaterialFloat(pcMat,AI_MATKEY_SHININESS_STRENGTH, &mat->Power);
}*/

void LoadMaterials(const aiMesh* mesh, aiMaterial** mMaterials, /*Batch *batch, */std::string& path, const std::string& asset_dir){
  /*aiString szPath;
  std::string basefolder = asset_dir + "Mesh\\";
  ///////////////////DIFFUSE TEXTURE
  if( AI_SUCCESS == aiGetMaterialString(mMaterials[mesh->mMaterialIndex], AI_MATKEY_TEXTURE_DIFFUSE(0), &szPath) ){
      std::string filename(StripPath(szPath.data));// sometimes the textures are stored with their full path, which is not correct, strip it
      Graphics::Texture* diffuse = batch->GetDiffuseMap();
      diffuse->Create(path + filename);
      diffuse->Save_To_Disk(basefolder + StripFileExtention(filename) + ".dds");
    }  else {
      OUTPUT_DEBUG_MSG("failure getting the diffuse texture from assimp model "<<szPath.length);
    }
  /////////////////////// BUMP MAP
  if( AI_SUCCESS == aiGetMaterialString(mMaterials[mesh->mMaterialIndex], AI_MATKEY_TEXTURE_HEIGHT(0),&szPath) ){
      std::string filename(StripPath(szPath.data));// sometimes the textures are stored with their full path, which is not correct, strip it
      Graphics::Texture* normalbump = batch->GetNormal_Bump();

      if(GetFileExtention(filename) == ".dds"){// file extention correct, load it
          normalbump->Create(path + filename);
          normalbump->Save_To_Disk(basefolder + StripFileExtention(filename) + ".dds");
        } else {// convert it to a dds file
          Graphics::Texture::GenerateNormalBumpFromBump(path + filename, basefolder + StripFileExtention(filename) + ".dds");
          normalbump->Create(basefolder + StripFileExtention(filename) + ".dds");
        }
      //////////////////NORMAL MAP
    } else if( AI_SUCCESS == aiGetMaterialString(mMaterials[mesh->mMaterialIndex], AI_MATKEY_TEXTURE_NORMALS(0),&szPath) ){// if there is a bump, I generate a normal map from it, so I dont bother with this
      std::string filename(StripPath(szPath.data));// sometimes the textures are stored with their full path, which is not correct, strip it

      Graphics::Texture* normalmap = batch->GetNormalMap();
      normalmap->Create(path + filename);
      normalmap->Save_To_Disk(basefolder + StripFileExtention(filename) + ".dds");

    }
  ExtractMaterials(*batch, mMaterials[mesh->mMaterialIndex]);*/
}

bool  Model::load(const aiScene *load)
{
  size_t numverts(0), currentvertex(0), currentindex(0), numindices(0);
  bool hasbones=false;
  // go through the mesh counting all the verts and indices that I will need
  for (unsigned int i = 0; i < load->mNumMeshes;++i) {
      numverts += load->mMeshes[i]->mNumVertices;
      numindices += load->mMeshes[i]->mNumFaces*3;
      if (load->mMeshes[i]->HasBones())
        hasbones=true;
    }
  if(!hasbones) {
      //aiReleaseImport(load);// free the resources
      //OUTPUT_DEBUG_MSG("Could not load the Model in Animated_Mesh::Load_MyFormat file: '" + file+"', there were no bones deteected.");
      HandleError::showError("No bones detected...");
      return false;
    }
  int stride(2);

  //IB.Stride=2;
  if(numverts >= 65536) {
      //IB.Stride=4;
      stride = 4;
    }

  //m_vertices.resize(numverts);
  //m_indices.resize((stride/2)*numindices);
  m_indices = new GLuint[(stride/2)*numindices];
  memset(m_indices, 0, (stride/2)*numindices * sizeof(*m_indices));
  std::vector<Vertex> tempverts(numverts);
  std::vector<std::string> bonenames;

  for (unsigned int i = 0; i < load->mNumMeshes;++i){
      //Graphics::Texture diffuse, normal;
      const aiMesh* mesh = load->mMeshes[i];
      if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
          //OUTPUT_DEBUG_MSG("There are errors with this submesh, named: "<<mesh->mName.data<<"   Please, fix it");
          if(mesh->mPrimitiveTypes == aiPrimitiveType_LINE){
              //OUTPUT_DEBUG_MSG("Problem: The mesh containes lines when it should only contain triangles");
            }else {
              //OUTPUT_DEBUG_MSG("Problem: The mesh containes points when it should only contain triangles");
            }
          continue;
        }

      if (!mesh->HasTextureCoords(0))	{
          //OUTPUT_DEBUG_MSG("There are errors with this submesh, named: "<<mesh->mName.data<<"   Please, fix it");
          //OUTPUT_DEBUG_MSG("Problem: The mesh containes no texcoords, which means there will just be color displayed. This engine does not support color mesh displays, only textured mesh!");
          continue;
        }
      /*if(!mesh->HasTangentsAndBitangents()) {
          //OUTPUT_DEBUG_MSG("There are errors with this submesh, named: "<<mesh->mName.data<<"   Please, fix it");
          //OUTPUT_DEBUG_MSG("Problem: Tangents were not created. No known fix");
          continue;
        }*/
      //OUTPUT_DEBUG_MSG("Loading "<<mesh->mNumBones<<" bones . . .");
      for (unsigned int a = 0; a < mesh->mNumBones; a++)	{
          const aiBone* bone = mesh->mBones[a];
          size_t bonein(-1);
          for(size_t ib(0); ib < bonenames.size(); ib++){
              std::string tname = bone->mName.data;
              if(tname == bonenames[ib]){// found the bone.. break
                  bonein=ib;
                  break;
                }
            }
          if(bonein ==-1){//  did not find the bone, this is a new one push back
              bonein = bonenames.size();// get the index before insertion
              bonenames.push_back(bone->mName.data);
            }
          // there should only be 4 per vertex here because assimp guaranteees it, but if there are more, we are ok
          for (unsigned int b = 0; b < bone->mNumWeights; b++){
              if(tempverts[bone->mWeights[b].mVertexId+ currentvertex].Weights.x <= 0.f) {
                  tempverts[bone->mWeights[b].mVertexId+ currentvertex].Bones[0] = static_cast<float>(bonein);
                  tempverts[bone->mWeights[b].mVertexId+ currentvertex].Weights.x = bone->mWeights[b].mWeight;
                } else if(tempverts[bone->mWeights[b].mVertexId+ currentvertex].Weights.y <= 0.f){
                  tempverts[bone->mWeights[b].mVertexId+ currentvertex].Bones[1] = static_cast<float>(bonein);
                  tempverts[bone->mWeights[b].mVertexId+ currentvertex].Weights.y = bone->mWeights[b].mWeight;
                } else if(tempverts[bone->mWeights[b].mVertexId+ currentvertex].Weights.z <= 0.f){
                  tempverts[bone->mWeights[b].mVertexId+ currentvertex].Bones[2] = static_cast<float>(bonein);
                  tempverts[bone->mWeights[b].mVertexId+ currentvertex].Weights.z = bone->mWeights[b].mWeight;
                } else if(tempverts[bone->mWeights[b].mVertexId+ currentvertex].Weights.w <= 0.f){
                  tempverts[bone->mWeights[b].mVertexId+ currentvertex].Bones[3] = static_cast<float>(bonein);
                  tempverts[bone->mWeights[b].mVertexId+ currentvertex].Weights.w = bone->mWeights[b].mWeight;
                }
            }
        }
      for (unsigned int x = 0; x < mesh->mNumVertices;++x){
          tempverts[x + currentvertex].pos[0] = mesh->mVertices[x][0];
          tempverts[x + currentvertex].pos[1] = mesh->mVertices[x][1];
          tempverts[x + currentvertex].pos[2] = mesh->mVertices[x][2];
          tempverts[x + currentvertex].pos[3] = mesh->mVertices[x][3];

          tempverts[x + currentvertex].tex[0] =  mesh->mTextureCoords[0][x][0];
          tempverts[x + currentvertex].tex[1] =  mesh->mTextureCoords[0][x][1];

          if (mesh->HasNormals()) {
              tempverts[x + currentvertex].normal[0] = mesh->mNormals[x][0];
              tempverts[x + currentvertex].normal[1] = mesh->mNormals[x][1];
              tempverts[x + currentvertex].normal[2] = mesh->mNormals[x][2];
            }

          if (mesh->HasTangentsAndBitangents()) {
              tempverts[x + currentvertex].tangent[0] = mesh->mTangents[x][0];
              tempverts[x + currentvertex].tangent[1] = mesh->mTangents[x][1];
              tempverts[x + currentvertex].tangent[2] = mesh->mTangents[x][2];
            }
        }
      // check whether we can use 16 bit indices for our format... the ASSIMPOBLARBLA uses 32 bit indices for all theirs..
      /*if (stride == 4){
          uint32_t* pbData = reinterpret_cast<uint32_t*>(&m_indices[currentindex]);
          for (unsigned int x = 0; x < mesh->mNumFaces;++x){
              for (unsigned int a = 0; a < 3 ;++a) {
                  *pbData++ = static_cast<uint32_t>(mesh->mFaces[x].mIndices[a]+ currentvertex);
                }
            }
        } else {
          uint16_t* pbData = reinterpret_cast<uint16_t*>(&m_indices[currentindex]);
          for (unsigned int x = 0; x < mesh->mNumFaces;++x){
              for (unsigned int a = 0; a < 3 ;++a) {
                  *pbData++ = static_cast<uint16_t>(mesh->mFaces[x].mIndices[a]+ currentvertex);
                }
            }
        }*/
      for (unsigned int x = 0; x < mesh->mNumFaces;++x){
          for (unsigned int a = 0; a < 3 ;++a) {
              m_indices[currentindex + a + x * 3] = static_cast<uint32_t>(mesh->mFaces[x].mIndices[a]+ currentvertex);
            }
        }


      //load the textures
      auto tmp_v = Utility::split<std::string>(modelName, "/");
      std::string pathtomodel = "";

      if (tmp_v.size() > 1) {
          tmp_v.pop_back();
          pathtomodel = Utility::join<std::string>(tmp_v, "/") + "/";
        }

      //GLuint tmp_val = m_vertices.size();
      for (auto it : tempverts) {
          m_vertices.push_back(it.pos[0]);
          m_vertices.push_back(it.pos[1]);
          m_vertices.push_back(it.pos[2]);
          //vertices.push_back(it.pos[3]);
        }

      m_index.push_back(std::pair<GLuint, GLuint>(currentindex, currentindex + mesh->mNumFaces*3));
      //m_index.push_back(std::pair<GLuint, GLuint>(tmp_val, m_vertices.size() - tmp_val));

      //Batch *batch = new Batch();

      //LoadMaterials(mesh, load->mMaterials, batch, pathtomodel, Asset_Dir);
      // the one to use LoadMaterials(mesh, load->mMaterials, pathtomodel, Asset_Dir);
      //batch->NumIndices=mesh->mNumFaces*3;
      //batch->StartIndex = static_cast<uint32_t>(currentindex);
      //batch->NumVerts= mesh->mNumVertices;
      // make sure to increment the ref count for thesse so they are properly destroyed

      currentvertex+=mesh->mNumVertices;
      currentindex+=mesh->mNumFaces*3;

      //Graphics::Shader_Macro* ptr = nullptr;
      //if(batch->Has_NormalMap()) ptr = macro1;
      //else ptr = macro0;

      //batch->GetVS()->CompileShaderFromFile("Animated_Mesh.fx", "VS", "vs_4_0", ptr);

      /*FormatDesc lay[] = {
        FormatDesc(),
        FormatDesc(TYPE_TEXCOORD, FORMAT_FLOAT, 2),
        FormatDesc(TYPE_NORMAL, FORMAT_FLOAT, 3),
        FormatDesc(TYPE_TANGENT, FORMAT_FLOAT, 3),
        FormatDesc(TYPE_BONE, FORMAT_FLOAT, 4),
        FormatDesc(TYPE_WEIGHT, FORMAT_FLOAT, 4)
      };
      batch->GetVS()->CreateInputLayout(lay, sizeof(lay)/sizeof(FormatDesc));
      batch->GetPS()->CompileShaderFromFile("Animated_Mesh.fx", "PS", "ps_4_0", ptr);

      Batches.push_back(batch);*/
    }
  m_scene->Init(load);

  if (currentvertex==0) {// this could happen, if so GET OUTOF HERE
      //OUTPUT_DEBUG_MSG("Problem loading the mesh, there were no vertices loaded. Failed to load the mesh");
      HandleError::showError("Error: no vertices loaded");
      return false;
    }
  for (unsigned int i = 0; i < m_vertices.size(); ++i) {
      m_colors.push_back(1.f);
    }

  glGenBuffers(1, &m_vboID);
  glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

  glBufferData(GL_ARRAY_BUFFER, m_vertices.size() + m_colors.size(), 0, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices.size(), &m_vertices[0]);
  glBufferSubData(GL_ARRAY_BUFFER, m_vertices.size(), m_colors.size(), &m_colors[0]);

  /*if (m_texturesSize > 0)
    glBufferSubData(GL_ARRAY_BUFFER, m_verticesSize, m_texturesSize, &m_textures[0]);
  if (m_colorsSize > 0)
    glBufferSubData(GL_ARRAY_BUFFER, m_verticesSize + m_texturesSize, m_colorsSize, &m_colors[0]);
  if (m_normalsSize > 0)
    glBufferSubData(GL_ARRAY_BUFFER, m_verticesSize + m_texturesSize + m_colorsSize, m_normalsSize, &m_normals[0]);*/

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &elementbuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
  //glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), &m_indices[0], GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentindex * sizeof(GLuint), m_indices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glGenVertexArrays(1, &m_vaoID);
  glBindVertexArray(m_vaoID);
  glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
  glVertexAttribPointer(VERTEX_COORD, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(VERTEX_COORD);
  glVertexAttribPointer(COLOR_COORD, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(m_vertices.size()));
  glEnableVertexAttribArray(COLOR_COORD);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

  /*glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/
  glBindVertexArray(0);

  if (!(m_shader = ShaderHandler::createShader(Shader::getStandardVertexShader(false), Shader::getStandardFragmentShader(false)))) {
      HandleError::showError("Shader didn't load in Model class");
      exit(-1);
    }

  m_uniLoc_projection = glGetUniformLocation(m_shader->getProgramID(), "projection");
  m_uniLoc_modelView = glGetUniformLocation(m_shader->getProgramID(), "modelview");
  m_uniloc_rot = glGetUniformLocation(m_shader->getProgramID(), "_rot");
  m_uniloc_pos = glGetUniformLocation(m_shader->getProgramID(), "_pos");

  //VB[0].Create(currentvertex, sizeof(Vertex), VERTEX_BUFFER, IMMUTABLE, CPU_NONE, &tempverts[0] );
  //IB.Create(currentindex, stride, INDEX_BUFFER, IMMUTABLE, CPU_NONE, &m_indices[0]); // create index buffer!
  //OUTPUT_DEBUG_MSG("Finished Loading the Mesh");
  //Name = FileName = file;
  return true;
}

/*bool Model::loadMaterial()
{
  // For now, we'll just look for ambient texture, i.e. line that begins with map_Ka
  FILE *fd = 0;//fopen(mtlName.c_str(), "r");

  if (!fd)
    return false;

  char line[256];

  while(fgets(line, 255, fd))
    {
      std::stringstream ss(line);
      std::string sType;
      ss >> sType;
      if (sType == "map_Ka") {
          std::string sLine = line;
          // Take the rest of line as texture name, remove newline character from end
          int from = sLine.find("map_Ka") + 6 + 1;

          std::string sTextureName = sLine.substr(from, sLine.length() - from - 1);
          std::vector<std::string> tmp_v = Utility::split<std::string>(""//mtlName, "/");
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
}*/

string Model::getClassName() const
{
  return std::string("Model");
}

/*bool  Model::loadFile()
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
                aiVector3D normal(0.f, 0.f, 0.f);
                aiVector3D uv(0.f, 0.f, 0.f);
                aiVector3D pos(0.f, 0.f, 0.f);
                if (mesh->mNumVertices > face.mIndices[k]) {
                    pos = mesh->mVertices[face.mIndices[k]];
                  }
                if (mesh->mTextureCoords[0])
                  uv = mesh->mTextureCoords[0][face.mIndices[k]];
                normal = mesh->HasNormals() && face.mNumIndices < k ? mesh->mNormals[face.mIndices[k]] : aiVector3D(1.0f, 1.0f, 1.0f);
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

  float tmp = m_height /(max - min);

 for (auto it = m_vertices.begin(); it != m_vertices.end(); ++it){
     (*it) *= tmp;
    }
 for (auto it = m_normals.begin(); it != m_normals.end(); ++it){
     (*it) *= tmp;
    }

  return true;
}*/

//#include "cAnimationController.h"
//#include "../Utilities/Utilities.h"

void TransformMatrix(glm::mat4& out,const aiMatrix4x4& in){// there is some type of alignment issue with my mat4 and the aimatrix4x4 class, so the copy must be manually
  out[0][0]=in.a1;
  out[0][1]=in.a2;
  out[0][2]=in.a3;
  out[0][3]=in.a4;

  out[1][0]=in.b1;
  out[1][1]=in.b2;
  out[1][2]=in.b3;
  out[1][3]=in.b4;

  out[2][0]=in.c1;
  out[2][1]=in.c2;
  out[2][2]=in.c3;
  out[2][3]=in.c4;

  out[3][0]=in.d1;
  out[3][1]=in.d2;
  out[3][2]=in.d3;
  out[3][3]=in.d4;
}
// ------------------------------------------------------------------------------------------------
// Constructor on a given animation.
cAnimEvaluator::cAnimEvaluator(const aiAnimation* pAnim) {
  mLastTime = 0.0;
  TicksPerSecond = static_cast<float>(pAnim->mTicksPerSecond != 0.0f ? pAnim->mTicksPerSecond : 100.0f);
  Duration = static_cast<float>(pAnim->mDuration);
  Name = std::string(pAnim->mName.data, pAnim->mName.length);
  if(Name.size()==0) Name ="Animation" + std::to_string(Animation_Indexer);
  //OUTPUT_DEBUG_MSG("Creating Animation named: "<<Name);
  Channels.resize(pAnim->mNumChannels);
  for(unsigned int a = 0; a < pAnim->mNumChannels; a++){
      Channels[a].Name = pAnim->mChannels[a]->mNodeName.data;
      for(unsigned int i(0); i< pAnim->mChannels[a]->mNumPositionKeys; i++) Channels[a].mPositionKeys.push_back(pAnim->mChannels[a]->mPositionKeys[i]);
      for(unsigned int i(0); i< pAnim->mChannels[a]->mNumRotationKeys; i++) Channels[a].mRotationKeys.push_back(pAnim->mChannels[a]->mRotationKeys[i]);
      for(unsigned int i(0); i< pAnim->mChannels[a]->mNumScalingKeys; i++) Channels[a].mScalingKeys.push_back(pAnim->mChannels[a]->mScalingKeys[i]);
    }
  mLastPositions.resize(pAnim->mNumChannels, std::make_tuple(0, 0, 0));
  //OUTPUT_DEBUG_MSG("Finished Creating Animation named: "<<Name);
}

unsigned int cAnimEvaluator::GetFrameIndexAt(float ptime){
  // get a [0.f ... 1.f) value by allowing the percent to wrap around 1
  ptime *= TicksPerSecond;

  float time = 0.0f;
  if(Duration > 0.0)
    time = fmod(ptime, Duration);

  float percent = time / Duration;
  if(!PlayAnimationForward) percent= (percent-1.0f)*-1.0f;// this will invert the percent so the animation plays backwards
  return static_cast<unsigned int>((static_cast<float>(Transforms.size()) * percent));
}

void cAnimEvaluator::Save(std::string const& file){
  std::ofstream fd(file, std::ios_base::binary | std::ios_base::trunc);

  if (fd.is_open()) {
      this->Save(fd);
      fd.close();
    }
}

void cAnimEvaluator::Save(std::ofstream& file){
  uint32_t nsize = static_cast<uint32_t>(Name.size());
  file.write(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the size of the animation name
  file.write(Name.c_str(), nsize);// the size of the animation name
  file.write(reinterpret_cast<char*>(&Duration), sizeof(Duration));// the duration
  file.write(reinterpret_cast<char*>(&TicksPerSecond), sizeof(TicksPerSecond));// the number of ticks per second
  nsize = static_cast<uint32_t>(Channels.size());// number of animation channels,
  file.write(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the number animation channels
  for(size_t j(0); j< Channels.size(); j++){// for each channel
      nsize =static_cast<uint32_t>(Channels[j].Name.size());
      file.write(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the size of the name
      file.write(Channels[j].Name.c_str(), nsize);// the size of the animation name

      nsize =static_cast<uint32_t>(Channels[j].mPositionKeys.size());
      file.write(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the number of position keys
      for(size_t i(0); i< nsize; i++){// for each channel
          file.write(reinterpret_cast<char*>(&Channels[j].mPositionKeys[i].mTime), sizeof(Channels[j].mPositionKeys[i].mTime));// pos key
          file.write(reinterpret_cast<char*>(&Channels[j].mPositionKeys[i].mValue), sizeof(Channels[j].mPositionKeys[i].mValue));// pos key
        }

      nsize =static_cast<uint32_t>(Channels[j].mRotationKeys.size());
      file.write(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the number of position keys
      for(size_t i(0); i< nsize; i++){// for each channel
          file.write(reinterpret_cast<char*>(&Channels[j].mRotationKeys[i].mTime), sizeof(Channels[j].mRotationKeys[i].mTime));// rot key
          file.write(reinterpret_cast<char*>(&Channels[j].mRotationKeys[i].mValue), sizeof(Channels[j].mRotationKeys[i].mValue));// rot key
        }

      nsize =static_cast<uint32_t>(Channels[j].mScalingKeys.size());
      file.write(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the number of position keys
      for(size_t i(0); i< nsize; i++){// for each channel
          file.write(reinterpret_cast<char*>(&Channels[j].mScalingKeys[i].mTime), sizeof(Channels[j].mScalingKeys[i].mTime));// rot key
          file.write(reinterpret_cast<char*>(&Channels[j].mScalingKeys[i].mValue), sizeof(Channels[j].mScalingKeys[i].mValue));// rot key
        }

    }
}

void cAnimEvaluator::Load(std::string const &file){
  std::ifstream fd(file);

  if (fd.is_open()) {
      this->Load(fd);
      fd.close();
    }
}

void cAnimEvaluator::Load(std::ifstream& file){
  uint32_t nsize = 0;
  file.read(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the size of the animation name
  char temp[250];
  file.read(temp, nsize);// the size of the animation name
  temp[nsize]=0;// null char
  Name=temp;
  //OUTPUT_DEBUG_MSG("Creating Animation named: "<<Name);
  file.read(reinterpret_cast<char*>(&Duration), sizeof(Duration));// the duration
  file.read(reinterpret_cast<char*>(&TicksPerSecond), sizeof(TicksPerSecond));// the number of ticks per second
  file.read(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the number animation channels
  Channels.resize(nsize);
  for(size_t j(0); j< Channels.size(); j++){// for each channel
      nsize =0;
      file.read(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the size of the name
      file.read(temp, nsize);// the size of the animation name
      temp[nsize]=0;// null char
      Channels[j].Name=temp;

      nsize =static_cast<uint32_t>(Channels[j].mPositionKeys.size());
      file.read(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the number of position keys
      Channels[j].mPositionKeys.resize(nsize);
      for(size_t i(0); i< nsize; i++){// for each channel
          file.read(reinterpret_cast<char*>(&Channels[j].mPositionKeys[i].mTime), sizeof(Channels[j].mPositionKeys[i].mTime));// pos key
          file.read(reinterpret_cast<char*>(&Channels[j].mPositionKeys[i].mValue), sizeof(Channels[j].mPositionKeys[i].mValue));// pos key
        }

      nsize =static_cast<uint32_t>(Channels[j].mRotationKeys.size());
      file.read(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the number of position keys
      Channels[j].mRotationKeys.resize(nsize);
      for(size_t i(0); i< nsize; i++){// for each channel
          file.read(reinterpret_cast<char*>(&Channels[j].mRotationKeys[i].mTime), sizeof(Channels[j].mRotationKeys[i].mTime));// pos key
          file.read(reinterpret_cast<char*>(&Channels[j].mRotationKeys[i].mValue), sizeof(Channels[j].mRotationKeys[i].mValue));// pos key
        }

      nsize =static_cast<uint32_t>(Channels[j].mScalingKeys.size());
      file.read(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the number of position keys
      Channels[j].mScalingKeys.resize(nsize);
      for(size_t i(0); i< nsize; i++){// for each channel
          file.read(reinterpret_cast<char*>(&Channels[j].mScalingKeys[i].mTime), sizeof(Channels[j].mScalingKeys[i].mTime));// pos key
          file.read(reinterpret_cast<char*>(&Channels[j].mScalingKeys[i].mValue), sizeof(Channels[j].mScalingKeys[i].mValue));// pos key
        }
    }
  mLastPositions.resize(Channels.size(), std::make_tuple(0, 0, 0));
}

// ------------------------------------------------------------------------------------------------
// Evaluates the animation tracks for a given time stamp.
void cAnimEvaluator::Evaluate(float pTime, std::map<std::string, cBone*>& bones) {

  pTime *= TicksPerSecond;

  float time = 0.0f;
  if(Duration > 0.0)
    time = fmod(pTime, Duration);

  // calculate the transformations for each animation channel
  for(unsigned int a = 0; a < Channels.size(); a++){
      const cAnimationChannel* channel = &Channels[a];
      std::map<std::string, cBone*>::iterator bonenode = bones.find(channel->Name);

      if(bonenode == bones.end()) {
          //OUTPUT_DEBUG_MSG("did not find the bone node "<<channel->Name);
          continue;
        }

      // ******** Position *****
      aiVector3D presentPosition(0, 0, 0);
      if(channel->mPositionKeys.size() > 0){
          // Look for present frame number. Search from last position if time is after the last time, else from beginning
          // Should be much quicker than always looking from start for the average use case.
          unsigned int frame = (time >= mLastTime) ? std::get<0>(mLastPositions[a]): 0;
          while(frame < channel->mPositionKeys.size() - 1){
              if(time < channel->mPositionKeys[frame+1].mTime){
                  break;
                }
              frame++;
            }

          // interpolate between this frame's value and next frame's value
          unsigned int nextFrame = (frame + 1) % channel->mPositionKeys.size();

          const aiVectorKey& key = channel->mPositionKeys[frame];
          const aiVectorKey& nextKey = channel->mPositionKeys[nextFrame];
          double diffTime = nextKey.mTime - key.mTime;
          if(diffTime < 0.0)
            diffTime += Duration;
          if(diffTime > 0) {
              float factor = float((time - key.mTime) / diffTime);
              presentPosition = key.mValue + (nextKey.mValue - key.mValue) * factor;
            } else {
              presentPosition = key.mValue;
            }
          std::get<0>(mLastPositions[a]) = frame;
        }
      // ******** Rotation *********
      aiQuaternion presentRotation(1, 0, 0, 0);
      if(channel->mRotationKeys.size() > 0)
        {
          unsigned int frame = (time >= mLastTime) ? std::get<1>(mLastPositions[a]) : 0;
          while(frame < channel->mRotationKeys.size()  - 1){
              if(time < channel->mRotationKeys[frame+1].mTime)
                break;
              frame++;
            }

          // interpolate between this frame's value and next frame's value
          unsigned int nextFrame = (frame + 1) % channel->mRotationKeys.size() ;

          const aiQuatKey& key = channel->mRotationKeys[frame];
          const aiQuatKey& nextKey = channel->mRotationKeys[nextFrame];
          double diffTime = nextKey.mTime - key.mTime;
          if(diffTime < 0.0) diffTime += Duration;
          if(diffTime > 0) {
              float factor = float((time - key.mTime) / diffTime);
              aiQuaternion::Interpolate(presentRotation, key.mValue, nextKey.mValue, factor);
            } else presentRotation = key.mValue;
          std::get<1>(mLastPositions[a]) = frame;
        }

      // ******** Scaling **********
      aiVector3D presentScaling(1, 1, 1);
      if(channel->mScalingKeys.size() > 0) {
          unsigned int frame = (time >= mLastTime) ? std::get<2>(mLastPositions[a]) : 0;
          while(frame < channel->mScalingKeys.size() - 1){
              if(time < channel->mScalingKeys[frame+1].mTime)
                break;
              frame++;
            }

          presentScaling = channel->mScalingKeys[frame].mValue;
          std::get<2>(mLastPositions[a]) = frame;
        }

      aiMatrix4x4 mat = aiMatrix4x4(presentRotation.GetMatrix());

      mat.a1 *= presentScaling.x; mat.b1 *= presentScaling.x; mat.c1 *= presentScaling.x;
      mat.a2 *= presentScaling.y; mat.b2 *= presentScaling.y; mat.c2 *= presentScaling.y;
      mat.a3 *= presentScaling.z; mat.b3 *= presentScaling.z; mat.c3 *= presentScaling.z;
      mat.a4 = presentPosition.x; mat.b4 = presentPosition.y; mat.c4 = presentPosition.z;
      mat.Transpose();

      TransformMatrix(bonenode->second->LocalTransform, mat);
    }
  mLastTime = time;
}

void SceneAnimator::Release(){// this should clean everything up
  CurrentAnimIndex = -1;
  Animations.clear();// clear all animations
  delete Skeleton;// This node will delete all children recursivly
  Skeleton = NULL;// make sure to zero it out
}

bool SceneAnimator::Init(const aiScene* pScene){// this will build the skeleton based on the scene passed to it and CLEAR EVERYTHING
  if(!pScene->HasAnimations()) return false;
  Release();

  Skeleton = CreateBoneTree(pScene->mRootNode, NULL);
  ExtractAnimations(pScene);

  for (unsigned int i = 0; i < pScene->mNumMeshes;++i){
      const aiMesh* mesh = pScene->mMeshes[i];

      for (unsigned int n = 0; n < mesh->mNumBones;++n){
          const aiBone* bone = mesh->mBones[n];
          std::map<std::string, cBone*>::iterator found = BonesByName.find(bone->mName.data);
          if(found != BonesByName.end()){// FOUND IT!!! woohoo, make sure its not already in the bone list
              bool skip = false;
              for(size_t j(0); j< Bones.size(); j++){
                  std::string bname = bone->mName.data;
                  if(Bones[j]->Name == bname) {
                      skip = true;// already inserted, skip this so as not to insert the same bone multiple times
                      break;
                    }
                }
              if(!skip){// only insert the bone if it has not already been inserted
                  //std::string tes = found->second->Name;
                  TransformMatrix(found->second->Offset, bone->mOffsetMatrix);
                  //found->second->Offset.Transpose();// transpoce their matrix to get in the correct format
                  found->second->Offset = glm::transpose(found->second->Offset);
                  Bones.push_back(found->second);
                  BonesToIndex[found->first] = (unsigned int)Bones.size()-1;
                }
            }
        }
    }
  Transforms.resize(Bones.size());
  float timestep = 1.0f/30.0f;// 30 per second
  for(size_t i(0); i< Animations.size(); i++){// pre calculate the animations
      SetAnimIndex((unsigned int)i);
      float dt = 0;
      for(float ticks = 0; ticks < Animations[i].Duration; ticks += Animations[i].TicksPerSecond/30.0f){
          dt +=timestep;
          Calculate(dt);
          Animations[i].Transforms.push_back(std::vector<glm::mat4>());
          std::vector<glm::mat4>& trans = Animations[i].Transforms.back();
          for(size_t a = 0; a < Transforms.size(); ++a){
              glm::mat4 rotationmat =  Bones[a]->Offset * Bones[a]->GlobalTransform;
              trans.push_back(rotationmat);
            }
        }
    }
  return true;
  //OUTPUT_DEBUG_MSG("Finished loading animations with "<<Bones.size()<<" bones");
}

void SceneAnimator::Save(std::string const &file){
  std::ofstream fd(file, std::ios_base::binary | std::ios_base::trunc);

  if (fd.is_open()) {
      this->Save(fd);
      fd.close();
    }
}

void SceneAnimator::Save(std::ofstream& file){
  // first recursivly save the skeleton
  if(Skeleton)
    SaveSkeleton(file, Skeleton);

  uint32_t nsize = static_cast<uint32_t>(Animations.size());
  file.write(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the number of animations
  for(uint32_t i(0); i< nsize; i++){
      Animations[i].Save(file);
    }

  nsize = static_cast<uint32_t>(Bones.size());
  file.write(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the number of bones

  for(uint32_t i(0); i< Bones.size(); i++){
      nsize = static_cast<uint32_t>(Bones[i]->Name.size());
      file.write(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the size of the bone name
      file.write(Bones[i]->Name.c_str(), nsize);// the name of the bone
    }
}

void SceneAnimator::Load(std::string const &file){
  std::ifstream fd(file);

  if (fd.is_open()) {
      this->Load(fd);
      fd.close();
    }
}

void SceneAnimator::Load(std::ifstream& file){
  Release();// make sure to clear this before writing new data
  Skeleton = LoadSkeleton(file, NULL);
  uint32_t nsize = 0;
  file.read(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the number of animations
  Animations.resize(nsize);
  //OUTPUT_DEBUG_MSG("Extracting Animations . . ");
  for(uint32_t i(0); i< nsize; i++){
      Animations[i].Load(file);
    }
  for(uint32_t i(0); i< Animations.size(); i++){// get all the animation names so I can reference them by name and get the correct id
      AnimationNameToId.insert(std::map<std::string, uint32_t>::value_type(Animations[i].Name, i));
    }
  if(Animations.size() >0) CurrentAnimIndex =0;// set it to the first animation if there are any
  char bname[250];
  file.read(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the number of bones
  Bones.resize(nsize);

  for(uint32_t i(0); i< Bones.size(); i++){
      file.read(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the size of the bone name
      file.read(bname, nsize);// the size of the bone name
      bname[nsize]=0;
      std::map<std::string, cBone*>::iterator found = BonesByName.find(bname);
      BonesToIndex[found->first] = i;
      cBone* tep = found->second;
      Bones[i]=tep;
    }

  Transforms.resize(Bones.size());
  float timestep = 1.0f/30.0f;// 30 per second
  for(size_t i(0); i< Animations.size(); i++){// pre calculate the animations
      SetAnimIndex((unsigned int)i);
      float dt = 0;
      for(float ticks = 0; ticks < Animations[i].Duration; ticks += Animations[i].TicksPerSecond/30.0f){
          dt +=timestep;
          Calculate(dt);
          Animations[i].Transforms.push_back(std::vector<glm::mat4>());
          std::vector<glm::mat4>& trans = Animations[i].Transforms.back();
          for(size_t a = 0; a < Transforms.size(); ++a){
              glm::mat4 rotationmat =  Bones[a]->Offset * Bones[a]->GlobalTransform;
              trans.push_back(rotationmat);
            }
        }
    }
  //OUTPUT_DEBUG_MSG("Finished loading animations with "<<Bones.size()<<" bones");
}

void SceneAnimator::ExtractAnimations(const aiScene* pScene){
  //OUTPUT_DEBUG_MSG("Extracting Animations . . ");
  for(size_t i(0); i< pScene->mNumAnimations; i++){
      Animations.push_back(cAnimEvaluator(pScene->mAnimations[i]) );// add the animations
    }
  for(uint32_t i(0); i< Animations.size(); i++){// get all the animation names so I can reference them by name and get the correct id
      AnimationNameToId.insert(std::map<std::string, uint32_t>::value_type(Animations[i].Name, i));
    }
  CurrentAnimIndex=0;
  SetAnimation("Idle");
}

bool SceneAnimator::SetAnimation(const std::string& name){
  std::map<std::string, uint32_t>::iterator itr = AnimationNameToId.find(name);
  int32_t oldindex = CurrentAnimIndex;
  if(itr !=AnimationNameToId.end()) CurrentAnimIndex = itr->second;
  return oldindex != CurrentAnimIndex;
}

bool SceneAnimator::SetAnimIndex(int32_t  pAnimIndex){
  if((size_t)pAnimIndex >= Animations.size()) return false;// no change, or the animations data is out of bounds
  int32_t oldindex = CurrentAnimIndex;
  CurrentAnimIndex = pAnimIndex;// only set this after the checks for good data and the object was actually inserted
  return oldindex != CurrentAnimIndex;
}

// ------------------------------------------------------------------------------------------------
// Calculates the node transformations for the scene.
void SceneAnimator::Calculate(float &pTime){
  if((CurrentAnimIndex < 0) | ((size_t)CurrentAnimIndex >= Animations.size()) ) return;// invalid animation

  Animations[CurrentAnimIndex].Evaluate(pTime, BonesByName);
  UpdateTransforms(Skeleton);
}

/*void UQTtoUDQ(vec4* dual, quat q, vec4& tran ) {
  dual[0].x = q.x ;
  dual[0].y = q.y ;
  dual[0].z = q.z ;
  dual[0].w = q.w ;
  dual[1].x = 0.5f *  (tran[0] * q.w + tran[1] * q.z - tran[2] * q.y ) ;
  dual[1].y = 0.5f *  (-tran[0] * q.z + tran[1] * q.w + tran[2] * q.x ) ;
  dual[1].z = 0.5f *  (tran[0] * q.y - tran[1] * q.x + tran[2] * q.w ) ;
  dual[1].w = -0.5f * (tran[0] * q.x + tran[1] * q.y + tran[2] * q.z ) ;
}*/

// ------------------------------------------------------------------------------------------------
// Calculates the bone matrices for the given mesh.
void SceneAnimator::CalcBoneMatrices(){
  for(size_t a = 0; a < Transforms.size(); ++a){
      Transforms[a] =  Bones[a]->Offset * Bones[a]->GlobalTransform;
      /*
                mat4 rotationmat = Transforms[a];
                quat q;
                q.frommatrix(rotationmat);

                vec4 dual[2] ;
                UQTtoUDQ(dual, q, Transforms[a].row3_v);
                QuatTransforms[a].row0_v =dual[0];
                QuatTransforms[a].row1_v =dual[1];
                */
    }

}

// ------------------------------------------------------------------------------------------------
// Recursively creates an internal node structure matching the current scene and animation.
cBone* SceneAnimator::CreateBoneTree(aiNode* pNode, cBone* pParent){
  cBone* internalNode = new cBone();// create a node
  internalNode->Name = pNode->mName.data;// get the name of the bone
  internalNode->Parent = pParent; //set the parent, in the case this is theroot node, it will be null

  BonesByName[internalNode->Name] = internalNode;// use the name as a key
  TransformMatrix(internalNode->LocalTransform, pNode->mTransformation);
  //internalNode->LocalTransform.Transpose();
  internalNode->LocalTransform = glm::transpose(internalNode->LocalTransform);
  internalNode->OriginalLocalTransform = internalNode->LocalTransform;// a copy saved
  CalculateBoneToWorldTransform(internalNode);

  // continue for all child nodes and assign the created internal nodes as our children
  for(unsigned int a = 0; a < pNode->mNumChildren; a++){// recursivly call this function on all children
      internalNode->Children.push_back(CreateBoneTree(pNode->mChildren[a], internalNode));
    }
  return internalNode;
}

// ------------------------------------------------------------------------------------------------
// Recursively updates the internal node transformations from the given matrix array
void SceneAnimator::UpdateTransforms(cBone* pNode) {
  CalculateBoneToWorldTransform(pNode);// update global transform as well
  for(std::vector<cBone*>::iterator it = pNode->Children.begin(); it != pNode->Children.end(); ++it)// continue for all children
    UpdateTransforms(*it);
}

// ------------------------------------------------------------------------------------------------
// Calculates the global transformation matrix for the given internal node
void SceneAnimator::CalculateBoneToWorldTransform(cBone* child){
  child->GlobalTransform = child->LocalTransform;
  cBone* parent = child->Parent;
  while(parent ){// this will climb the nodes up along through the parents concentating all the matrices to get the Object to World transform, or in this case, the Bone To World transform
      child->GlobalTransform *= parent->LocalTransform;
      parent  = parent->Parent;// get the parent of the bone we are working on
    }
}

void SceneAnimator::SaveSkeleton(std::ofstream& file, cBone* parent){
  uint32_t nsize = static_cast<uint32_t>(parent->Name.size());
  file.write(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the number of chars
  file.write(parent->Name.c_str(), nsize);// the name of the bone
  file.write(reinterpret_cast<char*>(&parent->Offset), sizeof(parent->Offset));// the bone offsets
  file.write(reinterpret_cast<char*>(&parent->OriginalLocalTransform), sizeof(parent->OriginalLocalTransform));// original bind pose
  nsize = static_cast<uint32_t>(parent->Children.size());// number of children
  file.write(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the number of children
  for(std::vector<cBone*>::iterator it = parent->Children.begin(); it != parent->Children.end(); ++it)// continue for all children
    SaveSkeleton(file, *it);
}

cBone* SceneAnimator::LoadSkeleton(std::ifstream& file, cBone* parent){
  cBone* internalNode = new cBone();// create a node
  internalNode->Parent = parent; //set the parent, in the case this is theroot node, it will be null
  uint32_t nsize=0;
  file.read(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the number of chars
  char temp[250];
  file.read(temp, nsize);// the name of the bone
  temp[nsize]=0;
  internalNode->Name = temp;
  BonesByName[internalNode->Name] = internalNode;// use the name as a key
  file.read(reinterpret_cast<char*>(&internalNode->Offset), sizeof(internalNode->Offset));// the bone offsets
  file.read(reinterpret_cast<char*>(&internalNode->OriginalLocalTransform), sizeof(internalNode->OriginalLocalTransform));// original bind pose

  internalNode->LocalTransform = internalNode->OriginalLocalTransform;// a copy saved
  CalculateBoneToWorldTransform(internalNode);

  file.read(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the number of children

  // continue for all child nodes and assign the created internal nodes as our children
  for(unsigned int a = 0; a < nsize && file; a++){// recursivly call this function on all children
      internalNode->Children.push_back(LoadSkeleton(file, internalNode));
    }
  return internalNode;
}
