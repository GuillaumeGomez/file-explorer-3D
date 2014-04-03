#include "Model.hpp"

#include <assimp/vector3.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/config.h>
#include <assimp/cimport.h>

#include "shaders/ShaderHandler.hpp"
#include "HandleError.hpp"
#include "String_utils.hpp"
#include "HandleSDL.hpp"

using namespace Object;

const char* s_sShaderVertSource = "#version 120\n"
    "\n"
    "uniform mat4 projectionMatrix;\n"
    "uniform mat4 viewMatrix;\n"
    "uniform mat4 modelMatrix;\n"
    "uniform mat4 boneMatrices[60];\n"
    "uniform float zoom;\n"
    "\n"
    "attribute vec4 inPosition;\n"
    "attribute vec3 inNormal;\n"
    "attribute vec4 inColor;\n"
    "attribute vec2 inTexCoord;\n"
    "attribute vec4 inBoneWeights;\n"
    "attribute vec4 inBoneIndices;\n"
    "\n"
    "varying vec4 worldPosition;\n"
    "varying vec3 worldNormal;\n"
    "varying vec4 outColor;\n"
    "varying vec2 outTexCoord;\n"
    "\n"
    "void main()\n"
    "{\n"
    "  vec4 boneWeights = inBoneWeights;\n"
    "  boneWeights.w = 1.0 - dot(boneWeights.yxz, vec3(1.0, 1.0, 1.0));\n"
    "\n"
    "  mat4 transformMatrix = boneWeights.x * boneMatrices[int(inBoneIndices.x)];\n"
    "  transformMatrix += boneWeights.y * boneMatrices[int(inBoneIndices.y)];\n"
    "  transformMatrix += boneWeights.z * boneMatrices[int(inBoneIndices.z)];\n"
    "  transformMatrix += boneWeights.w * boneMatrices[int(inBoneIndices.w)];\n"
    "\n"
    "  vec4 newPosition = transformMatrix * inPosition;\n"
    "  vec4 newNormal = transformMatrix * vec4(inNormal, 0.0);\n"
    "	 worldNormal = (newNormal).xyz;\n"
    "\n"
    "  newPosition.x *= zoom;\n"
    "  newPosition.y *= zoom;\n"
    "  newPosition.z *= zoom;\n"
    "	 gl_Position = projectionMatrix * viewMatrix * newPosition;\n"
    "	 worldPosition = newPosition;\n"
    "	 outColor = inColor;\n"
    "	 outTexCoord = inTexCoord;\n"
    "}\n";

const char* s_sShaderFragSource = "#version 120\n"
    "\n"
    //"uniform vec3 lightPosition;\n"
    //"uniform vec4 lightAmbientColor;\n"
    //"uniform vec4 lightDiffuseColor;\n"
    "uniform int useTexture;\n"
    "uniform sampler2D texture;\n"
    "\n"
    "varying vec4 worldPosition;\n"
    "varying vec3 worldNormal;\n"
    "varying vec4 outColor;\n"
    "varying vec2 outTexCoord;\n"
    "\n"
    "void main()\n"
    "{\n"
    "	 vec3 normal = normalize(worldNormal);\n"
    "  vec3 position = worldPosition.xyz - worldPosition.w;\n"
    //"  vec3 lightVector = normalize(lightPosition);\n"
    "  vec4 fragColor;\n"
    "  \n"
    "  if (useTexture == 0)\n"
    "  {\n"
    "	  fragColor = outColor;\n"
    "  } else\n"
    "  {\n"
    "	  fragColor = texture2D(texture, outTexCoord);\n"
    "  }\n"
    "  \n"
    //"  vec4 ambient = fragColor * lightAmbientColor;\n"
    //"  vec4 diffuse = fragColor * lightDiffuseColor * max(0.0, dot(normal, lightVector));\n"
    "  \n"
    //"	 gl_FragColor = ambient + diffuse;\n"
    "gl_FragColor = fragColor;\n"
    "}\n";

Model::Model(Vector3D v, Rotation r, const char *model, float height) : myGLWidget(v, r), m_height(height) {
  m_pScene = NULL;
  m_pAnimator = NULL;
  m_mModelMatrix = glm::mat4(1.0f);
  m_pStore = 0;
  if (!model) {
      HandleError::showError("Invalid filename in AnimatedModel");
      exit(-1);
    }
  m_sModelPath = model;
  m_pause = false;
  m_className = "Model";

  //m_pLight = new Light(vLightPosition, vLightAmbientColor, vLightDiffuseColor);
  //m_pCamera = new Camera(fCameraDistance, fCameraHeight, fCameraAngle);

  //m_mModelMatrix = glm::mat4(1.0f);
}

Model::~Model() {
  for (auto it : m_vTextures)
    delete it;
  for (auto it : m_animations)
    delete it.second;
  if (m_pScene != NULL) {
      aiReleaseImport(m_pScene);
      if (m_pStore)
        aiReleasePropertyStore(m_pStore);
    }
  for (auto it : m_vMeshes)
    delete it;
  /*for (int i = 0; i < m_iNumMeshes; i++) {
      if (m_vTextures[i] != -1) {
          glDeleteTextures(1, &m_vTextures[i]);
        }
    }*/
  /*if (m_vTextures != NULL) {
      delete[] m_vTextures;
    }*/
  /*if (m_pAnimator != NULL) {
      delete m_pAnimator;
    }*/
}

