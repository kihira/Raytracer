#ifndef INC_322COM_RAYCASTER_RAY_H
#define INC_322COM_RAYCASTER_RAY_H

#include <vector>
#include <glm.hpp>

class Shape; // Forward declaration

struct Intersect {
    Shape *hitShape;
    float distance;
    int triangleIndex; // Used for mesh
    glm::vec2 uv; // Used for triangle
    glm::vec3 hitPoint;
    void reset() {
        hitShape = nullptr;
    }
};

class Ray {
public:
    glm::vec3 direction;
	glm::vec3 invDirection;
    glm::vec3 origin;
    int depth;

    Ray(const glm::vec3 &origin, const glm::vec3 &direction);

    bool cast(std::vector<Shape *> &shapes, Intersect &intersect, Shape *ignore = nullptr);

    void setOrigin(const glm::vec3 &origin);

    void setDirection(const glm::vec3 &direction);
};

#endif //INC_322COM_RAYCASTER_RAY_H
