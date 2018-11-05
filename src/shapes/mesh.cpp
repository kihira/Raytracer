
#include "mesh.h"
#include <utility>

Mesh::Mesh(const glm::vec3 &position, const glm::vec3 &minBounds, const glm::vec3 &maxBounds, std::vector<Triangle *> triangles, const Material &material)
        : Shape(position, material), triangles(std::move(triangles)), minBounds(minBounds), maxBounds(maxBounds)
{
	
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
