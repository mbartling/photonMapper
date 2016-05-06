#include <cmath>

#include "scene.h"
#include "light.h"
#include "../ui/TraceUI.h"

using namespace std;

const int kDDepth = 15;
bool USE_KD_TREE = true;

bool Geometry::intersect(ray& r, isect& i) const {
	double tmin, tmax;
	if (hasBoundingBoxCapability() && !(bounds.intersect(r, tmin, tmax))) return false;
	// Transform the ray into the object's local coordinate space
	Vec3d pos = transform->globalToLocalCoords(r.p);
	Vec3d dir = transform->globalToLocalCoords(r.p + r.d) - pos;
	double length = dir.length();
	dir /= length;
	Vec3d Wpos = r.p;
	Vec3d Wdir = r.d;
	r.p = pos;
	r.d = dir;
	bool rtrn = false;
	if (intersectLocal(r, i))
	{
		// Transform the intersection point & normal returned back into global space.
		i.N = transform->localToGlobalCoordsNormal(i.N);
		i.t /= length;
		rtrn = true;
	}
	r.p = Wpos;
	r.d = Wdir;
	return rtrn;
}

bool Geometry::hasBoundingBoxCapability() const {
	// by default, primitives do not have to specify a bounding box.
	// If this method returns true for a primitive, then either the ComputeBoundingBox() or
    // the ComputeLocalBoundingBox() method must be implemented.

	// If no bounding box capability is supported for an object, that object will
	// be checked against every single ray drawn.  This should be avoided whenever possible,
	// but this possibility exists so that new primitives will not have to have bounding
	// boxes implemented for them.
	return false;
}

Scene::~Scene() {
    giter g;
    liter l;
    tmap::iterator t;
    // kdtree.delete_tree();
    for( g = objects.begin(); g != objects.end(); ++g ) delete (*g);
    for( g = objectsBB.begin(); g != objectsBB.end(); ++g ) delete (*g);
    for( l = lights.begin(); l != lights.end(); ++l ) delete (*l);
    for( t = textureCache.begin(); t != textureCache.end(); t++ ) delete (*t).second;
}

void Scene::buildKdTree(){
	kdtree.buildTree(objects, kDDepth, sceneBounds);
	// std::cout << "Is KD Tree Valid?: "<<kdtree.verify_objects() << std::endl;
}
// Get any intersection with an object.  Return information about the 
// intersection through the reference parameter.
bool Scene::intersect(ray& r, isect& i) const {
	double tmin = 0.0;
	double tmax = 0.0;
	bool have_one = false;

	//NOTE IF USE THIS THEN MAKE SURE TO UPDATE THE TRIMESH CLASS TO ADD/NOT ADD triangles to scene
	// if (USE_KD_TREE) 
		have_one = kdtree.intersect(r, i);
	// else{
	// 	typedef vector<Geometry*>::const_iterator iter;
	// 	for(iter j = objects.begin(); j != objects.end(); ++j) {
	// 		isect cur;
	// 		if( (*j)->intersect(r, cur) ) {
	// 			if(!have_one || (cur.t < i.t)) {
	// 				i = cur;
	// 				have_one = true;
	// 			}
	// 		}
	// 	}
	// } // END ELSE
	if (TraceUI::m_debug) intersectCache.push_back(std::make_pair(new ray(r), new isect(i)));	
	if(!have_one) i.setT(1000.0);
	// if debugging,
	return have_one;
}

TextureMap* Scene::getTexture(string name) {
	tmap::const_iterator itr = textureCache.find(name);
	if(itr == textureCache.end()) {
		textureCache[name] = new TextureMap(name);
		return textureCache[name];
	} else return (*itr).second;
}


