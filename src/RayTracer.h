#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

// The main ray tracer.

#include "scene/ray.h"
#include <time.h>
#include <queue>
#include "ui/pixelblock.h"
#include "scene/cubeMap.h"
#include "scene/photon.h"
#include <tuple>
class Scene;

class RayTracer
{
public:
	RayTracer();
        ~RayTracer();

	Vec3d tracePixel(int i, int j);
  Vec3d tracePixelBlock(const pixelBlock pxB);
	std::pair<Vec3d,Vec3d> trace(double x, double y);
	std::pair<Vec3d,Vec3d> traceRay(ray& r, int depth);
  void tracePhoton(photon& r, int depth);
  void firePhotons(int numPhotons, Vec3d mFlux, int numThreads);
	void getBuffer(unsigned char *&buf, unsigned char *&bufP, int &w, int &h);
	double aspectRatio();

	void traceSetup( int w, int h );

	bool loadScene(char* fn);
	bool sceneLoaded() const { return scene != 0; }

	void setReady(bool ready) { m_bBufferReady = ready; }
	bool isReady() const { return m_bBufferReady; }

  void setDistributionRayTracing(bool yn){DISTRIBUTION_TRACING = yn;}
  bool isDistributionRayTracing(void){return DISTRIBUTION_TRACING;}

  bool haveCubeMap(void)const {return environment != nullptr;}
	void createCubeMap(void){if(!haveCubeMap()) environment = new CubeMap();}

  // void addCubeFace(int faceNum, std::string fileName);
  const Scene& getScene() { return *scene; }
    // int getSuperSampling(void) const {return SuperSampleX;}
  // void setSuperSampling(int x){SuperSampleX = x;}

public:
  // int SuperSampleX;

  unsigned char *buffer;
  unsigned char *bufferP;
  int buffer_width, buffer_height;
  int bufferSize;
  Scene* scene;
  bool DISTRIBUTION_TRACING;
  bool m_bBufferReady;
  CubeMap* environment;
};    

#endif // __RAYTRACER_H__
