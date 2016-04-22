//
// ray.h
//
// The low-level classes used by ray tracing: the ray and isect classes.
//

#ifndef __RAY_H__
#define __RAY_H__

// who the hell cares if my identifiers are longer than 255 characters:
#pragma warning(disable : 4786)

#include "../vecmath/vec.h"
#include "../vecmath/mat.h"
#include "material.h"
#include "../ui/TraceUI.h"

class SceneObject;

// A ray has a position where the ray starts, and a direction (which should
// always be normalized!)

class ray {
public:
	enum RayType
	{
		VISIBILITY,
		REFLECTION,
		REFRACTION,
		SHADOW
	};

        ray(const Vec3d &pp, const Vec3d &dd, RayType tt = VISIBILITY)
	  : p(pp), d(dd), t(tt) {}
        ray(const ray& other) : p(other.p), d(other.d), t(other.t) {}
	~ray() {}
    // virtual ~ray(){}
	ray& operator =( const ray& other ) 
	{ p = other.p; d = other.d; t = other.t; return *this; }

	Vec3d at( double t ) const
	{ return p + (t*d); }

	Vec3d getPosition() const { return p; }
	Vec3d getDirection() const { return d; }
	RayType type() const { return t; }

public:
	Vec3d p;
	Vec3d d;
	RayType t;
};

// The description of an intersection point.

class isect
{
public:
    isect() : obj( NULL ), t( 0.0 ), N(), material(0) {}
	isect(const isect& other)
	{
		obj = other.obj;
		t = other.t;
		N = other.N;
		bary = other.bary;
		uvCoordinates = other.uvCoordinates;
		if (other.material) material = new Material(*other.material);
		else material = 0;
	}

	~isect() { delete material; }
    
    isect& operator = (const isect& other) {
        if( this != &other ) {
            obj = other.obj;
            t = other.t;
            N = other.N;
			bary = other.bary;
            uvCoordinates = other.uvCoordinates;
			if( other.material ) {
                if( material ) *material = *other.material;
                else material = new Material(*other.material );
            }
            else {
                delete material;
                material = 0;
            }
        }
        return *this;
    }

    void setObject(const SceneObject *o) { obj = o; }
    void setT(double tt) { t = tt; }
    void setN(const Vec3d& n) { N = n; }
    void setMaterial(const Material& m)  { if(material) *material = m; else material = new Material(m); }
    void setUVCoordinates( const Vec2d& coords ) { uvCoordinates = coords; }
    void setBary(const Vec3d& weights) { bary = weights; }
    void setBary(const double alpha, const double beta, const double gamma)
		{ bary[0] = alpha; bary[1] = beta; bary[2] = gamma; }
    const Material &getMaterial() const;

public:
    const SceneObject *obj;
    double t;   //distance to intersection point
    Vec3d N;    //Normal
    Vec2d uvCoordinates; //UV Coordinates for textures
    Vec3d bary;          //alpha, beta, gamma
    Material *material;         // if this intersection has its own material
                                // (as opposed to one in its associated object)
                                // as in the case where the material was interpolated
};

Vec3d CosWeightedRandomHemiDir2(Vec3d n);

const double RAY_EPSILON = 0.00000001;

#endif // __RAY_H__