void Model::initializeGL()
{
  if (!(m_shader = ShaderHandler::createShader(s_sShaderVertSource, s_sShaderFragSource))) {
      HandleError::showError("Shader didn't compile in Model class");
      exit(-1);
    }

  m_uniloc_pos = glGetAttribLocation(m_shader->getProgramID(), "inPosition");
  m_uniLoc_normal = glGetAttribLocation(m_shader->getProgramID(), "inNormal");
  m_uniLoc_modelView = glGetUniformLocation(m_shader->getProgramID(), "viewMatrix");
  m_uniLoc_projection = glGetUniformLocation(m_shader->getProgramID(), "projectionMatrix");
  m_uniLoc_boneMatrice = glGetUniformLocation(m_shader->getProgramID(), "boneMatrices");
  m_uniLoc_boneWeight = glGetAttribLocation(m_shader->getProgramID(), "inBoneWeights");
  m_uniLoc_boneIndice = glGetAttribLocation(m_shader->getProgramID(), "inBoneIndices");
  m_uniLoc_tex = glGetAttribLocation(m_shader->getProgramID(), "inTexCoord");
  m_uniLoc_color = glGetAttribLocation(m_shader->getProgramID(), "inColor");
  m_uniLoc_useTex = glGetUniformLocation(m_shader->getProgramID(), "useTexture");
  m_uniLoc_zoom = glGetUniformLocation(m_shader->getProgramID(), "zoom");

  //import the model via Assimp
  /*m_pStore = aiCreatePropertyStore();
  aiSetImportPropertyInteger(m_pStore, AI_CONFIG_IMPORT_TER_MAKE_UVS, 1);
  aiSetImportPropertyFloat(m_pStore, AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.0f);
  aiSetImportPropertyInteger(m_pStore, AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);

  m_pScene = aiImportFileExWithProperties(m_sModelPath.c_str(),
                                          aiProcessPreset_TargetRealtime_Quality |
                                          aiProcess_FindInstances |
                                          aiProcess_ValidateDataStructure |
                                          aiProcess_OptimizeMeshes |
                                          aiProcess_JoinIdenticalVertices,
                                          NULL,
                                          m_pStore);*/
  m_pScene = aiImportFile(m_sModelPath.c_str(),
                          aiProcessPreset_TargetRealtime_Quality |
                          aiProcess_FindInstances |
                          aiProcess_ValidateDataStructure |
                          aiProcess_OptimizeMeshes |
                          aiProcess_JoinIdenticalVertices);

  if (!m_pScene) {
      HandleError::showError(m_sModelPath + std::string(": ") + aiGetErrorString());
      exit(-1);
    }

  m_iNumMeshes = m_pScene->mNumMeshes;

  //load the textures into the vram
  m_vTextures.resize(m_pScene->mNumMaterials);

  struct aiString* sTexturePath = (aiString*) malloc(sizeof(struct aiString));
  std::vector<std::string> t = Utility::split<std::string>(m_sModelPath, "/");

  if (t.size() > 1)
    t.pop_back();
  std::string tex_path = Utility::join<std::string>(t, "/") + "/";

  for (int i = 0; i < (int) m_pScene->mNumMaterials; i++) {
      m_vTextures[i] = 0;
      aiMaterial* pMat = m_pScene->mMaterials[i];

      if (aiReturn_SUCCESS != aiGetMaterialTexture(pMat, aiTextureType_DIFFUSE, 0, sTexturePath, 0, 0, 0, 0, 0, 0))
        continue;

      std::string filepath = tex_path + sTexturePath->C_Str();

      if (!HandleFile::exists(filepath.c_str())) {
          std::vector<std::string>  t = Utility::split<std::string>(sTexturePath->C_Str(), "/");

          filepath = tex_path + t[t.size() - 1];
        }
      /*Texture *tmp_tex = new Texture(tex_path + sTexturePath->C_Str());
      tmp_tex->load();
      m_vTextures[i] = tmp_tex;*/

      //if (m_pScene->HasAnimations()) {
      SDL_Surface* pTex = HandleSDL::loadImage(filepath);

      if (pTex) {
          GLuint t;
          glGenTextures(1, &t);
          glBindTexture(GL_TEXTURE_2D, t);

          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

          GLenum codagePixel;
          if (pTex->format->Rmask == 0x000000ff)
            {
              if (pTex->format->BytesPerPixel > 3)
                codagePixel = GL_RGBA;
              else
                codagePixel = GL_RGB;
            }
          else
            {
#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif
              if (pTex->format->BytesPerPixel > 3)
                codagePixel = GL_BGRA;
              else
                codagePixel = GL_BGR;
            }
          /*gluBuild2DMipmaps(GL_TEXTURE_2D, pTex->format->BytesPerPixel, pTex->w, pTex->h,
                            codagePixel, GL_UNSIGNED_BYTE, pTex->pixels);*/

          glTexImage2D(GL_TEXTURE_2D, 0, pTex->format->BytesPerPixel, pTex->w, pTex->h,
                       0, codagePixel, GL_UNSIGNED_BYTE, pTex->pixels);

          SDL_FreeSurface(pTex);

          Texture *tmp_tex = new Texture(t);
          std::cerr << filepath << ": OK" << std::endl;
          m_vTextures[i] = tmp_tex;
        } else {
          std::cerr << filepath << ": problem with this file" << std::endl;
        }
      /*} else {
          if (HandleFile::exists(filepath.c_str())) {
              Texture *tmp_tex = new Texture(filepath);
              tmp_tex->load();
              m_vTextures[i] = tmp_tex;
            }
        }*/
    }

  free(sTexturePath);

  float min(0.f), max(0.f);

  for (int i = 0; i < m_iNumMeshes; i++) {
      aiMesh* pCurrentMesh = m_pScene->mMeshes[i];
      Mesh* pNewMesh = new Mesh();

      pNewMesh->m_iNumFaces = pCurrentMesh->mNumFaces;
      pNewMesh->m_iNumVertices = pCurrentMesh->mNumVertices;
      pNewMesh->m_iNumBones = pCurrentMesh->mNumBones;
      pNewMesh->m_iMaterialIndex = pCurrentMesh->mMaterialIndex;

      if ((pNewMesh->m_iNumFaces == 0) || (pNewMesh->m_iNumVertices == 0)) {
          continue;
        }
      if (!pCurrentMesh->HasPositions()) {
          std::cerr << "A mesh of the model has no vertices and is not loaded." << std::endl;
          continue;
        }
      if (!pCurrentMesh->HasNormals()) {
          std::cerr << "A mesh of the model has no normals and is not loaded." << std::endl;
          continue;
        }
      if (!pCurrentMesh->HasFaces()) {
          std::cerr << "A mesh of the model has no polygon faces and is not loaded." << std::endl;
          continue;
        }

      m_vMeshes.push_back(pNewMesh);

      pNewMesh->m_pVertices = new glm::vec4[pNewMesh->m_iNumVertices];
      pNewMesh->m_pNormals = new glm::vec3[pNewMesh->m_iNumVertices];
      pNewMesh->m_pColors = new glm::vec4[pNewMesh->m_iNumVertices];
      pNewMesh->m_pTexCoords = new glm::vec2[pNewMesh->m_iNumVertices];
      pNewMesh->m_pIndices = new GLint[pNewMesh->m_iNumFaces * 3];
      pNewMesh->m_pBoneIndices = new glm::vec4[pNewMesh->m_iNumVertices];
      pNewMesh->m_pWeights = new glm::vec4[pNewMesh->m_iNumVertices];

      for (int j = 0; j < pNewMesh->m_iNumVertices; j++) {
          pNewMesh->m_pVertices[j].x = pCurrentMesh->mVertices[j].x;
          pNewMesh->m_pVertices[j].y = pCurrentMesh->mVertices[j].y;
          pNewMesh->m_pVertices[j].z = pCurrentMesh->mVertices[j].z;
          pNewMesh->m_pVertices[j].w = 1.0f;

          if (pCurrentMesh->mVertices[j].y > max) {
              max = pCurrentMesh->mVertices[j].y;
            } else if (pCurrentMesh->mVertices[j].y < min) {
              min = pCurrentMesh->mVertices[j].y;
            }

          pNewMesh->m_pNormals[j].x = pCurrentMesh->mNormals[j].x;
          pNewMesh->m_pNormals[j].y = pCurrentMesh->mNormals[j].y;
          pNewMesh->m_pNormals[j].z = pCurrentMesh->mNormals[j].z;

          if ((!pCurrentMesh->HasVertexColors(0)) || (pCurrentMesh->mColors[j] == NULL)) {
              if ((pCurrentMesh->mMaterialIndex < m_pScene->mNumMaterials) &&
                  (m_pScene->mMaterials[pCurrentMesh->mMaterialIndex] != NULL)) {
                  aiMaterial * pCurrentMaterial = m_pScene->mMaterials[pCurrentMesh->mMaterialIndex];
                  aiColor4D color(0.5f, 0.5f, 0.5f, 1.0f);
                  aiGetMaterialColor(pCurrentMaterial, AI_MATKEY_COLOR_DIFFUSE, &color);

                  pNewMesh->m_pColors[j].r = color.r;
                  pNewMesh->m_pColors[j].g = color.g;
                  pNewMesh->m_pColors[j].b = color.b;
                  pNewMesh->m_pColors[j].a = 1.0f;
                }
              else {
                  pNewMesh->m_pColors[j].r = 0.5f;
                  pNewMesh->m_pColors[j].g = 0.5f;
                  pNewMesh->m_pColors[j].b = 0.5f;
                  pNewMesh->m_pColors[j].a = 1.0f;
                }
            }
          else {
              pNewMesh->m_pColors[j].r = pCurrentMesh->mColors[j]->r;
              pNewMesh->m_pColors[j].g = pCurrentMesh->mColors[j]->g;
              pNewMesh->m_pColors[j].b = pCurrentMesh->mColors[j]->b;
              pNewMesh->m_pColors[j].a = pCurrentMesh->mColors[j]->a;
            }

          if ((pCurrentMesh->mTextureCoords != NULL) && (pCurrentMesh->mTextureCoords[0] != NULL)) {
              pNewMesh->m_pTexCoords[j].x = pCurrentMesh->mTextureCoords[0][j].x;
              pNewMesh->m_pTexCoords[j].y = 1.0f - pCurrentMesh->mTextureCoords[0][j].y;
            }
          else {
              pNewMesh->m_pTexCoords[j].x = 0.0f;
              pNewMesh->m_pTexCoords[j].y = 0.0f;
            }
        }

      for (int j = 0; j < pNewMesh->m_iNumFaces; j++) {
          pNewMesh->m_pIndices[j * 3] = pCurrentMesh->mFaces[j].mIndices[0];
          pNewMesh->m_pIndices[j * 3 + 1] = pCurrentMesh->mFaces[j].mIndices[1];
          pNewMesh->m_pIndices[j * 3 + 2] = pCurrentMesh->mFaces[j].mIndices[2];
        }

      //read bone indices and weights for bone animation
      std::vector<aiVertexWeight> * vTempWeightsPerVertex = new std::vector<aiVertexWeight>[pCurrentMesh->mNumVertices];

      for (unsigned int j = 0; j < pCurrentMesh->mNumBones; j++) {
          const aiBone * pBone = pCurrentMesh->mBones[j];

          for (unsigned int b = 0; b < pBone->mNumWeights; b++) {
              vTempWeightsPerVertex[pBone->mWeights[b].mVertexId].push_back(aiVertexWeight(j, pBone->mWeights[b].mWeight));
            }
        }

      for (int j = 0; j < pNewMesh->m_iNumVertices; j++) {
          pNewMesh->m_pBoneIndices[j] = glm::uvec4(0, 0, 0, 0);
          pNewMesh->m_pWeights[j] = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

          if (pCurrentMesh->HasBones()) {
              if (vTempWeightsPerVertex[j].size() > 4) {
                  std::cerr << "The model has invalid bone weights and is not loaded." << std::endl;

                  return;
                  //return false;
                }

              for (unsigned int k = 0; k < vTempWeightsPerVertex[j].size(); k++) {
                  pNewMesh->m_pBoneIndices[j][k] = (GLfloat) vTempWeightsPerVertex[j][k].mVertexId;
                  pNewMesh->m_pWeights[j][k] = (GLfloat) vTempWeightsPerVertex[j][k].mWeight;
                }
            }
        }

      if (vTempWeightsPerVertex != NULL) {
          delete[] vTempWeightsPerVertex;
          vTempWeightsPerVertex = NULL;
        }
    }

  m_zoom = m_height / (max - min);

  /*for (Mesh *mesh : m_vMeshes) {
      for (int j = 0; j < mesh->m_iNumVertices; j++) {
          mesh->m_pVertices[j].x *= zoom;
          mesh->m_pVertices[j].y *= zoom;
          mesh->m_pVertices[j].z *= zoom;
        }
    }*/

  if (m_pScene->HasAnimations()) {
      for (unsigned int i = 0; i < m_pScene->mNumAnimations; ++i) {
          Animator *tmp = new Animator(m_pScene, i);

          m_animations[std::string(m_pScene->mAnimations[i]->mName.C_Str())] = tmp;
          m_pAnimator = tmp;
        }
    }
}

