#include "ray.h"
#include "shapes/shape.h"

Ray::Ray(const glm::vec3 &origin, const glm::vec3 &direction) : direction(direction), origin(origin) {}

bool Ray::cast(std::vector<Shape *> &shapes, Intersect &intersect, Shape *ignore) {
    // Loop through the shapes and see if we hit
	glm::vec2 uv;
    float distance = INFINITY;
    intersect.distance = distance;
	int triangleIndex = -1;

    for (auto& shape : shapes) {
        if (shape == ignore) {
            continue;
        }
        if (shape->intersects(this, &distance, uv, &triangleIndex) && distance > 0 && distance < intersect.distance) {
            intersect.hitShape = shape;
            intersect.distance = distance;
			intersect.uv = uv;
			intersect.triangleIndex = triangleIndex;
        }
    }
    return intersect.hitShape != nullptr;
}

void Ray::setDirection(const glm::vec3 &direction) {
    Ray::direction = direction;
	invDirection = 1.f / direction;
}

void Ray::setOrigin(const glm::vec3 &origin) {
    Ray::origin = origin;
}
