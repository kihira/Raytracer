#include "Ray.h"
#include "shapes/Shape.h"

#define MAX_DISTANCE 1000.f

Ray::Ray(const glm::vec3 &origin, const glm::vec3 &direction) : direction(direction), origin(origin) {}

bool Ray::cast(std::vector<Shape *> &shapes, Shape **hitShape, float *hitDistance, Shape *ignore) {
    // Loop through the shapes and see if we hit
    float distance = MAX_DISTANCE;
    *hitDistance = distance;
    for (auto& shape : shapes) {
        if (shape == ignore) {
            continue;
        }
        if (shape->intersects(this, &distance) && distance < *hitDistance) {
            *hitShape = shape;
            *hitDistance = distance;
        }
    }
    return *hitShape != nullptr;
}
