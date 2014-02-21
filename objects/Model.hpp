#ifndef MODEL_HPP
#define MODEL_HPP

#include <stdint.h>

#ifdef WIN32
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int16 int16_t;
typedef unsigned __int16 uint16_t;
#else
#include <stdint.h>
#endif
#include "Utils/MyMath.hpp"

#include "../myGLWidget.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>
#include <fstream>

class Bone_Weight {
public:
  Bone_Weight() {Weights.zero(); Bones[0]=Bones[1]=Bones[2]=Bones[3]=0;}
  float Bones[4];
  vec4  Weights;
};

class Vertex : public Bone_Weight {
public:
  Vertex() : Bone_Weight() {
    toZero(pos, 4);
    toZero(tex, 2);
    toZero(normal, 3);
    toZero(tangent, 3);
    //toZero(boneIndices, 4);
    //toZero(boneWeights, 4);
  }
  void  toZero(float *s, int size)
  {
    while (size-- > 0)
      s[size] = 0.f;
  }

  float pos[4];
  float tex[2];
  float normal[3];
  float tangent[3];
  //float boneIndices[4];
  //float boneWeights[4];
};

class cBone {
public:
  std::string Name;
  glm::mat4 Offset, LocalTransform, GlobalTransform, OriginalLocalTransform;
  cBone* Parent;
  std::vector<cBone*> Children;

  cBone() :Parent(0){}
  ~cBone(){ for(size_t i(0); i< Children.size(); i++) delete Children[i]; }
};
class cAnimationChannel{
public:
  std::string Name;
  std::vector<aiVectorKey> mPositionKeys;
  std::vector<aiQuatKey> mRotationKeys;
  std::vector<aiVectorKey> mScalingKeys;
};

class cAnimEvaluator{
public:
  cAnimEvaluator(): mLastTime(0.0f), TicksPerSecond(0.0f), Duration(0.0f), PlayAnimationForward(true), Animation_Indexer(0) {}
  cAnimEvaluator(const aiAnimation* pAnim);
  void Evaluate(float pTime, std::map<std::string, cBone*>& bones);
  void Save(std::ofstream& file);
  void Save(std::string const& file);
  void Load(std::ifstream& file);
  void Load(std::string const& file);
  std::vector<glm::mat4>& GetTransforms(float dt){ return Transforms[GetFrameIndexAt(dt)]; }
  unsigned int GetFrameIndexAt(float time);

  std::string Name;
  uint32_t Animation_Indexer;// this is only used if an animation has no name. I assigned it Animation + Animation_Indexer
  std::vector<cAnimationChannel> Channels;
  bool PlayAnimationForward;// play forward == true, play backward == false
  float mLastTime, TicksPerSecond, Duration;
  std::vector<std::tuple<unsigned int, unsigned int, unsigned int> > mLastPositions;
  std::vector<std::vector<glm::mat4>> Transforms;//, QuatTransforms;/** Array to return transformations results inside. */
};


class SceneAnimator{
public:
  SceneAnimator(): Skeleton(0), CurrentAnimIndex(-1) {}
  ~SceneAnimator(){ Release(); }

