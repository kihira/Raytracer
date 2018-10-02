#ifndef INC_322COM_RAYCASTER_SPHERE_H
#define INC_322COM_RAYCASTER_SPHERE_H


#include <glm.hpp>

class Sphere {
    glm::vec3 position;
    float radius;
    glm::vec3 colour;

public:
    Sphere(const glm::vec3 &position, float radius, const glm::vec3 &colour);
};


#endif //INC_322COM_RAYCASTER_SPHERE_H
