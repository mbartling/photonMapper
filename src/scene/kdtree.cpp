#include "kdtree.h"
// #include "scene.h"
#include <algorithm>
#include <list>
#include <limits>
// std::vector<Geometry*> xSortedGeom;
// std::vector<Geometry*> ySortedGeom;
// std::vector<Geometry*> zSortedGeom;

std::vector<double> costX;
std::vector<double> costY;
std::vector<double> costZ;

const int KDMinObjCount = 10;

template<typename Obj>
struct compareBB{
  int ax;
  compareBB(int axis): ax(axis){}
  bool operator()(Obj* i, Obj* j) { 
    Vec3d bi = i->getBoundingBox().getMin();
    Vec3d bj = j->getBoundingBox().getMin();
    return (bi[ax]<bj[ax]); 
  }
};

template <typename Obj>
double getOptimalSplittingPlane(KDNode<Obj>* node, int depth)
{
  int axis = depth % 3;
  std::list<double> SplittingPlanes;
  for(auto& geom: node->_geometry_p){
    SplittingPlanes.push_back(geom->getBoundingBox().getMin()[axis]);
    SplittingPlanes.push_back(geom->getBoundingBox().getMax()[axis]);
  }
  SplittingPlanes.sort();
  SplittingPlanes.unique(equivalentPlanes());

  BoundingBox leftBox;
  double bestT = -1;
  double bestCost = std::numeric_limits<double>::max();
  double boundingAreaInv = 1/node->_bounds.area();

  // auto it = node->_geometry_p.begin();
  for(auto& t: SplittingPlanes){
    int leftCount = 0;
    int rightCount = 0;
    double lArea;
    double rArea;

    //NOTE Check this for safety, there might be FP error
    // in calculating before and after
    for(auto& geom: node->_geometry_p){
      double areaTemp = geom->getBoundingBox()->area();
      if(geom->getBoundingBox().getMin()[axis] < t){
        leftCount++;
        lArea += areaTemp;
        if(geom->getBoundingBox().getMax()[axis] > t){
          rightCount++;
          rArea += areaTemp;
        }
      } // Object is entirely in the far side 
      else{
        rightCount++;
        rArea += areaTemp;
      }
    } //Now have the Areas, and counts

    double splitCost = 0.3 + (lArea*leftCount + rArea*rightCount)*boundingAreaInv;
    if(splitCost < bestCost){
      bestCost = splitCost;
      bestT = t;
    }
  }// end Outter For loop

  return bestT;
}
template <typename Obj>
KDNode<Obj>* KDNode<Obj>::buildTree(const std::list<Obj>& objs, int depth) const {
  
  int axis = depth % 3;
  KDNode<Obj>* node = new KDNode<Obj>();
  node->_geometry_p = objs;
  node->_left = nullptr;
  node->_right = nullptr;
  node->_bounds = BoundingBox();


  if (objs.size() == 0){ //Dead Leaf
    return node;
  } 
  //Now make sure the list is sorted across whatever axis we are dealing with
  node->_geometry_p.sort(compareBB<Obj>(axis));
  //Generate The Nodes bounding Volume (AABB)
  BoundingBox bx;
  for(auto& object: objs){
    bx.merge(object);
  }
  node->_bounds = bx;

  //Make Leaf
  if(objs.size() < KDMinObjCount){
    return node;

  } //If we have a non empty set and the depth is zero
  //return 
  if(depth == 0){
    return node;    
  }

  double t = getOptimalSplittingPlane(node, depth);
  std::list<Obj> leftList;
  std::list<Obj> rightList;

  BoundingBox LeftBox = node->_bounds;
  BoundingBox RightBox = node->_bounds;
  LeftBox.setMax(axis, t);
  RightBox.setMin(axis, t);

  for(auto& object: objs){
    if(LeftBox.intersects(object->getBoundingBox())) leftList.push_back(object);
    if(RightBox.intersects(object->getBoundingBox())) rightList.push_back(object);
  }

  node->_left = buildTree(leftList, depth - 1);
  node->_right = buildTree(rightList, depth - 1);

  return node;
}

template <typename Obj>
void KdTree<Obj>::delete_tree(KDNode<Obj>* node){
  if(node != nullptr){
    // if(node->_left != )
    delete_tree(node->_left);
    delete_tree(node->_right);
    delete(node);
  }
}

template <typename Obj>
void KdTree<Obj>::buildTree(const std::vector<Obj>& v, int depth){
  std::list<Obj> tmp(v.begin(), v.end());
  data = tmp;
  root = buildTree(data, depth);
}