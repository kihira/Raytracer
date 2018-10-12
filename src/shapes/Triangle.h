#ifndef INC_322COM_RAYCASTER_TRIANGLE_H
#define INC_322COM_RAYCASTER_TRIANGLE_H


#include "Shape.h"

class Triangle : public Shape {
private:
    glm::vec3 *vertices;
public:
    bool intersects(Ray *ray, float *distance) override;

    Triangle(glm::vec3 *vertices, Material material);
    ~Triangle();
};


#endif //INC_322COM_RAYCASTER_TRIANGLE_H
