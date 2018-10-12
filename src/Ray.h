#ifndef INC_322COM_RAYCASTER_RAY_H
#define INC_322COM_RAYCASTER_RAY_H

#include <vector>
#include <glm.hpp>

class Shape; // Forward declaration

class Ray {
public:
    glm::vec3 direction;
    glm::vec3 origin;

    Ray(const glm::vec3 &direction, const glm::vec3 &origin);
    bool cast(std::vector<Shape *> &shapes, Shape **hitShape, float *hitDistance);
};


#endif //INC_322COM_RAYCASTER_RAY_H
