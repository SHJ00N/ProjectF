#pragma once

#include "object/collider/collider.h"

class AABB : public Collider
{
public:
    // members
    glm::vec3 center{ 0.0f, 0.0f, 0.0f };
    glm::vec3 extents{ 0.0f, 0.0f, 0.0f };
    // constructors
    AABB(const class Model &model);
    AABB(const glm::vec3& inCenter, float iI, float iJ, float iK);
    // override functions
    bool isOnOrForwardPlane(const Plane& plane) const final;
    bool isOnFrustum(const Frustum& camFrustum, const class Transform& transform) const final;

    // world space aabb
    AABB GetGlobalAABB(const class Transform& transform);
};