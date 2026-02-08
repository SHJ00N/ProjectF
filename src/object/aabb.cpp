#include "object/collider/aabb.h"
#include "model/model.h"
#include "object/transform.h"

#include <glm/glm.hpp>


AABB::AABB(const Model &model)
{
    // check model's min, max range
    glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());
	for (auto&& mesh : model.meshes)
	{
		for (auto&& vertex : mesh.vertices)
		{
			minAABB.x = std::min(minAABB.x, vertex.Position.x);
			minAABB.y = std::min(minAABB.y, vertex.Position.y);
			minAABB.z = std::min(minAABB.z, vertex.Position.z);

			maxAABB.x = std::max(maxAABB.x, vertex.Position.x);
			maxAABB.y = std::max(maxAABB.y, vertex.Position.y);
			maxAABB.z = std::max(maxAABB.z, vertex.Position.z);
		}
    }

    center = (maxAABB + minAABB) * 0.5f;
    extents = glm::vec3(maxAABB.x - center.x, maxAABB.y - center.y, maxAABB.z - center.z);
}

AABB::AABB(const glm::vec3& inCenter, float iI, float iJ, float iK) : center{ inCenter }, extents{ iI, iJ, iK }
{
}

bool AABB::isOnOrForwardPlane(const Plane &plane) const
{
    // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
    const float r = extents.x * std::abs(plane.normal.x) + extents.y * std::abs(plane.normal.y) +
        extents.z * std::abs(plane.normal.z);

    return -r <= plane.getSignedDistanceToPlane(center);
}

bool AABB::isOnFrustum(const Frustum& camFrustum, const Transform& transform) const
{
    //Get global scale thanks to our transform
    const glm::vec3 globalCenter{ transform.GetModelMatrix() * glm::vec4(center, 1.f) };

    // Scaled orientation
    const glm::vec3 right = transform.GetRight() * extents.x;
    const glm::vec3 up = transform.GetUp() * extents.y;
    const glm::vec3 forward = transform.GetForward() * extents.z;

    const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

    const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

    const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

    const AABB globalAABB(globalCenter, newIi, newIj, newIk);

    return (globalAABB.isOnOrForwardPlane(camFrustum.leftFace) &&
        globalAABB.isOnOrForwardPlane(camFrustum.rightFace) &&
        globalAABB.isOnOrForwardPlane(camFrustum.topFace) &&
        globalAABB.isOnOrForwardPlane(camFrustum.bottomFace) &&
        globalAABB.isOnOrForwardPlane(camFrustum.nearFace) &&
        globalAABB.isOnOrForwardPlane(camFrustum.farFace));
};

AABB AABB::GetGlobalAABB(const Transform &transform)
{
    //Get global scale thanks to our transform
    const glm::vec3 globalCenter{ transform.GetModelMatrix() * glm::vec4(center, 1.f) };

    // Scaled orientation
    const glm::vec3 right = transform.GetRight() * extents.x;
    const glm::vec3 up = transform.GetUp() * extents.y;
    const glm::vec3 forward = transform.GetForward() * extents.z;

    const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

    const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

    const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

    return AABB(globalCenter, newIi, newIj, newIk);
}