#include "Ray.h"
#include "shapes/Shape.h"

Ray::Ray(const glm::vec3 &direction, const glm::vec3 &origin) : direction(direction), origin(origin) {}

bool Ray::cast(std::vector<Shape *> &shapes, Shape **hitShape, float *hitDistance) {
    // Loop through the shapes and see if we hit
    float distance = 1000.f;
    *hitDistance = distance;
    for (auto& shape : shapes) {
        if (shape->intersects(this, &distance) && distance < *hitDistance) {
            *hitShape = shape;
            *hitDistance = distance;
        }
    }
    return hitShape == nullptr;
}
