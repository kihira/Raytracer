#ifndef INC_322COM_RAYCASTER_RAY_H
#define INC_322COM_RAYCASTER_RAY_H


#include <glm.hpp>

class Ray {
public:
    glm::vec3 direction;
    glm::vec3 origin;

    Ray(const glm::vec3 &direction, const glm::vec3 &origin);
};


#endif //INC_322COM_RAYCASTER_RAY_H
