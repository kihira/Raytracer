#ifndef INC_322COM_RAYCASTER_SHAPE_H
#define INC_322COM_RAYCASTER_SHAPE_H


#include <glm.hpp>
#include "../Ray.h"

class Shape {
public:
    glm::vec3 colour;
    virtual bool intersects(Ray *ray, float *distance) = 0;

    Shape() = default;
    virtual ~Shape() {};
};


#endif //INC_322COM_RAYCASTER_SHAPE_H
