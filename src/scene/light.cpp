#include <cmath>

#include "light.h"
#include "../ui/TraceUI.h"
extern TraceUI* traceUI;

using namespace std;

double DirectionalLight::distanceAttenuation(const Vec3d& P) const
{
  // distance to light is infinite, so f(di) goes to 0.  Return 1.
  return 1.0;
}


Vec3d DirectionalLight::shadowAttenuation(const ray& r, const Vec3d& p) const
{
  // YOUR CODE HERE:
  // You should implement shadow-handling code here.
  ray R = r;

  isect i;
  if(scene->intersect(R, i)) { //We are occluded
    const Material& m = i.getMaterial();
    if(i.t < RAY_EPSILON) return Vec3d(1,1,1);
    if (m.Trans()){return m.kd(i) %(Vec3d(1,1,1) - m.kt(i));}
    else return Vec3d(0,0,0);
  }else{
    return Vec3d(1,1,1);
  }
}

Vec3d DirectionalLight::getColor() const
{
  return color;
}

Vec3d DirectionalLight::getDirection(const Vec3d& P) const
{
  // for directional light, direction doesn't depend on P
  return -orientation;
}

// Random Sample the sphere
std::tuple<Vec3d,Vec3d> DirectionalLight::firePhoton(void){
  double x = (double)rand()/(double)RAND_MAX;
  double y = (double)rand()/(double)RAND_MAX;
  double z = (double)rand()/(double)RAND_MAX;

  Vec3d sample(x, y, z);
  sample.normalize();

  return std::make_tuple(position, getDirection(sample));
}

double PointLight::distanceAttenuation(const Vec3d& P) const
{

  // YOUR CODE HERE

  // You'll need to modify this method to attenuate the intensity 
  // of the light based on the distance between the source and the 
  // point P.  For now, we assume no attenuation and just return 1.0
  double d = (P - position).length();
  double falloff = constantTerm + (linearTerm + quadraticTerm*d)*d;
  falloff = (fabs(falloff) < RAY_EPSILON) ? 1.0 : 1.0 / falloff;
  return min( 1.0, falloff );

}

Vec3d PointLight::getColor() const
{
  return color;
}

Vec3d PointLight::getDirection(const Vec3d& P) const
{
  Vec3d ret = position - P;
  ret.normalize();
  return ret;
}

// Random Sample the sphere
std::tuple<Vec3d,Vec3d> PointLight::firePhoton(void){
  double x = (double)rand()/(double)RAND_MAX;
  double y = (double)rand()/(double)RAND_MAX;
  double z = (double)rand()/(double)RAND_MAX;

  Vec3d sample(x, y, z);
  sample.normalize();

  return std::make_tuple(position, getDirection(sample));
}
Vec3d PointLight::shadowAttenuation(const ray& r, const Vec3d& p) const
{
  // YOUR CODE HERE:
  // You should implement shadow-handling code here.
  ray R = r;
  // if(traceUI->isDistributionRayTracing()){
  //   Vec3d mDir = R.getDirection();
  //   Vec3d perterbT = CosWeightedRandomHemiDir2(mDir);
  //   R.d = mDir + perterbT;
  //   R.d.normalize();    
  // }

  isect i;
  if(scene->intersect(R, i)) { //We are potentially occluded
    if (i.t < (position - p).length()){
      const Material& m = i.getMaterial();
      if(i.t < RAY_EPSILON) return Vec3d(1,1,1);
    
      if (m.Trans()){
        return m.kd(i)% (Vec3d(1,1,1)- m.kt(i));
      }
      else{
        return Vec3d(0,0,0);
      }
    }
    else return Vec3d(1,1,1);
  }else{
    return Vec3d(1,1,1);
  }

}
