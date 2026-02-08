#pragma once

#include <glm/glm.hpp>

struct Plane
{
    glm::vec3 normal = {0.0f, 1.0f, 0.0f}; // unit vector
    float distance = 0.0f; // distance with origin

    Plane() = default;

	Plane(const glm::vec3& p1, const glm::vec3& norm) : normal(glm::normalize(norm)), distance(glm::dot(normal, p1)) { }

	float getSignedDistanceToPlane(const glm::vec3& point) const
	{
		return glm::dot(normal, point) - distance;
	}
};

struct Frustum
{
    Plane topFace;
	Plane bottomFace;

	Plane rightFace;
	Plane leftFace;

	Plane farFace;
	Plane nearFace;
};