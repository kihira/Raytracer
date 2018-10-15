#include "Triangle.h"

// TODO BUG This doesn't work in multithread as we're writing to u and v across multiple threads
bool Triangle::intersects(Ray *ray, float *distance) {
    glm::vec3 e1 = vertices[1] - vertices[0];
    glm::vec3 e2 = vertices[2] - vertices[0];

    glm::vec3 de2 = glm::cross(ray->direction, e2);
    float det = glm::dot(e1, de2); // Determinate (Normal of the triangle)

    // Aiming in the same direction so we're only seeing the back
    if (det < 1e-6) return false;

    float invDet = 1.f / det;
    glm::vec3 oa = ray->origin - vertices[0];

    // Calculate u and test
    u = glm::dot(oa, de2) * invDet;
    if (u < 0 || u > 1) return false;

    v = glm::dot(ray->direction, glm::cross(oa, e1)) * invDet;
    if (v < 0 || u + v > 1) return false;

    *distance = glm::dot(e2, glm::cross(oa, e1)) * invDet;
    return true;
}

Triangle::Triangle(glm::vec3 *vertices, glm::vec3 *normals, Material material) : Shape(material), vertices(vertices), normals(normals) {}

glm::vec3 Triangle::getNormal(glm::vec3 &intersectionPoint) {
    // u and v were calculated earlier, now we can just calculate w
    w = 1.f - u - v;

    return glm::normalize(w * normals[0] + u * normals[1] + v * normals[2]);
}
