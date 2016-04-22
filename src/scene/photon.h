#include "ray.h"
#include "../vecmath/vec.h"

class photon : public ray{
public:

  Vec3f flux;

  photon(const Vec3d &pp, const Vec3d &dd, RayType tt = VISIBILITY, const Vec3d& ff)
    : p(pp), d(dd), t(tt), flux(ff) {}
  photon(const photon& other): p(other.p), d(other.d), t(other.t), flux(other.flux){}

  photon& operator = (const photon& other){
    p = other.p; d = other.d; t = other.t; 
    flux = other.flux;
    return *this;
  }
  photon& operator += (const photon& other){
    flux += other.flux;
    return *this;
  }

};