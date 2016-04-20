#include <cmath>

#include "Sphere.h"

using namespace std;


bool Sphere::intersectLocal(ray& r, isect& i) const
{
	Vec3d v = -r.getPosition();
	double b = v * r.getDirection();
	double discriminant = b*b - v*v + 1;

	if( discriminant < 0.0 ) {
		return false;
	}

	discriminant = sqrt( discriminant );
	double t2 = b + discriminant;

	if( t2 <= RAY_EPSILON ) {
		return false;
	}

	i.obj = this;
	i.setMaterial(this->getMaterial());

	double t1 = b - discriminant;

	if( t1 > RAY_EPSILON ) {
		i.t = t1;
		i.N = r.at( t1 );
		i.N.normalize();
	} else {
		i.t = t2;
		i.N = r.at( t2 );
		i.N.normalize();
	}

	return true;
}