void  Model::play()
{
  m_pause = false;
}

void  Model::pause()
{
  m_pause = true;
}

void  Model::update(const float &f)
{
  if (!m_pause) {
      g_lLastTime = f;
      //set the bone animation to the specified timestamp
      if (m_pAnimator != NULL) {
          //long lTimeNow = SDL_GetTicks();
          //long lTimeDifference = lTimeNow - g_lLastTime;
          //g_lLastTime = lTimeNow;
          g_lElapsedTime += g_lLastTime;

          m_pAnimator->UpdateAnimation(g_lElapsedTime, ANIMATION_TICKS_PER_SECOND);
        }
    }
}

bool  Model::setCurrentAnimation(const char *animationName)
{
  if (m_pScene->HasAnimations() && animationName) {
      std::string s = animationName;
      Animator *tmp = m_animations[s];

      if (!tmp)
        return false;
      m_pAnimator = tmp;
      return true;
    }
  return false;
}

bool  Model::cutAnimation(const char *animationName, const char *cutAnimationName, float from, float to)
{
  if (!m_pScene->HasAnimations() || !animationName || !cutAnimationName || from >= to || to <= 0.f)
    return false;
  std::string s = animationName, s2 = cutAnimationName;

  if (s == s2)
    return false;

  from /= float(ANIMATION_TICKS_PER_SECOND);
  to /= float(ANIMATION_TICKS_PER_SECOND);

  Animator *tmp = m_animations[s2];
  if (tmp)
    return false;

  tmp = m_animations[s];

  if (!tmp)
    return false;
  Animator *an = new Animator(m_pScene, tmp->getAnimationIndex(), from, to);
  m_animations[s2] = an;
  return true;
}

