// #pragma once
#ifndef __KDTREE2_H__
#define __KDTREE2_H__

#include "ray.h"
#include "bbox.h"
#include <vector>
#include <list>
#include <algorithm>
#include <limits>


template <typename Obj> class KDNode;


template <typename Obj>
class KdTree{
public:
  
  // KdTree(void):root(nullptr){}

  void buildTree(const std::vector<Obj>& v, int depth, const BoundingBox& bounds);
  void delete_tree(KDNode<Obj>* node);
  void delete_tree(){delete_tree(root);}

  bool intersect(ray& r, isect& i) const;
  bool intersectLocal(ray& r, isect& i) const;
  void verify_objectsR(KDNode<Obj>* node, std::list<Obj>& mList);
  bool verify_objects(void);

  ~KdTree(void){delete_tree(root);}
private:
  KDNode<Obj>* root;
  std::list<Obj> data;
  int _depth;
  // std::vector<Obj> data;
 
};

class equivalentPlanes{
public:
  bool operator()(double x, double y){ return fabs(x-y) <= RAY_EPSILON; }
};

template <typename Obj> 
class KDNode{
  public:
    BoundingBox _bounds;

    KDNode* _left;
    KDNode* _right;
    double _t;
    // KDNode(void):_left(nullptr), _right(nullptr){}
    bool intersect(ray& r, isect& i) const;
    bool intersectLocal(ray& r, isect& i) const;
    bool isLeaf(void) const {return (_left == nullptr && _right == nullptr);}
    bool hasGeometry(void) const {return _geometry_p.size() > 0;}
    // Axis is chosen as axis = depth % 3;
    // KDNode<Obj>* buildTree(const std::list<Obj>& objs, int depth ) const;
    std::list<Obj> _geometry_p;

  private: 

};


const int KDMinObjCount = 10;

template<typename Obj>
struct compareBB{
  int ax;
  compareBB(int axis): ax(axis){}
  bool operator()(Obj i, Obj j) { 
    Vec3d bi = i->getBoundingBox().getMin();
    Vec3d bj = j->getBoundingBox().getMin();
    return (bi[ax]<bj[ax]); 
  }
};
template<typename Obj>
struct compareBBU{
  int ax;
  double t;
  compareBBU(int axis, double _t): ax(axis), t(_t){}
  bool operator()(Obj i) { 
    Vec3d bi = i->getBoundingBox().getMin();
    return (bi[ax]<t + RAY_EPSILON); 
  }
};

template <typename Obj>
double getOptimalSplittingPlane(KDNode<Obj>* node, int depth, const std::list<Obj>& geomList)
{
  int axis = depth % 3;
  std::list<double> SplittingPlanes;
  for(auto& geom: geomList){
    SplittingPlanes.push_back(geom->getBoundingBox().getMin()[axis]);
    SplittingPlanes.push_back(geom->getBoundingBox().getMax()[axis]);
  }
  SplittingPlanes.sort();
  SplittingPlanes.unique(equivalentPlanes());

  BoundingBox leftBox;
  double bestT = -1;
  double bestCost = std::numeric_limits<double>::max();
  double boundingAreaInv = 1/node->_bounds.area();
  int    leftCount = 0;
  int    rightCount = 0;
  double lArea = 0.0;
  double rArea = 0.0;
  BoundingBox lb = node->_bounds;
  BoundingBox rb = node->_bounds;

  double areaTempL;
  double areaTempR;

  // auto it = node->_geometry_p.begin();
  for(auto& t: SplittingPlanes){
    leftCount = 0;
    rightCount = 0;
    lArea = 0.0;
    rArea = 0.0;
    // BoundingBox lb = node->_bounds;
    // BoundingBox rb = node->_bounds;
    lb.setMax(axis, t);
    rb.setMin(axis, t);
    lArea = lb.area();
    rArea = rb.area();

    //NOTE Check this for safety, there might be FP error
    // in calculating before and after
    for(auto& geom: geomList){
      BoundingBox bb = geom->getBoundingBox();
      if(bb.getMin()[axis] < t){
        leftCount++;
        // lArea += areaTempL;
        // lb.merge()
      }
      if(bb.getMax()[axis] > t){
          rightCount++;
          // rArea += areaTempR;
        }
    } //Now have the Areas, and counts

    double splitCost = 0.3 + (lArea*leftCount + rArea*rightCount)*boundingAreaInv;
    if(splitCost < bestCost){
      bestCost = splitCost;
      bestT = t;
    }
  }// end Outter For loop
  node->_t = bestT;
  return bestT;
}


