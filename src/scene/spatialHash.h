#include <tuple>
#include <unordered_map>
#include "photon.h"
#include "../vecmath/vec.h"

template <typename T>
class mHash{
  float radius;

  public:
    mHash(float r): radius(r){}

    // Update this part to support whatever type we use for Point
    std::tuple<int, int, int> operator() ( T p ){
      return std::make_tuple( int(p.x/radius), int(p.y/radius), int(p.z/radius))
    }

};


// spatialHash = unordered_map<Point, Photon, mHash(radius)>(); //Create the Spatial Hash
