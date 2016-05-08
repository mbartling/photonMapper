#include <cmath>
#include <iostream>

#include "light.h"
#include "../ui/TraceUI.h"
extern TraceUI* traceUI;

using namespace std;
#define PI 3.14159265359

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

// Random Sample the plane that contains the image of the scene bounds
std::tuple<Vec3d,Vec3d> DirectionalLight::firePhoton(const BoundingBox* bBox = nullptr) const {

  std::tuple<Vec3d, Vec3d> returnTuple; 

  if(bBox == nullptr)
  {
    returnTuple = std::make_tuple(Vec3d(0,0,0), Vec3d(0,0,0));
    cerr << "Error in DirectionalLight!" << endl;
  }
  else
  {
    Vec3d photonStartPt;
    double boxLengthDiv2;

    int mAxis = (abs(orientation[0]) > abs(orientation[1])) ? 0 : 1;
    mAxis = (abs(orientation[mAxis]) > abs(orientation[2])) ? mAxis : 2;
    //double maxMag = abs(orientation[mAxis]);

    boxLengthDiv2 = (bBox->getMax()[mAxis] - bBox->getMin()[mAxis]) / 2.0; 

    if(orientation[mAxis] > 0)
      photonStartPt[mAxis] = bBox->getMin()[mAxis] - boxLengthDiv2;
    else
      photonStartPt[mAxis] = bBox->getMax()[mAxis] + boxLengthDiv2;

    double distU = bBox->getMax()[(mAxis + 1) % 3] - bBox->getMin()[(mAxis + 1) % 3];
    double distV = bBox->getMax()[(mAxis + 2) % 3] - bBox->getMin()[(mAxis + 2) % 3];

    // double randU = ((double)rand() / (double) RAND_MAX) * distU * 2 + (bBox->getMin()[(mAxis + 1) % 3] - distU/2.0);
    // double randV = ((double)rand() / (double) RAND_MAX) * distV * 2 + (bBox->getMin()[(mAxis + 2) % 3] - distV/2.0);

    double randU = ((double)rand() / (double) RAND_MAX) * distU + (bBox->getMin()[(mAxis + 1) % 3]);
    double randV = ((double)rand() / (double) RAND_MAX) * distV + (bBox->getMin()[(mAxis + 2) % 3]);

    photonStartPt[(mAxis + 1) % 3] = randU;
    photonStartPt[(mAxis + 2) % 3] = randV;

    returnTuple = std::make_tuple(photonStartPt, orientation);
  }

  return returnTuple;
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

std::tuple<Vec3d,Vec3d> PointLight::firePhoton(const BoundingBox* bBox = nullptr) const {
  //double x = ((double)rand() - RAND_MAX/2)/(double)RAND_MAX;
  //double y = ((double)rand() - RAND_MAX/2)/(double)RAND_MAX;
  //double z = ((double)rand() - RAND_MAX/2)/(double)RAND_MAX;

  std::tuple<Vec3d, Vec3d> returnTuple; 

  if(bBox == nullptr)
  {
    //cout << "bEmpty is TRUE" << endl;

    // in this case, we random sample the sphere 

    double theta = ((double)rand() / (double) RAND_MAX)*2.0*PI;
    // double phi = ((double)rand() / (double) RAND_MAX)*PI;
    double phi = acos(2.0*((double)rand() / (double) RAND_MAX)-1.0);
    double x = cos(theta)*sin(phi);
    double y = sin(theta)*sin(phi);
    double z = cos(phi);
    Vec3d sample(x, y, z);
    sample.normalize();
    returnTuple = std::make_tuple(position, -getDirection(sample));
  }
  else
  {
    //cout << "bEmpty is FALSE" << endl;

    std::vector<Vec3d> bBoxVerts;

    bBoxVerts.push_back(Vec3d(bBox->getMin()[0], bBox->getMin()[1], bBox->getMin()[2]) - position);
    bBoxVerts.push_back(Vec3d(bBox->getMax()[0], bBox->getMin()[1], bBox->getMin()[2]) - position);
    bBoxVerts.push_back(Vec3d(bBox->getMax()[0], bBox->getMax()[1], bBox->getMin()[2]) - position);
    bBoxVerts.push_back(Vec3d(bBox->getMin()[0], bBox->getMax()[1], bBox->getMin()[2]) - position);

    bBoxVerts.push_back(Vec3d(bBox->getMin()[0], bBox->getMin()[1], bBox->getMax()[2]) - position);
    bBoxVerts.push_back(Vec3d(bBox->getMax()[0], bBox->getMin()[1], bBox->getMax()[2]) - position);
    bBoxVerts.push_back(Vec3d(bBox->getMax()[0], bBox->getMax()[1], bBox->getMax()[2]) - position);
    bBoxVerts.push_back(Vec3d(bBox->getMin()[0], bBox->getMax()[1], bBox->getMax()[2]) - position);


    double minTheta = 10000;
    double maxTheta = -10000;
    double minPhi = 10000;
    double maxPhi = -10000;

    for(auto& vert : bBoxVerts)
    {
      vert.normalize();
      
      // now we figure out the theta and the phi for spherical coordinates
      double thetaTemp = atan2( ((double)vert[1]), ((double)vert[0]) ); 
      double phiTemp = acos( ((double)vert[2]) );

      // thetaTemp is in the range -pi to pi
      // we convert it to the range 0 to 2pi

      if(thetaTemp < 0)
        thetaTemp = thetaTemp + 2*PI;

      if(thetaTemp < minTheta) minTheta = thetaTemp;
      if(thetaTemp > maxTheta) maxTheta = thetaTemp;
      if(phiTemp < minPhi) minPhi = phiTemp;
      if(phiTemp > maxPhi) maxPhi = phiTemp;
    }

    double theta = ((double)rand() / (double) RAND_MAX)*2.0*PI;
    double phi = acos(2.0*((double)rand() / (double) RAND_MAX)-1.0);

    theta = theta * (1.0/(2.0 * PI)) * (maxTheta - minTheta) + minTheta;
    phi = phi * (1.0/PI) * (maxPhi - minPhi) + minPhi;

    double x = cos(theta)*sin(phi);
    double y = sin(theta)*sin(phi);
    double z = cos(phi);

    Vec3d sample(x, y, z);
    sample.normalize();
    returnTuple = std::make_tuple(position, -getDirection(sample));
  }

  return returnTuple;
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
        //return m.kd(i) % m.kt(i);
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
