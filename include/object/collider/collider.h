#pragma once

#include "frustum.h"

class Collider
{
    virtual bool isOnFrustum(const Frustum& camFrustum, const class Transform& transform) const = 0;

	virtual bool isOnOrForwardPlane(const Plane& plane) const = 0;

	bool isOnFrustum(const Frustum& frustum) const
	{
		return (isOnOrForwardPlane(frustum.leftFace) &&
			isOnOrForwardPlane(frustum.rightFace) &&
			isOnOrForwardPlane(frustum.bottomFace) &&
			isOnOrForwardPlane(frustum.nearFace) &&
			isOnOrForwardPlane(frustum.farFace));
	}
};