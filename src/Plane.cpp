#include "Plane.h"
#include "Ray.h"

Plane::Plane(const glm::vec3 &position, const glm::vec3 &normal) : position(position), normal(normal) {}

bool Plane::intersects(Ray *ray, float *distance) {
    return false;
}