// template <typename Obj>
// KDNode<Obj>* KDNode<Obj>::buildTree(const std::list<Obj>& objs, int depth) const {
template <typename Obj>
KDNode<Obj>* KdTreeFactory(std::list<Obj>& objs, int depth, const BoundingBox& _bounds) {
  
  int axis = depth % 3;
  KDNode<Obj>* node = new KDNode<Obj>();
  // node->_geometry_p = objs;
  node->_left = nullptr;
  node->_right = nullptr;
  node->_bounds = _bounds;


  if (objs.size() == 0){ //Dead Leaf
    return node;
  } 
  //Now make sure the list is sorted across whatever axis we are dealing with
  objs.sort(compareBB<Obj>(axis));

  //Make Leaf
  if(objs.size() < KDMinObjCount){
    node->_geometry_p = objs;
    return node;

  } //If we have a non empty set and the depth is zero
  //return 
  if(depth == 0){
    node->_geometry_p = objs;
    return node;    
  }

  double t = getOptimalSplittingPlane(node, depth, objs);
  // double t = (_bounds.getMax()[axis] - _bounds.getMin()[axis])/2.0; //Dumb median split
  // std::list<Obj> leftList;
  // std::list<Obj> rightList;

  BoundingBox LeftBox = node->_bounds;
  BoundingBox RightBox = node->_bounds;
  LeftBox.setMax(axis, t);
  RightBox.setMin(axis, t);

  // for(auto& object: objs){
  auto objsBegin = objs.begin();
  auto objsEnd = objs.end();
  auto leftBegin = objsBegin;
  auto leftEnd = objsBegin;
  auto rightBegin = objsEnd;
  auto rightEnd = objsEnd;
  std::list<Obj> leftList;
  std::list<Obj> rightList;

  //Remember, objects are sorted by their minimum side
  rightBegin = std::partition(objs.begin(), objs.end(), compareBBU<Obj>(axis,t));
 // for(auto& object: objs){
  // for(auto object = objsBegin; object != objsEnd; object++){
  //   auto byteme = object;
  //   bool lInt, rInt;
  //   int mCase = 0;
  //   if(lInt = LeftBox.intersects((*object)->getBoundingBox())) mCase += 1;//leftList.push_back(object);
  //   if(rInt = RightBox.intersects((*object)->getBoundingBox())) mCase += 2;//rightList.push_back(object);
  //   // if(!lInt && !rInt){
  //   //   leftList.push_back(object);
  //   //   rightList.push_back(object);
  //   // }
  //   switch(mCase){
  //     case 1: //Left only
  //       leftList.splice(leftList.begin(), objs, object, byteme++);
  //       break;
  //     case 2: //Right Only
  //       rightList.splice(leftList.begin(), objs, object, byteme++);
  //       break;
  //     default:
  //       rightList.push_back(*object);
  //       leftList.splice(leftList.begin(), objs, object, byteme++);
  //       break;
  //   }
  // }  
  leftList.splice(leftList.end(), objs, leftBegin, rightBegin);
  rightList.splice(rightList.end(), objs, rightBegin, rightEnd);
  node->_left = KdTreeFactory(leftList, depth - 1, LeftBox);
  node->_right = KdTreeFactory(rightList, depth - 1, RightBox);

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
bool KdTree<Obj>::verify_objects(void){
  std::list<Obj> mThings;
  verify_objectsR(root, mThings);
  mThings.sort();
  mThings.unique();
  data.sort();
  std::cout << mThings.size();
  return mThings.size() == data.size();
}

template <typename Obj>
void KdTree<Obj>::verify_objectsR(KDNode<Obj>* node, std::list<Obj>& mList){
  if(node != nullptr){
    if(node->isLeaf()){
      for(auto& damnThing: node->_geometry_p){
        mList.push_back(damnThing);
      }
      return;
    }
    verify_objectsR(node->_left, mList);
    verify_objectsR(node->_right, mList);
  }
  return;
}

template <typename Obj>
void KdTree<Obj>::buildTree(const std::vector<Obj>& v, int depth,const BoundingBox& bounds){
  // std::list<Obj> tmp(v.begin(), v.end());
  // data = tmp;
  std::copy(v.begin(), v.end(), std::back_inserter(data));
  _depth = depth;
  root = KdTreeFactory(data, depth, bounds);
}

template <typename Obj>
bool Intersect(KDNode<Obj>* node, ray& r, isect& i, int depth){
  double tMin;
  double tMax;
  Vec3d R0 = r.getPosition();
  Vec3d Rd = r.getDirection();
  bool have_one = false;
  int axis = depth % 3;
  if(node != nullptr){
    // If it is a leaf and Not empty
    if(node->isLeaf()){
        if(node->hasGeometry()) return node->intersect(r,i);
        else return false;  
    }

    if(node->_bounds.intersect(r, tMin, tMax)){

      //Get who is near and who is far

      // double vd = Rd[axis];
      // double v = node->_t - R0[axis];
      // double ts = v/vd;
      double s = node->_t;
      double a = r.at(tMin)[axis];
      double b = r.at(tMax)[axis];

      if(a <= s){
        if(b < s){
          have_one = Intersect(node->_left, r, i, depth-1);
        } 
        else{
          have_one = Intersect(node->_left, r, i, depth - 1);
          isect cur;
          if(Intersect(node->_right, r, cur, depth - 1)){
            if(!have_one || (cur.t < i.t)){
            i = cur;
            have_one = true;
            }
          }
        }
      }
      else{
        if(b > s){
          have_one = Intersect(node->_right, r, i, depth-1);
        } else{
          have_one = Intersect(node->_right, r, i, depth - 1);
          isect cur;
          if(Intersect(node->_left, r, cur, depth - 1)){
            if(!have_one || (cur.t < i.t)){
            i = cur;
            have_one = true;
            }
          }

        }
      }
    } // If ray intersects node
  } //End if
  return have_one;
}



template <typename Obj>
bool IntersectLocal(KDNode<Obj>* node, ray& r, isect& i, int depth){
  double tMin;
  double tMax;
  Vec3d R0 = r.getPosition();
  Vec3d Rd = r.getDirection();
  bool have_one = false;
  int axis = depth % 3;
  if(node != nullptr){
    // If it is a leaf and Not empty
    if(node->_bounds.intersect(r, tMin, tMax)){
      if(node->isLeaf()){
        if(node->hasGeometry()) return node->intersectLocal(r,i);
        else return false;  
      }

      //Get who is near and who is far

      double vd = Rd[axis];
      double v = node->_t - R0[axis];
      double ts = v/vd;
      double s = node->_t;
      double a = r.at(tMin)[axis];
      double b = r.at(tMax)[axis];

      if(a <= s){
        if(b < s){
          have_one = IntersectLocal(node->_left, r, i, depth-1);
        } 
        else{
          have_one = IntersectLocal(node->_left, r, i, depth - 1);
          isect cur;
          if(IntersectLocal(node->_right, r, cur, depth - 1)){
            if(!have_one || (cur.t < i.t)){
            i = cur;
            have_one = true;
            }
          }
        }
      }
      else{
        if(b > s){
          have_one = IntersectLocal(node->_right, r, i, depth-1);
        } else{
          have_one = IntersectLocal(node->_right, r, i, depth - 1);
          isect cur;
          if(IntersectLocal(node->_left, r, cur, depth - 1)){
            if(!have_one || (cur.t < i.t)){
            i = cur;
            have_one = true;
            }
          }

        }
      }
    } // If ray intersects node
  } //End if
  return have_one;

}


template<typename Obj>
bool KDNode<Obj>::intersect(ray& r, isect& i) const {
  double tmin = 0.0;
  double tmax = 0.0;
  bool have_one = false;
  // typedef std::list<Obj>::const_iterator iter;
  using iter = typename std::list<Obj>::const_iterator;
  for(iter j = _geometry_p.begin(); j != _geometry_p.end(); ++j) {
    isect cur;
    if( (*j)->intersect(r, cur) ) {
      if(!have_one || (cur.t < i.t)) {
        i = cur;
        have_one = true;
      }
    }
  }
  if(!have_one) i.setT(1000.0);
  return have_one;
}

template<typename Obj>
bool KDNode<Obj>::intersectLocal(ray& r, isect& i) const {
  double tmin = 0.0;
  double tmax = 0.0;
  bool have_one = false;
  // typedef std::list<Obj>::const_iterator iter;
  using iter = typename std::list<Obj>::const_iterator;
  for(iter j = _geometry_p.begin(); j != _geometry_p.end(); ++j) {
    isect cur;
    if( (*j)->intersectLocal(r, cur) ) {
      if(!have_one || (cur.t < i.t)) {
        i = cur;
        have_one = true;
      }
    }
  }
  if(!have_one) i.setT(1000.0);
  return have_one;
}


template<typename Obj>
bool KdTree<Obj>::intersect(ray& r, isect& i) const {
  int depth = _depth;
  return Intersect(root, r, i, depth);
}
template<typename Obj>
bool KdTree<Obj>::intersectLocal(ray& r, isect& i) const {
  int depth = _depth;
  return IntersectLocal(root, r, i, depth);
}

#endif