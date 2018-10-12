#include "Ray.h"
#include "shapes/Shape.h"

Ray::Ray(const glm::vec3 &direction, const glm::vec3 &origin) : direction(direction), origin(origin) {}

bool Ray::cast(std::vector<Shape *> &shapes, Shape **hitShape) {
    // Loop through the shapes and see if we hit
    float closestDist = 1000.f;
    float distance = 1000.f;
    for (auto& shape : shapes) {
        if (shape->intersects(this, &distance) && distance < closestDist) {
            *hitShape = shape;
            closestDist = distance;
        }
    }
    return hitShape == nullptr;
}
