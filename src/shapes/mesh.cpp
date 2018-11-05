
#include "mesh.h"

Mesh::Mesh(const glm::vec3 &position, const glm::vec3 &minBounds, const glm::vec3 &maxBounds, const std::vector<Triangle *> triangles, const Material &material)
        : Shape(position, material), minBounds(minBounds), maxBounds(maxBounds), triangles(triangles) {}

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

    bool result;
    for (int i = 0; i < triangles.size(); ++i) {
        result = triangles[i]->intersects(ray, distance, uv, nullptr);
        if (result) {
            triangleIndex = &i;
            return true;
        }
    }
    return false;
}

glm::vec3 Mesh::getNormal(Intersect &intersect) {
    // intersect.hitShape
    return triangles[intersect.triangleIndex]->getNormal(intersect);
}
