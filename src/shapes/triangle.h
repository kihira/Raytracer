#ifndef INC_322COM_RAYCASTER_TRIANGLE_H
#define INC_322COM_RAYCASTER_TRIANGLE_H


#include "shape.h"

class Triangle : public Shape {
private:
    glm::vec3 *vertices;
    glm::vec3 *normals;
public:
    bool intersects(Ray *ray, float *distance, glm::vec2 &uv) override;

    Triangle(glm::vec3 position, glm::vec3 *vertices, glm::vec3 *normals, Material material);

    glm::vec3 getNormal(Intersect &intersect) override;
};


#endif //INC_322COM_RAYCASTER_TRIANGLE_H