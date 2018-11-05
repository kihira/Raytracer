#include "ray.h"
#include "shapes/shape.h"

Ray::Ray(const glm::vec3 &origin, const glm::vec3 &direction) : direction(direction), origin(origin) {}

bool Ray::cast(std::vector<Shape *> &shapes, Intersect &intersect, Shape *ignore) {
    // Loop through the shapes and see if we hit
    float distance = INFINITY;
    intersect.distance = distance;
    Shape *hitShape = nullptr;

    for (auto& shape : shapes) {
        if (shape == ignore) {
            continue;
        }
        if (shape->intersects(this, &distance, intersect.uv, &intersect.triangleIndex) && distance > 0 && distance < intersect.distance) {
            intersect.hitShape = hitShape;
            intersect.distance = distance;
        }
    }
    return intersect.hitShape != nullptr;
}

void Ray::setDirection(const glm::vec3 &direction) {
    Ray::direction = direction;
}

void Ray::setOrigin(const glm::vec3 &origin) {
    Ray::origin = origin;
}
