#include "Triangle.h"

bool Triangle::intersects(Ray *ray, float *distance) {
    glm::vec3 e1 = vertices[1] - vertices[0];
    glm::vec3 e2 = vertices[2] - vertices[0];

    glm::vec3 oa = ray->origin - vertices[0];
    glm::vec3 de2 = ray->direction * e2;

    float u = glm::dot(oa, de2) / glm::dot(e1, de2);
    float v = glm::dot(ray->direction, oa * e1) / glm::dot(e1, de2);

    if (u < 0 || u > 1 || v < 0 || u + v > 1) return false;

    *distance = glm::dot(e2, (oa * e1) / glm::dot(e1, de2));
    return true;
}

Triangle::Triangle(glm::vec3 *vertices, const glm::vec3 &colour) : vertices(vertices) {
    this->colour = colour;
}

Triangle::~Triangle() {
    delete[] vertices;
}