  bool Init(const aiScene* pScene);// this must be called to fill the SceneAnimator with valid data
  void Release();// frees all memory and initializes everything to a default state
  void Save(std::string const &file);
  void Save(std::ofstream& file);
  void Load(std::string const &file);
  void Load(std::ifstream& file);
  bool HasSkeleton() const { return !Bones.empty(); }// lets the caller know if there is a skeleton present
  // the set animation returns whether the animation changed or is still the same.
  bool SetAnimIndex(int32_t pAnimIndex);// this takes an index to set the current animation to
  bool SetAnimation(const std::string& name);// this takes a string to set the animation to, i.e. SetAnimation("Idle");
  // the next two functions are good if you want to change the direction of the current animation. You could use a forward walking animation and reverse it to get a walking backwards
  void PlayAnimationForward() { Animations[CurrentAnimIndex].PlayAnimationForward = true; }
  void PlayAnimationBackward() { Animations[CurrentAnimIndex].PlayAnimationForward = false; }
  //this function will adjust the current animations speed by a percentage. So, passing 100, would do nothing, passing 50, would decrease the speed by half, and 150 increase it by 50%
  void AdjustAnimationSpeedBy(float percent) { Animations[CurrentAnimIndex].TicksPerSecond*=percent/100.0f; }
  //This will set the animation speed
  void AdjustAnimationSpeedTo(float tickspersecond) { Animations[CurrentAnimIndex].TicksPerSecond=tickspersecond; }
  // get the animationspeed... in ticks per second
  float GetAnimationSpeed() const { return Animations[CurrentAnimIndex].TicksPerSecond; }
  // get the transforms needed to pass to the vertex shader. This will wrap the dt value passed, so it is safe to pass 50000000 as a valid number
  std::vector<glm::mat4>& GetTransforms(float const &dt){ return Animations[CurrentAnimIndex].GetTransforms(dt); }

  int32_t GetAnimationIndex() const { return CurrentAnimIndex; }
  std::string GetAnimationName() const { return Animations[CurrentAnimIndex].Name;  }
  //GetBoneIndex will return the index of the bone given its name. The index can be used to index directly into the vector returned from GetTransform
  int GetBoneIndex(const std::string& bname){ std::map<std::string, unsigned int>::iterator found = BonesToIndex.find(bname); if(found!=BonesToIndex.end()) return found->second; else return -1;}
  //GetBoneTransform will return the matrix of the bone given its name and the time. be careful with this to make sure and send the correct dt. If the dt is different from what the model is currently at, the transform will be off
  glm::mat4 GetBoneTransform(float dt, const std::string& bname) { int bindex=GetBoneIndex(bname); if(bindex == -1) return glm::mat4(); return Animations[CurrentAnimIndex].GetTransforms(dt)[bindex]; }
  // same as above, except takes the index
  glm::mat4 GetBoneTransform(float dt, unsigned int bindex) {  return Animations[CurrentAnimIndex].GetTransforms(dt)[bindex]; }

  std::vector<cAnimEvaluator> Animations;// a std::vector that holds each animation
  int32_t CurrentAnimIndex;/** Current animation index */

protected:
  cBone* Skeleton;/** Root node of the internal scene structure */
  std::map<std::string, cBone*> BonesByName;/** Name to node map to quickly find nodes by their name */
  std::map<std::string, unsigned int> BonesToIndex;/** Name to node map to quickly find nodes by their name */
  std::map<std::string, uint32_t> AnimationNameToId;// find animations quickly
  std::vector<cBone*> Bones;// DO NOT DELETE THESE when the destructor runs... THEY ARE JUST COPIES!!
  std::vector<glm::mat4> Transforms;// temp array of transfrorms

  void SaveSkeleton(std::ofstream& file, cBone* pNode);
  cBone* LoadSkeleton(std::ifstream& file, cBone* pNode);

  void UpdateTransforms(cBone* pNode);
  void CalculateBoneToWorldTransform(cBone* pInternalNode);/** Calculates the global transformation matrix for the given internal node */

  void Calculate(float &pTime);
  void CalcBoneMatrices();

  void ExtractAnimations(const aiScene* pScene);
  cBone* CreateBoneTree(aiNode* pNode, cBone* pParent);// Recursively creates an internal node structure matching the current scene and animation.
};

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
    virtual void update(const float &);

  protected:
    bool  load(const aiScene*);
    /*bool  loadMaterial();
    bool  loadFile();*/

    std::string modelName;
    float       m_height;
    SceneAnimator *m_scene;
    GLuint *m_indices;
    std::vector<Vertex> vertices;
    std::vector<std::pair<GLuint, GLuint> >  m_index; //first: index, second: size
    GLuint elementbuffer;
    float dtime;
  };
}

#endif // MODEL_HPP
