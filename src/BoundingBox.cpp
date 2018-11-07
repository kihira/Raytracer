
#include "BoundingBox.h"

bool BoundingBox::intersects(Ray *ray) {
    // Check if the ray is starting inside the bounding box
    if (ray->origin.x > minBounds.x && ray->origin.y > minBounds.y && ray->origin.z > minBounds.z &&
        ray->origin.x < maxBounds.x && ray->origin.y < maxBounds.y && ray->origin.z < maxBounds.z) {
        return true;
    }

    glm::vec3 t;

    // Calculate distances (t) to the closest face of the box on each axis
    for (int i = 0; i < 3; ++i) {
        if (ray->direction[i] > 0) {
            t[i] = (minBounds[i] - ray->origin[i]) * ray->invDirection[i];
        } else {
            t[i] = (maxBounds[i] - ray->origin[i]) * ray->invDirection[i];
        }
    }

    // The largest distance (t) is the plane that got hit
    int plane = 0;
    for (int i = 1; i < 3; ++i) {
        if (t[plane] < t[i]) plane = i;
    }

    // Now check if we've landed inside the box
    for (int i = 0; i < 3; ++i) {
        if (plane != i) {
            auto coord = ray->origin[i] + t[plane] * ray->direction[i];
            if (coord < minBounds[i] || coord > maxBounds[i]) {
                return false;
            }
        }
    }
    return true;
}

BoundingBox::BoundingBox(const glm::vec3 &minBounds, const glm::vec3 &maxBounds) : minBounds(minBounds),
                                                                                   maxBounds(maxBounds) {}
