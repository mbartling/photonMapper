#ifndef ___SPATIALHASH_H__
#define ___SPATIALHASH_H__

#include <tuple>
#include <unordered_map>
#include "photon.h"
#include "../vecmath/vec.h"
// #include <functional>
#include <boost/functional/hash.hpp>
//Will have to do this since the map takes a type not an object
const double r = 1.0; // 

template <typename T>
class mHash{
  const double radius = 0.005;

  public:
    // mHash(): radius(r){}

    // Update this part to support whatever type we use for Point
    std::tuple<int, int, int> operator() ( T p ){
      return std::make_tuple( int(p.x()/radius), int(p.y()/radius), int(p.z()/radius));
    }

};

template <typename myhashT> 
class myMap{
  std::unordered_map<std::tuple<int, int, int>, photon, boost::hash<std::tuple<int, int, int>>> _map;
  myhashT _hash;
  public:

    photon operator[] (const Vec3d& point) const{
      // if (_map.count(point))
        return _map[_hash(point)];
      // else return photon();
    }
    photon& operator[] (const Vec3d& point){
      return _map[_hash(point)];
    }
    int count(const Vec3d& point ){return _map.count(_hash(point));}
    int size( void ){return _map.size();}

};

// spatialHash = unordered_map<Point, Photon, mHash>(); //Create the Spatial Hash
// using SpatialHash = std::unordered_map<Vec3d, photon, std::hash<mHash<Vec3d>>>;
using SpatialHash = myMap<mHash<Vec3d>>;

#endif /*___SPATIALHASH_H__*/