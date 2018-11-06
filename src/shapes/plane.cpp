#include "plane.h"
#include "../ray.h"

Plane::Plane(const glm::vec3 &position, const glm::vec3 &normal, Material material) : Shape(position, material) {
    this->normal = normal;
}

bool Plane::intersects(Ray *ray, float *distance, glm::vec2 &uv, int *triangleIndex) {
    // Check if the rays are in the same direction or not. If it's almost 0 then they're pointing the same direction
    float dotProduct = glm::dot(normal * -1.f, ray->direction);
    if (dotProduct < 1e-6) return false;

    *distance = glm::dot((position - ray->origin), normal * -1.f) / dotProduct;

    return *distance >= 0.f;
}
