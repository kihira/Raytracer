#include "Sphere.h"

Sphere::Sphere(const glm::vec3 &position, float radius, const glm::vec3 &colour) : position(position), radius(radius) {
    this->colour = colour;
}

bool Sphere::intersects(Ray *ray, float *distance) {
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