void Model::paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix) {
  //m_pShader->Bind();
  glUseProgram(m_shader->getProgramID());

  glm::mat4 tmp = glm::translate(view_matrix, glm::vec3(m_pos.x(), m_pos.y(), m_pos.z()));

  if (m_rot.rotation() != 0.f)
    tmp = glm::rotate(tmp, m_rot.rotation(), glm::vec3(m_rot.x(), m_rot.y(), m_rot.z()));

  glUniformMatrix4fv(m_uniLoc_projection, 1, GL_FALSE, glm::value_ptr(proj_matrix));
  glUniformMatrix4fv(m_uniLoc_modelView, 1, GL_FALSE, glm::value_ptr(tmp));
  glUniform1f(m_uniLoc_zoom, m_zoom);
  //glUniformMatrix4fv(glGetUniformLocation(m_shader->getProgramID(), "modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_mModelMatrix));
  //glUniform3fv(m_pShader->GetLightPositionLocation(), 1, glm::value_ptr(m_pLight->GetPosition()));
  //glUniform4fv(m_pShader->GetLightAmbientColorLocation(), 1, glm::value_ptr(m_pLight->GetAmbient()));
  //glUniform4fv(m_pShader->GetLightDiffuseColorLocation(), 1, glm::value_ptr(m_pLight->GetDiffuse()));

  //draw the model
  if (m_pScene->mRootNode != NULL) {
      RenderNode(m_pScene->mRootNode);
    }

  glUseProgram(0);
}

