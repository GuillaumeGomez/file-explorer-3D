#ifndef ANIMATEDMODEL_HPP
#define ANIMATEDMODEL_HPP

#include <vector>
#include <assimp/anim.h>
#include <assimp/scene.h>
#include <map>
#include "myGLWidget.hpp"

#define ANIMATION_TICKS_PER_SECOND 30.f
#define MAXBONESPERMESH 60 //This value has to be changed in the shader code as well, boneMatrices[MAXBONESPERMESH]

struct AnimationNode;

//---------------------------------------------------------------------------------
/** Calculates the animated node transformations for a given scene and timestamp.
*
*  Create an instance for a aiScene you want to animate and set the current animation
*  to play. You can then have the instance calculate the current pose for all nodes
*  by calling Calculate() for a given timestamp. After this you can retrieve the
*  present transformation for a given node by calling GetLocalTransform() or
*  GetGlobalTransform().
*/
class Animator
{
public:
  //----------------------------------------------------------------------------
  /** Constructor for a given scene.
  *
  * The object keeps a reference to the scene during its lifetime, but
  * ownership stays at the caller.
  * @param pScene The scene to animate.
  * @param pAnimIndex Index of the animation to play.
  */
  Animator(const aiScene* pScene, unsigned int pAnimIndex);
  Animator(const aiScene* pScene, unsigned int pAnimIndex, float from, float to);
  ~Animator();

  //----------------------------------------------------------------------------
  /** Sets the animation to use for playback. This also recreates the internal
  * mapping structures, which might take a few cycles.
  * @param uAnimationIndex Index of the animation in the scene's animation array
  */
  void SetAnimationIndex(unsigned int uAnimationIndex);
  unsigned int  getAnimationIndex() const;

  //----------------------------------------------------------------------------
  /** Calculates the node transformations for the scene. Call this to get
  * up-to-date results before calling one of the getters.
  * Evaluates the animation tracks for a given time stamp. The calculated pose can be retrieved as a
  * array of transformation matrices afterwards by calling GetTransformations().
  * @param lElapsedTime Elapsed time since animation start in ms.
  */
  void UpdateAnimation(float lElapsedTime, double dTicksPerSecond);

  //----------------------------------------------------------------------------
  /** Calculates the bone matrices for the given mesh.
  *
  * Each bone matrix transforms from mesh space in bind pose to mesh space in
  * skinned pose, it does not contain the mesh's world matrix. Thus the usual
  * matrix chain for using in the vertex shader is
  * @code
  * projMatrix * viewMatrix * worldMatrix * boneMatrix
  * @endcode
  * @param pNode The node carrying the mesh.
  * @param pMeshIndex Index of the mesh in the node's mesh array. The NODE's
  *   mesh array, not  the scene's mesh array! Leave out to use the first mesh
  *   of the node, which is usually also the only one.
  * @return A reference to a vector of bone matrices. Stays stable till the
  *   next call to GetBoneMatrices();
  */
  const std::vector<aiMatrix4x4>& GetBoneMatrices(const aiNode* pNode, unsigned int pMeshIndex = 0);

  // ----------------------------------------------------------------------------
  /** Retrieves the most recent global transformation matrix for the given node.
  *
  * The returned matrix is in world space, which is the same coordinate space
  * as the transformation of the scene's root node. If the node is not animated,
  * the node's original transformation is returned so that you can safely use or
  * assign it to the node itsself. If there is no node with the given name, the
  * identity matrix is returned. All transformations are updated whenever
  * Calculate() is called.
  * @param pNodeName Name of the node
  * @return A reference to the node's most recently calculated global
  *   transformation matrix.
  */
  const aiMatrix4x4& GetGlobalTransform(const aiNode* node) const;

private:
  /** Recursively creates an internal node structure matching the
  *  current scene and animation.
  */
  AnimationNode* CreateNodeTree(aiNode* pNode, AnimationNode* pParent);

  /** Calculates the global transformation matrix for the given internal node */
  void CalculateGlobalTransform(AnimationNode* pInternalNode);

  /** Recursively updates the internal node transformations from the
  *  given matrix array
  */
  void UpdateTransforms(AnimationNode* pNode, const std::vector<aiMatrix4x4>& vTransforms);

