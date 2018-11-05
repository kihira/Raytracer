
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
	glm::vec3 tMin = (minBounds - ray->origin) * ray->invDirection;
	glm::vec3 tMax = (maxBounds - ray->origin) * ray->invDirection;
    if (tMin.x > tMax.x) std::swap(tMin.x, tMax.x);
    if (tMin.y > tMax.y) std::swap(tMin.y, tMax.y);

    if (tMin.x > tMax.y || tMin.y > tMax.x) return false;

    if (tMin.y > tMin.x) {
        tMin.x = tMin.y;
    }

    if (tMax.y < tMax.x) {
        tMax.x = tMax.y;
    }

    if (tMin.z > tMax.z) std::swap(tMin.z, tMax.z);

    if (tMin.x > tMax.z || tMin.z > tMax.x) return false;

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