void Model::RenderNode(aiNode * pNode) {
  for (unsigned int i = 0; i < pNode->mNumMeshes; i++) {
      const aiMesh* pCurrentMesh = m_pScene->mMeshes[pNode->mMeshes[i]];
      glm::mat4* pMatrices = new glm::mat4[MAXBONESPERMESH];

      //upload bone matrices
      if ((pCurrentMesh->HasBones()) && (m_pAnimator != NULL)) {
          const std::vector<aiMatrix4x4>& vBoneMatrices = m_pAnimator->GetBoneMatrices(pNode, i);

          if (vBoneMatrices.size() != pCurrentMesh->mNumBones) {
              continue;
            }

          for (unsigned int j = 0; j < pCurrentMesh->mNumBones; j++) {
              if (j < MAXBONESPERMESH) {
                  pMatrices[j][0][0] = vBoneMatrices[j].a1;
                  pMatrices[j][0][1] = vBoneMatrices[j].b1;
                  pMatrices[j][0][2] = vBoneMatrices[j].c1;
                  pMatrices[j][0][3] = vBoneMatrices[j].d1;
                  pMatrices[j][1][0] = vBoneMatrices[j].a2;
                  pMatrices[j][1][1] = vBoneMatrices[j].b2;
                  pMatrices[j][1][2] = vBoneMatrices[j].c2;
                  pMatrices[j][1][3] = vBoneMatrices[j].d2;
                  pMatrices[j][2][0] = vBoneMatrices[j].a3;
                  pMatrices[j][2][1] = vBoneMatrices[j].b3;
                  pMatrices[j][2][2] = vBoneMatrices[j].c3;
                  pMatrices[j][2][3] = vBoneMatrices[j].d3;
                  pMatrices[j][3][0] = vBoneMatrices[j].a4;
                  pMatrices[j][3][1] = vBoneMatrices[j].b4;
                  pMatrices[j][3][2] = vBoneMatrices[j].c4;
                  pMatrices[j][3][3] = vBoneMatrices[j].d4;
                }
            }
        }

      //upload the complete bone matrices to the shaders
      glUniformMatrix4fv(m_uniLoc_boneMatrice, MAXBONESPERMESH, GL_FALSE, glm::value_ptr(*pMatrices));

      delete pMatrices;

      DrawMesh(pNode->mMeshes[i]);
    }

  //render all child nodes
  for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
      RenderNode(pNode->mChildren[i]);
    }
}

void Model::DrawMesh(unsigned int uIndex) {
  Mesh* pMesh = m_vMeshes.at(uIndex);
  Texture *t(0);

  glEnableVertexAttribArray(m_uniloc_pos);
  glVertexAttribPointer(m_uniloc_pos, 4, GL_FLOAT, GL_FALSE, 0, pMesh->m_pVertices);

  if (m_uniLoc_normal > -1) {
      glEnableVertexAttribArray(m_uniLoc_normal);
      glVertexAttribPointer(m_uniLoc_normal, 3, GL_FLOAT, GL_FALSE, 0, pMesh->m_pNormals);
    }

  glEnableVertexAttribArray(m_uniLoc_boneWeight);
  glVertexAttribPointer(m_uniLoc_boneWeight, 4, GL_FLOAT, GL_FALSE, 0, pMesh->m_pWeights);

  glEnableVertexAttribArray(m_uniLoc_boneIndice);
  glVertexAttribPointer(m_uniLoc_boneIndice, 4, GL_FLOAT, GL_FALSE, 0, pMesh->m_pBoneIndices);

  glEnableVertexAttribArray(m_uniLoc_tex);
  glVertexAttribPointer(m_uniLoc_tex, 2, GL_FLOAT, GL_FALSE, 0, pMesh->m_pTexCoords);

  if (m_vTextures[pMesh->m_iMaterialIndex]) {
      //glActiveTexture(GL_TEXTURE0);
      t = m_vTextures[pMesh->m_iMaterialIndex];
      t->bind();
      //glBindTexture(GL_TEXTURE_2D, m_vTextures[pMesh->m_iMaterialIndex]);

      glUniform1i(m_uniLoc_useTex, 1);
      //glUniform1i(m_uniLoc_tex, 0);
    } else {
      glEnableVertexAttribArray(m_uniLoc_color);
      glVertexAttribPointer(m_uniLoc_color, 4, GL_FLOAT, GL_FALSE, 0, pMesh->m_pColors);
      glUniform1i(m_uniLoc_useTex, 0);
    }

  glDrawElements(GL_TRIANGLES, pMesh->m_iNumFaces * 3, GL_UNSIGNED_INT, pMesh->m_pIndices);
  if (t)
    t->unbind();
}

const char *Model::getCurrentAnimationName() const
{
  if (!m_pAnimator)
    return "";
  aiAnimation *tmp = m_pScene->mAnimations[m_pAnimator->getAnimationIndex()];
  if (!tmp)
    return 0;
  return tmp->mName.C_Str();
}