  /** Name to node map to quickly find nodes by their name */
  typedef std::map<const aiNode*, AnimationNode*> NodeMap;
  NodeMap m_mapNodesByName;

  /** Name to node map to quickly find nodes for given bones by their name */
  typedef std::map<const char*, const aiNode*> BoneMap;
  BoneMap m_mapBoneNodesByName;

  /** At which frame the last evaluation happened for each channel.
  * Useful to quickly find the corresponding frame for slightly increased time stamps
  */
  double m_dLastTime;
  glm::uvec3* m_pLastFramePosition;

  /** Array to return transformations results inside. */
  std::vector<aiMatrix4x4> m_vTransforms;

  /** Identity matrix to return a reference to in case of error */
  aiMatrix4x4 m_mIdentityMatrix;

  const aiScene* m_pScene;
  AnimationNode* m_pRootNode;
  unsigned int m_uCurrentAnimationIndex;
  const aiAnimation* m_pCurrentAnimation;
  float m_from, m_to;
  double m_duration;
};

struct Mesh {
  Mesh() {
    m_pVertices = NULL;
    m_pNormals = NULL;
    m_pColors = NULL;
    m_pTexCoords = NULL;
    m_pWeights = NULL;
    m_pBoneIndices = NULL;
    m_pIndices = NULL;
    m_iMaterialIndex = -1;
    m_iNumFaces = 0;
    m_iNumVertices = 0;
    m_iNumBones = 0;
    m_uTexture = -1;
  }

  ~Mesh() {
    delete[] m_pVertices;
    m_pVertices = NULL;

    delete[] m_pNormals;
    m_pNormals = NULL;

    delete[] m_pColors;
    m_pColors = NULL;

    delete[] m_pTexCoords;
    m_pTexCoords = NULL;

    delete[] m_pBoneIndices;
    m_pBoneIndices = NULL;

    delete[] m_pWeights;
    m_pWeights = NULL;

    delete[] m_pIndices;
    m_pIndices = NULL;
  }

  glm::vec4* m_pVertices;
  glm::vec3* m_pNormals;
  glm::vec4* m_pColors;
  glm::vec2* m_pTexCoords;
  glm::vec4* m_pWeights;
  glm::vec4* m_pBoneIndices;
  int* m_pIndices;
  int m_iMaterialIndex;
  int m_iNumFaces;
  int m_iNumVertices;
  int m_iNumBones;
  unsigned int m_uTexture;
};

struct aiPropertyStore;

namespace Object {
  class Model : public myGLWidget {
  public:
    Model(Vector3D, Rotation, const char *sModelPath, float height);
    virtual ~Model();

    void  initializeGL();
    void  paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix);
    bool  cutAnimation(const char *animationName, const char *cutAnimationName, float from, float to);
    bool  setCurrentAnimation(const char *animationName);
    const char *getCurrentAnimationName() const;
    void  play();
    void  pause();
    void  update(const float &);

  private:
    void  DrawMesh(unsigned int uIndex);
    void  RenderNode(aiNode* pNode);

    //parameters for assimp
    float m_height;
    GLfloat m_zoom;
    GLint  m_uniloc_pos, m_uniLoc_boneMatrice, m_uniLoc_normal,
    m_uniLoc_boneWeight, m_uniLoc_boneIndice, m_uniLoc_tex, m_uniLoc_color, m_uniLoc_useTex, m_uniLoc_zoom;

    float g_lLastTime;
    float g_lElapsedTime;
    bool  m_pause;
    const struct aiScene* m_pScene;
    struct aiPropertyStore* m_pStore;
    //aiVector3D m_vSceneMin;
    //aiVector3D m_vSceneMax;
    //aiVector3D m_vSceneCenter;

    //parameters to draw the model
    glm::mat4 m_mModelMatrix;
    std::string m_sModelPath;
    int m_iNumMeshes;
    std::vector<Mesh*> m_vMeshes;
    std::vector<Texture*> m_vTextures;
    Animator* m_pAnimator;
    std::map<std::string, Animator*>  m_animations;
  };
}

#endif // ANIMATEDMODEL_HPP
