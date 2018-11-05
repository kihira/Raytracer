
#ifndef INC_322COM_RAYCASTER_MESH_H
#define INC_322COM_RAYCASTER_MESH_H


#include "shape.h"
#include "triangle.h"

class Mesh : public Shape {
private:
    std::vector<Triangle *> triangles;
    glm::vec3 minBounds{}; // AABB
    glm::vec3 maxBounds{};
public:
    Mesh(const glm::vec3 &position, std::vector<Triangle *> triangles, const Material &material);

    glm::vec3 getNormal(Intersect &intersect) override;

private:
    bool intersects(Ray *ray, float *distance, glm::vec2 &uv, int *triangleIndex) override;
};


#endif //INC_322COM_RAYCASTER_MESH_H
