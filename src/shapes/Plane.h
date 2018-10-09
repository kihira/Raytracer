#ifndef INC_322COM_RAYCASTER_PLANE_H
#define INC_322COM_RAYCASTER_PLANE_H


#include <glm.hpp>
#include "Shape.h"
#include "../Ray.h"

class Plane : public Shape {
private:
    glm::vec3 position;
    glm::vec3 normal;
public:
    Plane(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec3 &colour);
    bool intersects(Ray *ray, float *distance) override;
};


#endif //INC_322COM_RAYCASTER_PLANE_H
