#ifndef INC_322COM_RAYCASTER_TRIANGLE_H
#define INC_322COM_RAYCASTER_TRIANGLE_H


#include "Shape.h"

class Triangle : public Shape {
private:
    glm::vec3 *vertices;
    glm::vec3 *normals;
    float u, v, w;
public:
    bool intersects(Ray *ray, float *distance) override;

    Triangle(glm::vec3 *vertices, glm::vec3 *normals, Material material);

    glm::vec3 getNormal(glm::vec3 &intersectionPoint) override;
};


#endif //INC_322COM_RAYCASTER_TRIANGLE_H
