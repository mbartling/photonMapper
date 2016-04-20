#ifndef __CUBEMAP_H__
#define __CUBEMAP_H__

#include "ray.h"
#include <string>
#include <cmath>
#include "material.h"

class CubeMap{
public:
  enum Positions {TOP, LEFT, FRONT, RIGHT, BACK, DOWN, NUMPOSITIONS};

  CubeMap(void){}
  ~CubeMap(void){
    for(int i = 0; i < NUMPOSITIONS; i++){
      if(sides[i]) delete sides[i];
    }
  }

  void setTop(std::string fPath){sides[TOP] = new TextureMap(fPath);}
  void setLeft(std::string fPath){sides[LEFT] = new TextureMap(fPath);}
  void setFront(std::string fPath){sides[FRONT] = new TextureMap(fPath);}
  void setRight(std::string fPath){sides[RIGHT] = new TextureMap(fPath);}
  void setBack(std::string fPath){sides[BACK] = new TextureMap(fPath);}
  void setDown(std::string fPath){sides[DOWN] = new TextureMap(fPath);}

  Vec3d intersect(const ray& r){
    Vec3d Rd = r.getDirection();
    int primaryDirection = 0;
    
    primaryDirection = (fabs(Rd[0]) > fabs(Rd[1])) ? 0 : 1;
    primaryDirection = (fabs(Rd[primaryDirection]) > fabs(Rd[2])) ? primaryDirection : 2;

    Vec2d uv;
    int mFace = 0;
    // if (primaryDirection == 0 && Rd[primaryDirection] > 0.0)        {mFace = RIGHT; uv[0] = Rd[1]; uv[1] = Rd[2]; }
    // else if (primaryDirection == 0 && Rd[primaryDirection] <= 0.0)  {mFace = LEFT;  uv[0] = Rd[2]; uv[1] = Rd[1]; }
    // else if (primaryDirection == 1 && Rd[primaryDirection] > 0.0)   {mFace = TOP;   uv[0] = Rd[2]; uv[1] = Rd[0]; }
    // else if (primaryDirection == 1 && Rd[primaryDirection] <= 0.0)  {mFace = DOWN;  uv[0] = Rd[0]; uv[1] = Rd[2]; }
    // else if (primaryDirection == 2 && Rd[primaryDirection] > 0.0)   {mFace = FRONT; uv[0] = Rd[0]; uv[1] = Rd[1]; }
    // else {mFace = BACK; uv[0] = Rd[1]; uv[1] = Rd[0];}

    // // uv/=max(fabs(uv[0]), fabs(uv[1]));
    // uv.normalize();
    // uv *= 0.5;
    // uv += 0.5;

    if (primaryDirection == 0 && Rd[primaryDirection] > 0.0)        {mFace = RIGHT; uv[0] = 1.0 - (Rd[2]/Rd[0]+1.0)*0.5; uv[1] = (Rd[1]/Rd[0] + 1.0)*0.5; }
    else if (primaryDirection == 0 && Rd[primaryDirection] <= 0.0)  {mFace = LEFT;  uv[0] = 1.0 - (Rd[2]/Rd[0]+1.0)*0.5; uv[1] = 1.0 - (Rd[1]/Rd[0] + 1.0)*0.5; }
    else if (primaryDirection == 1 && Rd[primaryDirection] > 0.0)   {mFace = TOP;   uv[0] = (Rd[0]/Rd[1]+1.0)*0.5; uv[1] = 1.0 - (Rd[2]/Rd[1] + 1.0)*0.5; }
    else if (primaryDirection == 1 && Rd[primaryDirection] <= 0.0)  {mFace = DOWN;  uv[0] = 1.0 - (Rd[0]/Rd[1]+1.0)*0.5; uv[1] = (Rd[2]/Rd[1] + 1.0)*0.5;}
    else if (primaryDirection == 2 && Rd[primaryDirection] > 0.0)   {mFace = FRONT; uv[0] = (Rd[0]/Rd[2]+1.0)*0.5; uv[1] = (Rd[1]/Rd[2] + 1.0)*0.5; }
    else {mFace = BACK; uv[0] = (Rd[0]/Rd[2]+1.0)*0.5; uv[1] = 1.0 - (Rd[1]/Rd[2] + 1.0)*0.5;}

    // uv/=max(fabs(uv[0]), fabs(uv[1]));
    // uv.normalize();
    // uv *= 0.5;
    // uv += 0.5;
    if(uv[0] < 0) uv[0] = 0.0;
    if(uv[0] > 1.0) uv[0] = 1.0;
    if(uv[1] < 0) uv[1] = 0.0;
    if(uv[1] > 1.0) uv[1] = 1.0;

    if(sides[mFace]){
      return sides[mFace]->getMappedValue(uv);
    }else
      return Vec3d(0,0,0); //If Fail for any reason
  }
private:
  // BoundingBox bounds;
  // std::vector<TextureMap*> sides;
  TextureMap* sides[NUMPOSITIONS];
};

#endif /*__CUBEMAP_H__*/