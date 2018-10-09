#include "Triangle.h"

bool Triangle::intersects(Ray *ray, float *distance) {
    glm::vec3 e1 = vertices[1] - vertices[0];
    glm::vec3 e2 = vertices[2] - vertices[0];

    glm::vec3 de2 = glm::cross(ray->direction, e2);
    float det = glm::dot(e1, de2); // Determinate (Normal of the triangle)

    // Aiming in the same direction so we're only seeing the back
    if (det < 1e-6) return false;

    glm::vec3 oa = ray->origin - vertices[0];

    // Calculate u and test
    float u = glm::dot(oa, de2);
    if (u < 0 || u > det) return false;

    float v = glm::dot(ray->direction, glm::cross(oa, e1));
    if (v < 0 || u + v > det) return false;

    *distance = glm::dot(e2, glm::cross(oa, e1)) * (1.0 - det);
    return true;
}

Triangle::Triangle(glm::vec3 *vertices, const glm::vec3 &colour) : vertices(vertices) {
    this->colour = colour;
}

Triangle::~Triangle() {
    delete[] vertices;
}
