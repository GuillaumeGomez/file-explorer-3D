#include "MyMath.hpp"

uint32_t ABSVAL = 0x7FFFFFF;
const vec4 ABSROWVAL(*(reinterpret_cast<float*>(&ABSVAL)), *(reinterpret_cast<float*>(&ABSVAL)), *(reinterpret_cast<float*>(&ABSVAL)), *(reinterpret_cast<float*>(&ABSVAL)) );

//rayDelta is NOT normalized, meaning if you want to test for an aabb hit, you must scale the ray to ensure its long enough to hit it
float RayAABBIntersect(const vec3& min, const vec3& max, const vec3& rayOrg, const vec3& rayDelta){
  // check for point inside box, trivial reject
  const float kNoIntersection = INFINITY;

  // Check for point inside box, trivial reject, and determine parametric
  // distance to each front face

  bool inside = true;

  float xt, xn;
  if (rayOrg.x < min.x) {
      xt = min.x - rayOrg.x;
      if (xt > rayDelta.x) return kNoIntersection;
      xt /= rayDelta.x;
      inside = false;
      xn = -1.0f;
    } else if (rayOrg.x > max.x) {
      xt = max.x - rayOrg.x;
      if (xt < rayDelta.x) return kNoIntersection;
      xt /= rayDelta.x;
      inside = false;
      xn = 1.0f;
    } else {
      xt = -1.0f;
    }

  float yt, yn;
  if (rayOrg.y < min.y) {
      yt = min.y - rayOrg.y;
      if (yt > rayDelta.y) return kNoIntersection;
      yt /= rayDelta.y;
      inside = false;
      yn = -1.0f;
    } else if (rayOrg.y > max.y) {
      yt = max.y - rayOrg.y;
      if (yt < rayDelta.y) return kNoIntersection;
      yt /= rayDelta.y;
      inside = false;
      yn = 1.0f;
    } else {
      yt = -1.0f;
    }

  float zt, zn;
  if (rayOrg.z < min.z) {
      zt = min.z - rayOrg.z;
      if (zt > rayDelta.z) return kNoIntersection;
      zt /= rayDelta.z;
      inside = false;
      zn = -1.0f;
    } else if (rayOrg.z > max.z) {
      zt = max.z - rayOrg.z;
      if (zt < rayDelta.z) return kNoIntersection;
      zt /= rayDelta.z;
      inside = false;
      zn = 1.0f;
    } else {
      zt = -1.0f;
    }

  // Inside box?

  if (inside) {
      return 0.0f;
    }

  // Select farthest plane - this is
  // the plane of intersection.

  int which = 0;
  float t = xt;
  if (yt > t) {
      which = 1;
      t = yt;
    }
  if (zt > t) {
      which = 2;
      t = zt;
    }

  switch (which) {

    case 0: // intersect with yz plane
      {
        float y = rayOrg.y + rayDelta.y*t;
        if (y < min.y || y > max.y) return kNoIntersection;
        float z = rayOrg.z + rayDelta.z*t;
        if (z < min.z || z > max.z) return kNoIntersection;
      } break;

    case 1: // intersect with xz plane
      {
        float x = rayOrg.x + rayDelta.x*t;
        if (x < min.x || x > max.x) return kNoIntersection;
        float z = rayOrg.z + rayDelta.z*t;
        if (z < min.z || z > max.z) return kNoIntersection;
      } break;

    case 2: // intersect with xy plane
      {
        float x = rayOrg.x + rayDelta.x*t;
        if (x < min.x || x > max.x) return kNoIntersection;
        float y = rayOrg.y + rayDelta.y*t;
        if (y < min.y || y > max.y) return kNoIntersection;

      } break;
    }

  // Return parametric point of intersection

  return t;
}
vec2 RayRayIntersect(vec3& ray1, const vec3& origin1, float lengthof2, vec3& ray2, const vec3& origin2){
  vec3 temp(Cross(ray1, ray2));// cross product of the normalized std::vectors
  float det = temp.LengthSq();// magnitude squared
  if(abs(det) < .01f) return vec2(INFINITY, INFINITY);// if det ==0.0f, but there might be floating point precision error, so make sure.. mmkay? this means the two rays are basically parallel
  det=1.0f/det;// inverse
  vec3 p2mp1(origin2-origin1);
  vec3 temp1(Cross(p2mp1, ray1));
  float t2 = Dot(temp1, temp)*det;
  if((t2 < 0.0f )| (t2 > lengthof2)) return vec2(INFINITY, INFINITY);// the rays did not intersect
  temp1 = Cross(p2mp1, ray2);
  float t1 = Dot(temp1, temp)*det;
  return vec2(t1, t2);// returns the distance each ray needs to be scaled to hit each other
}
