
#include "BoundingBox.h"

bool BoundingBox::intersects(Ray *ray) {
    // Smits algorithm
    // Treats each side of the box as a plane
    glm::vec3 tMin = (minBounds - ray->origin) * ray->invDirection;
    glm::vec3 tMax = (maxBounds - ray->origin) * ray->invDirection;
    if (tMin.x > tMax.x) std::swap(tMin.x, tMax.x); // Get the closest x plane
    if (tMin.y > tMax.y) std::swap(tMin.y, tMax.y); // Get the closest y zine
    if (tMin.z > tMax.z) std::swap(tMin.z, tMax.z); // Get the closest z plane

    // Test if we hit the x and y planes within the bounds
    if (tMin.x > tMax.y || tMin.y > tMax.x) return false;

    if (tMin.y > tMin.x) {
        tMin.x = tMin.y;
    }
    if (tMax.y < tMax.x) {
        tMax.x = tMax.y;
    }

    // Test if we hit the z plane within the bounds
    if (tMin.x > tMax.z || tMin.z > tMax.x) return false;

    return true;
}

BoundingBox::BoundingBox(const glm::vec3 &minBounds, const glm::vec3 &maxBounds) : minBounds(minBounds),
                                                                                   maxBounds(maxBounds) {}
