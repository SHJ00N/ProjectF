#pragma once

#include "frustum.h"
#include <vector>

class Collider
{
public:
	class GameObject *Owner = nullptr;

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

	virtual bool isIntersectsAABB(const Collider &other) const = 0;

	void AddCollision(Collider *other)
	{
		m_currentCollisions.push_back(other);
	}
	void BeginFrame()
	{
		m_previousCollisions = m_currentCollisions;
		m_currentCollisions.clear();
	}

	std::vector<Collider*>& GetCurrentCollisions() { return m_currentCollisions; }
	std::vector<Collider*>& GetPreviousCollisions() { return m_previousCollisions; }
private:
	std::vector<Collider*> m_currentCollisions;
	std::vector<Collider*> m_previousCollisions;
};