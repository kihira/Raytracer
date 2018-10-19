#include "sphere.h"

Sphere::Sphere(const glm::vec3 &position, float radius, Material material)
        : Shape(position, material), radius(radius) {
}

bool Sphere::intersects(Ray *ray, float *distance, glm::vec2 &uv) {
    // Construct a right angle triangle between center of sphere, origin and ray
    glm::vec3 L = this->position - ray->origin;
    float tca = glm::dot(L, ray->direction);
    if (tca < 0) return false; // Object is opposite direction of the ray

    // d2 is the distance squared between the ray and sphere center
    float d2 = glm::dot(L, L) - (tca * tca);
    float radius2 = this->radius * this->radius;
    // We square the radius instead of sqrt the distance because it's cheaper
    if (d2 > radius2) return false; // If distance between ray and sphere is greater then radius, not hit

    // Calculate the closest hit point. TODO optimise
    float tHitCenter = sqrt(radius2 - d2);
    float hit1 = tca - tHitCenter;
    float hit2 = tca + tHitCenter;

    if (hit1 < hit2) *distance = hit1;
    else *distance = hit2;

    return true;
}

glm::vec3 Sphere::getNormal(Intersect &intersect) {
    return glm::normalize(intersect.hitPoint - position);
}
