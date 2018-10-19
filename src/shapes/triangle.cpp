#include "triangle.h"

#define EPSILON 1e-6f
// #define CULL_BACKFACE

Triangle::Triangle(glm::vec3 position, glm::vec3 *vertices, glm::vec3 *normals, Material material) : Shape(position, material), vertices(vertices), normals(normals) {
    Triangle::vertices[0] = modelMatrix * glm::vec4(vertices[0], 1);
    Triangle::vertices[1] = modelMatrix * glm::vec4(vertices[1], 1);
    Triangle::vertices[2] = modelMatrix * glm::vec4(vertices[2], 1);
}

bool Triangle::intersects(Ray *ray, float *distance, glm::vec2 &uv) {
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
    uv.x = glm::dot(oa, de2) * invDet;
    if (uv.x < 0 || uv.x > 1) return false;

    uv.y = glm::dot(ray->direction, glm::cross(oa, e1)) * invDet;
    if (uv.y < 0 || uv.x + uv.y > 1) return false;

    *distance = glm::dot(e2, glm::cross(oa, e1)) * invDet;
    return true;
}

glm::vec3 Triangle::getNormal(Intersect &intersect) {
    // u and v were calculated earlier, now we can just calculate w
    float w = 1.f - intersect.uv.x - intersect.uv.y;

    return glm::normalize(w * normals[0] + intersect.uv.x * normals[1] + intersect.uv.y * normals[2]);
}
