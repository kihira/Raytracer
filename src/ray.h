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
		distance = 0.f;
		triangleIndex = 0;
    }
};

class Ray {
public:
    glm::vec3 direction;
	glm::vec3 invDirection;
    glm::vec3 origin;
    int depth;

    Ray(const glm::vec3 &origin, const glm::vec3 &direction);

    /**
     * Cast the ray against the list of provided shapes. Returns true if a shape is hit.
     * Intersect holds the information about the hit shape if one was hit.
     * If there is only a need to check to see if it hits any shape, hitAny can be set to true
     * @param shapes
     * @param intersect
     * @param hitAny
     * @return Whether a shape was hit
     */
    bool cast(std::vector<Shape *> &shapes, Intersect &intersect, bool hitAny = false);

    void setOrigin(const glm::vec3 &origin);

    void setDirection(const glm::vec3 &direction);
};

#endif //INC_322COM_RAYCASTER_RAY_H
