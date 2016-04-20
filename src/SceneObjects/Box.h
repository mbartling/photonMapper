#ifndef __BOX_H__
#define __BOX_H__

#include "../scene/scene.h"

class Box : public MaterialSceneObject {
public:
	Box( Scene *scene, Material *mat )
		: MaterialSceneObject( scene, mat )
	{
	}

	virtual bool intersectLocal(ray& r, isect& i ) const;
	virtual bool hasBoundingBoxCapability() const { return true; }

    virtual BoundingBox ComputeLocalBoundingBox()
    {
        BoundingBox localbounds;
        localbounds.setMax(Vec3d(0.5, 0.5, 0.5));
		localbounds.setMin(Vec3d(-0.5, -0.5, -0.5));
        return localbounds;
    }

protected:
	void glDrawLocal(int quality, bool actualMaterials, bool actualTextures) const;
};

#endif // __BOX_H__