/*
---------------------------------------------------------------------------
Open Asset Import Library (ASSIMP)
---------------------------------------------------------------------------

Copyright (c) 2006-2010, ASSIMP Development Team

All rights reserved.

Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the following
conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the ASSIMP team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the ASSIMP Development Team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
---------------------------------------------------------------------------
*/

/*
---------------------------------------------------------------------------
This file was modified for the AssimpOpenGLDemo by drivenbynostalgia.com.
---------------------------------------------------------------------------
*/

//---------------------------------------------------------------------------------
//A little tree structure to match the scene's node structure, but holding additional data.
struct AnimationNode {
  std::string sName;
  AnimationNode* pParent;
  std::vector<AnimationNode*> vChildren;

  //most recently calculated local transform
  aiMatrix4x4 mLocalTransform;

  //same, but in world space
  aiMatrix4x4 mGlobalTransform;

  //index in the current animation's channel array.
  int uChannelIndex;

  //default construction
  AnimationNode() {
    uChannelIndex = -1;
    pParent = NULL;
  }

  //construction from a given name
  AnimationNode(const std::string& pName): sName(pName) {
    uChannelIndex = -1;
    pParent = NULL;
  }

  ~AnimationNode() {
    for (std::vector<AnimationNode *>::iterator it = vChildren.begin(); it != vChildren.end(); it++) {
        delete *it;
      }
  }
};

Animator::Animator(const aiScene* pScene, unsigned int uAnimationIndex) {
  m_pScene = pScene;
  m_dLastTime = 0.0;
  m_pRootNode = NULL;
  m_uCurrentAnimationIndex = -1;
  m_pCurrentAnimation = NULL;
  m_pLastFramePosition = NULL;
  m_mIdentityMatrix = aiMatrix4x4();
  m_from = 0.f;
  m_to = m_from;
  m_duration = m_from;

  //build the nodes-for-bones table
  for (unsigned int i = 0; i < m_pScene->mNumMeshes; i++) {
      const aiMesh* pCurrentMesh = m_pScene->mMeshes[i];

      for (unsigned int j = 0; j < pCurrentMesh->mNumBones; j++) {
          const aiBone* pCurrentBone = pCurrentMesh->mBones[j];

          m_mapBoneNodesByName[pCurrentBone->mName.data] = m_pScene->mRootNode->FindNode(pCurrentBone->mName);
        }
    }

  //changing the current animation also creates the node tree for this animation
  SetAnimationIndex(uAnimationIndex);
}

Animator::Animator(const aiScene* pScene, unsigned int uAnimationIndex, float from, float to) {
  m_pScene = pScene;
  m_dLastTime = 0.0;
  m_pRootNode = NULL;
  m_uCurrentAnimationIndex = -1;
  m_pCurrentAnimation = NULL;
  m_pLastFramePosition = NULL;
  m_mIdentityMatrix = aiMatrix4x4();
  m_from = from;
  m_to = to;
  m_duration = m_to - m_from;

  //build the nodes-for-bones table
  for (unsigned int i = 0; i < m_pScene->mNumMeshes; i++) {
      const aiMesh* pCurrentMesh = m_pScene->mMeshes[i];

      for (unsigned int j = 0; j < pCurrentMesh->mNumBones; j++) {
          const aiBone* pCurrentBone = pCurrentMesh->mBones[j];

          m_mapBoneNodesByName[pCurrentBone->mName.data] = m_pScene->mRootNode->FindNode(pCurrentBone->mName);
        }
    }

  //changing the current animation also creates the node tree for this animation
  SetAnimationIndex(uAnimationIndex);
}

Animator::~Animator() {
  if (m_pRootNode != NULL) {
      delete m_pRootNode;
    }

  if (m_pLastFramePosition != NULL) {
      delete[] m_pLastFramePosition;
    }
}

//------------------------------------------------------------------------------------------------
//Sets the animation to use for playback.
void Animator::SetAnimationIndex(unsigned int uAnimationIndex) {
  if (uAnimationIndex == m_uCurrentAnimationIndex) {
      return;
    }

  //kill data of the previous anim
  if (m_pRootNode != NULL) {
      delete m_pRootNode;
      m_pRootNode = NULL;
    }

  if (m_pLastFramePosition != NULL) {
      delete[] m_pLastFramePosition;
    }

  m_mapNodesByName.clear();
  m_pCurrentAnimation = NULL;

  m_uCurrentAnimationIndex = uAnimationIndex;
  m_pCurrentAnimation = m_pScene->mAnimations[m_uCurrentAnimationIndex];

  //create the internal node tree. Do this even in case of invalid animation index
  //so that the transformation matrices are properly set up to mimic the current scene
  m_pRootNode = CreateNodeTree(m_pScene->mRootNode, NULL);

  //invalid anim index
  if (uAnimationIndex >= m_pScene->mNumAnimations) {
      m_uCurrentAnimationIndex = 0;
      m_pCurrentAnimation = m_pScene->mAnimations[m_uCurrentAnimationIndex];
    }

  m_pLastFramePosition = new glm::uvec3[m_pCurrentAnimation->mNumChannels];
}

unsigned int  Animator::getAnimationIndex() const
{
  return m_uCurrentAnimationIndex;
}

