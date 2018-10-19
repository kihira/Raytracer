#ifndef INC_322COM_RAYCASTER_RAY_H
#define INC_322COM_RAYCASTER_RAY_H

#include <vector>
#include <glm.hpp>

class Shape; // Forward declaration

struct Intersect {
    Shape *hitShape;
    float distance;
    glm::vec2 uv; // Used for triangle
    glm::vec3 hitPoint;
};

class Ray {
public:
    glm::vec3 direction;
    glm::vec3 origin;

    void setDirection(const glm::vec3 &direction);

    Ray(const glm::vec3 &origin, const glm::vec3 &direction);
    bool cast(std::vector<Shape *> &shapes, Intersect &intersect, Shape *ignore = nullptr);
};

#endif //INC_322COM_RAYCASTER_RAY_H
