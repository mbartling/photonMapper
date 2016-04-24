#ifndef __PHOTON_H__
#define __PHOTON_H__

#include "ray.h"
#include "../vecmath/vec.h"

class photon : public ray{
public:

  Vec3d flux;
  photon():ray(Vec3d(0.0,0.0,0.0), Vec3d(0.0,0.0,0.0), VISIBILITY), flux(Vec3d(0.0,0.0,0.0)) {}
  photon(const Vec3d &pp, const Vec3d &dd, const Vec3d& ff, RayType tt = VISIBILITY)
    : ray(pp, dd, tt), flux(ff) {}
  photon(const photon& other): ray(other.p, other.d, other.t), flux(other.flux){}

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

#endif
