#include <tuple>
#include <unordered_map>
#include "photon.h"
#include "../vecmath/vec.h"

//Will have to do this since the map takes a type not an object
const float r = 1.0; // 

template <typename T>
class mHash{
  float radius;

  public:
    mHash(): radius(r){}

    // Update this part to support whatever type we use for Point
    std::tuple<int, int, int> operator() ( T p ){
      return std::make_tuple( int(p.x/radius), int(p.y/radius), int(p.z/radius))
    }

};


// spatialHash = unordered_map<Point, Photon, mHash>(); //Create the Spatial Hash
