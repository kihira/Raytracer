
#include "mesh.h"
#include <utility>
#include <iostream>

Mesh::Mesh(const glm::vec3 &position, std::vector<Triangle *> triangles, const Material &material) : Shape(position, material), triangles(std::move(triangles))
{
	// Set min/max to just the first vertex for now
	minBounds = Mesh::triangles[0]->getVertices()[0];
	maxBounds = Mesh::triangles[0]->getVertices()[0];

	for (auto triangle : Mesh::triangles)
	{
		for (auto i = 0; i < 3; ++i)
		{
			auto vertex = triangle->getVertices()[i];
			if (vertex.x < minBounds.x) minBounds.x = vertex.x;
			else if (vertex.x > maxBounds.x) maxBounds.x = vertex.x;
			if (vertex.y < minBounds.y) minBounds.y = vertex.y;
			else if (vertex.y > maxBounds.y) maxBounds.y = vertex.y;
			if (vertex.z < minBounds.z) minBounds.z = vertex.z;
			else if (vertex.z > maxBounds.z) maxBounds.z = vertex.z;
		}
	}
	// Don't need to transform the min/max bounds by position as the vertices positions were already transformed
}

bool Mesh::intersects(Ray *ray, float *distance, glm::vec2 &uv, int *triangleIndex) {
    // Check if it intersects bounding box first
    float tMin = (minBounds.x - ray->origin.x) / ray->direction.x;
    float tMax = (maxBounds.x - ray->origin.x) / ray->direction.x;
    if (tMin > tMax) std::swap(tMin, tMax);

    float tyMin = (minBounds.y - ray->origin.y) / ray->direction.y;
    float tyMax = (maxBounds.y - ray->origin.y) / ray->direction.y;
    if (tyMin > tyMax) std::swap(tyMin, tyMax);

    if (tMin > tyMax || tyMin > tMax) return false;

    if (tyMin > tMin) {
        tMin = tyMin;
    }

    if (tyMax < tMax) {
        tMax = tyMax;
    }

    float tzmin = (minBounds.z - ray->origin.z) / ray->direction.z;
    float tzmax = (maxBounds.z - ray->origin.z) / ray->direction.z;

    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    if ((tMin > tzmax) || (tzmin > tMax))
        return false;

	// Loop through the triangles and find the closest one
	// This is effectively the same code as Ray::cast
	auto distance2 = 0.f;
	glm::vec2 uv2;
    for (auto i = 0; i < triangles.size(); ++i) {
        if (triangles[i]->intersects(ray, &distance2, uv2, nullptr) && distance2 > 0.f && distance2 < *distance) {
			*distance = distance2;
            *triangleIndex = i;
			uv = uv2;
        }
    }
    return *triangleIndex > -1;
}

glm::vec3 Mesh::getNormal(Intersect &intersect) {
    return triangles[intersect.triangleIndex]->getNormal(intersect);
}