//------------------------------------------------------------------------------------------------
//Recursively creates an internal node structure matching the current scene and animation.
AnimationNode* Animator::CreateNodeTree(aiNode* pNode, AnimationNode * pParent) {
  //create a node
  AnimationNode* pInternalNode = new AnimationNode(pNode->mName.data);
  pInternalNode->pParent = pParent;
  m_mapNodesByName[pNode] = pInternalNode;

  //copy its transformation
  pInternalNode->mLocalTransform = pNode->mTransformation;
  CalculateGlobalTransform(pInternalNode);

  //find the index of the animation track affecting this node, if any
  if (m_uCurrentAnimationIndex < m_pScene->mNumAnimations) {
      pInternalNode->uChannelIndex = -1;

      for (unsigned int a = 0; a < m_pCurrentAnimation->mNumChannels; a++) {
          if (m_pCurrentAnimation->mChannels[a]->mNodeName.data == pInternalNode->sName) {
              pInternalNode->uChannelIndex = a;

              break;
            }
        }
    }

  //continue for all child nodes and assign the created internal nodes as our children
  for (unsigned int a = 0; a < pNode->mNumChildren; a++) {
      AnimationNode* childNode = CreateNodeTree(pNode->mChildren[a], pInternalNode);
      pInternalNode->vChildren.push_back(childNode);
    }

  return pInternalNode;
}

//------------------------------------------------------------------------------------------------
//Calculates the bone matrices for the given mesh.
const std::vector<aiMatrix4x4>& Animator::GetBoneMatrices(const aiNode* pNode, unsigned int uNodeMeshIndex) {
  if (uNodeMeshIndex < pNode->mNumMeshes) {
      unsigned int uMeshIndex = pNode->mMeshes[uNodeMeshIndex];

      if ((m_pScene != NULL) && (uMeshIndex < m_pScene->mNumMeshes)) {
          const aiMesh* pMesh = m_pScene->mMeshes[uMeshIndex];

          for (unsigned int i = 0; i < m_vTransforms.size(); i++) {
              m_vTransforms[i] = aiMatrix4x4();
            }

          //resize array and initialise it with identity matrices
          m_vTransforms.resize(pMesh->mNumBones, aiMatrix4x4());

          //calculate the mesh's inverse global transform
          aiMatrix4x4 mGlobalInverseMeshTransform = GetGlobalTransform(pNode);
          mGlobalInverseMeshTransform.Inverse();

          //Bone matrices transform from mesh coordinates in bind pose to mesh coordinates in skinned pose
          //Therefore the formula is offsetMatrix * currentGlobalTransform * inverseCurrentMeshTransform
          for (unsigned int i = 0; i < pMesh->mNumBones; i++) {
              const aiBone* pBone = pMesh->mBones[i];
              const aiMatrix4x4& mCurrentGlobalTransform = GetGlobalTransform(m_mapBoneNodesByName[pBone->mName.data]);
              m_vTransforms[i] = mGlobalInverseMeshTransform * mCurrentGlobalTransform * pBone->mOffsetMatrix;
            }
        }
    }

  //and return the result
  return m_vTransforms;
}

//------------------------------------------------------------------------------------------------
//Calculates the global transformation matrix for the given internal node.
void Animator::CalculateGlobalTransform(AnimationNode* pInternalNode) {
  //concatenate all parent transforms to get the global transform for this node
  pInternalNode->mGlobalTransform = pInternalNode->mLocalTransform;
  AnimationNode* pNode = pInternalNode->pParent;

  while (pNode) {
      pInternalNode->mGlobalTransform = pNode->mLocalTransform * pInternalNode->mGlobalTransform;
      pNode = pNode->pParent;
    }
}

//------------------------------------------------------------------------------------------------
//Retrieves the most recent global transformation matrix for the given node.
const aiMatrix4x4& Animator::GetGlobalTransform(const aiNode * node) const {
  NodeMap::const_iterator it = m_mapNodesByName.find(node);

  if (it == m_mapNodesByName.end()) {
      return m_mIdentityMatrix;
    }

  return it->second->mGlobalTransform;
}

