
#ifndef INC_322COM_RAYCASTER_BOUNDINGBOX_H
#define INC_322COM_RAYCASTER_BOUNDINGBOX_H


#include <glm.hpp>
#include "ray.h"

class BoundingBox {
private:
    glm::vec3 minBounds; // AABB
    glm::vec3 maxBounds;
public:
    BoundingBox(const glm::vec3 &minBounds, const glm::vec3 &maxBounds);

    bool intersects(Ray *ray);
};


#endif //INC_322COM_RAYCASTER_BOUNDINGBOX_H
