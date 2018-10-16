#include "triangle.h"

#define EPSILON 1e-6f
// #define CULL_BACKFACE

Triangle::Triangle(glm::vec3 position, glm::vec3 *vertices, glm::vec3 *normals, Material material) : Shape(position, material), vertices(vertices), normals(normals) {
    vertices[0] = modelMatrix * glm::vec4(vertices[0], 1);
    vertices[1] = modelMatrix * glm::vec4(vertices[1], 1);
    vertices[2] = modelMatrix * glm::vec4(vertices[2], 1);
}

// TODO BUG This doesn't work in multithread as we're writing to u and v across multiple threads
bool Triangle::intersects(Ray *ray, float *distance) {
    glm::vec3 e1 = vertices[1] - vertices[0];
    glm::vec3 e2 = vertices[2] - vertices[0];

    glm::vec3 de2 = glm::cross(ray->direction, e2);
    float det = glm::dot(e1, de2); // Determinate (Normal of the triangle)

#ifdef CULL_BACKFACE
    // Aiming in the same direction so we're only seeing the back
    // This breaks shadow casting
    if (det < 1e-6) return false;
#else
    if (det > -EPSILON && det < EPSILON) return false;
#endif

    float invDet = 1.f / det;
    glm::vec3 oa = ray->origin - vertices[0];

    // Calculate u and test
    float u = glm::dot(oa, de2) * invDet;
    if (u < 0 || u > 1) return false;

    float v = glm::dot(ray->direction, glm::cross(oa, e1)) * invDet;
    if (v < 0 || u + v > 1) return false;

    *distance = glm::dot(e2, glm::cross(oa, e1)) * invDet;
    return true;
}

glm::vec3 Triangle::getNormal(glm::vec3 &intersectionPoint) {
    // u and v were calculated earlier, now we can just calculate w
    w = 1.f - u - v;

    return glm::normalize(w * normals[0] + u * normals[1] + v * normals[2]);
}
