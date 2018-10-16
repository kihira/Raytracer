#ifndef INC_322COM_RAYCASTER_PLANE_H
#define INC_322COM_RAYCASTER_PLANE_H


#include <glm.hpp>
#include "shape.h"
#include "../ray.h"

class Plane : public Shape {
public:
    Plane(const glm::vec3 &position, const glm::vec3 &normal, Material material);
    bool intersects(Ray *ray, float *distance) override;
};


#endif //INC_322COM_RAYCASTER_PLANE_H