//------------------------------------------------------------------------------------------------
//Calculates the node transformations for the scene.
void Animator::UpdateAnimation(float lElapsedTime, double dTicksPerSecond) {
  if ((m_pCurrentAnimation) && (m_pCurrentAnimation->mDuration > 0.0)) {
      //double dTime = ((double) lElapsedTime) / 1000.0;
      double dTime = lElapsedTime;

      //calculate current local transformations
      //extract ticks per second. Assume default value if not given
      double dTicksPerSecondCorrected = dTicksPerSecond != 0.0 ? dTicksPerSecond : ANIMATION_TICKS_PER_SECOND;

      //map into anim's duration
      double dTimeInTicks = 0.f;

      if (m_pCurrentAnimation->mDuration > 0.0) {
          float tmp_ = (dTime + m_from) * dTicksPerSecondCorrected;

          dTimeInTicks = fmod(tmp_, m_duration > 0.f ? m_duration : m_pCurrentAnimation->mDuration);
        }

      if (m_vTransforms.size() != m_pCurrentAnimation->mNumChannels) {
          m_vTransforms.resize(m_pCurrentAnimation->mNumChannels);
        }

      //calculate the transformations for each animation channel
      for (unsigned int i = 0; i < m_pCurrentAnimation->mNumChannels; i++) {
          const aiNodeAnim* pChannel = m_pCurrentAnimation->mChannels[i];

          //******** Position *****
          aiVector3D vPresentPosition(0, 0, 0);

          if (pChannel->mNumPositionKeys > 0) {
              //Look for present frame number. Search from last position if time is after the last time, else from beginning
              //Should be much quicker than always looking from start for the average use case.
              unsigned int uFrame = (dTimeInTicks >= m_dLastTime) ? m_pLastFramePosition[i].x : m_from;

              while (uFrame < pChannel->mNumPositionKeys - 1) {
                  if (dTimeInTicks < pChannel->mPositionKeys[uFrame + 1].mTime) {
                      break;
                    }

                  uFrame++;
                }

              //interpolate between this frame's value and next frame's value
              unsigned int uNextFrame = (uFrame + 1) % pChannel->mNumPositionKeys;
              const aiVectorKey& pKey = pChannel->mPositionKeys[uFrame];
              const aiVectorKey& pNextKey = pChannel->mPositionKeys[uNextFrame];
              double dTimeDifference = pNextKey.mTime - pKey.mTime;

              if (dTimeDifference < 0.0) {
                  dTimeDifference += m_pCurrentAnimation->mDuration;
                }

              if (dTimeDifference > 0) {
                  float fInterpolationFactor = (float) ((dTimeInTicks - pKey.mTime) / dTimeDifference);
                  vPresentPosition = pKey.mValue + (pNextKey.mValue - pKey.mValue) * fInterpolationFactor;
                }
              else {
                  vPresentPosition = pKey.mValue;
                }

              m_pLastFramePosition[i].x = uFrame;
            }

          //******** Rotation *********
          aiQuaternion qPresentRotation(1, 0, 0, 0);

          if (pChannel->mNumRotationKeys > 0) {
              unsigned int uFrame = (dTimeInTicks >= m_dLastTime) ? m_pLastFramePosition[i].y : 0;

              while (uFrame < pChannel->mNumRotationKeys - 1) {
                  if (dTimeInTicks < pChannel->mRotationKeys[uFrame + 1].mTime) {
                      break;
                    }

                  uFrame++;
                }

              //interpolate between this frame's value and next frame's value
              unsigned int uNextFrame = (uFrame + 1) % pChannel->mNumRotationKeys;
              const aiQuatKey& pKey = pChannel->mRotationKeys[uFrame];
              const aiQuatKey& pNextKey = pChannel->mRotationKeys[uNextFrame];
              double dTimeDifference = pNextKey.mTime - pKey.mTime;

              if (dTimeDifference < 0.0) {
                  dTimeDifference += m_pCurrentAnimation->mDuration;
                }

              if (dTimeDifference > 0) {
                  float fInterpolationFactor = (float) ((dTimeInTicks - pKey.mTime) / dTimeDifference);
                  aiQuaternion::Interpolate(qPresentRotation, pKey.mValue, pNextKey.mValue, fInterpolationFactor);
                }
              else {
                  qPresentRotation = pKey.mValue;
                }

              m_pLastFramePosition[i].y = uFrame;
            }

          //******** Scaling **********
          aiVector3D vPresentScaling(1, 1, 1);

          if (pChannel->mNumScalingKeys > 0) {
              unsigned int uFrame = (dTimeInTicks >= m_dLastTime) ? m_pLastFramePosition[i].z : 0;

              while (uFrame < pChannel->mNumScalingKeys - 1) {
                  if (dTimeInTicks < pChannel->mScalingKeys[uFrame + 1].mTime) {
                      break;
                    }

                  uFrame++;
                }

              vPresentScaling = pChannel->mScalingKeys[uFrame].mValue;
              m_pLastFramePosition[i].z = uFrame;
            }

          //build a transformation matrix from it
          aiMatrix4x4& mTransformation = m_vTransforms[i];
          mTransformation = aiMatrix4x4(qPresentRotation.GetMatrix());
          mTransformation.a1 *= vPresentScaling.x; mTransformation.b1 *= vPresentScaling.x; mTransformation.c1 *= vPresentScaling.x;
          mTransformation.a2 *= vPresentScaling.y; mTransformation.b2 *= vPresentScaling.y; mTransformation.c2 *= vPresentScaling.y;
          mTransformation.a3 *= vPresentScaling.z; mTransformation.b3 *= vPresentScaling.z; mTransformation.c3 *= vPresentScaling.z;
          mTransformation.a4 = vPresentPosition.x; mTransformation.b4 = vPresentPosition.y; mTransformation.c4 = vPresentPosition.z;
        }

      m_dLastTime = dTimeInTicks;

      //and update all node transformations with the results
      UpdateTransforms(m_pRootNode, m_vTransforms);
    }
}

//------------------------------------------------------------------------------------------------
//Recursively updates the internal node transformations from the given matrix array
void Animator::UpdateTransforms(AnimationNode* pNode, const std::vector<aiMatrix4x4>& vTransforms){
  //update node local transform
  if (pNode->uChannelIndex != -1) {
      if (pNode->uChannelIndex >= vTransforms.size()) {
          return;
        }

      pNode->mLocalTransform = vTransforms[pNode->uChannelIndex];
    }

  //concatenate all parent transforms to get the global transform for this node
  pNode->mGlobalTransform = pNode->mLocalTransform;
  AnimationNode* pParentNode = pNode->pParent;

  while (pParentNode) {
      pNode->mGlobalTransform = pParentNode->mLocalTransform * pNode->mGlobalTransform;
      pParentNode = pParentNode->pParent;
    }

  //continue for all children
  for (std::vector<AnimationNode*>::iterator it = pNode->vChildren.begin(); it != pNode->vChildren.end(); it++) {
      UpdateTransforms(*it, vTransforms);
    }
}
