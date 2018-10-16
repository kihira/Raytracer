#ifndef INC_322COM_RAYCASTER_SPHERE_H
#define INC_322COM_RAYCASTER_SPHERE_H


#include <glm.hpp>
#include "shape.h"
#include "../ray.h"

class sphere : public Shape {
private:
    float radius;
public:
    sphere(const glm::vec3 &position, float radius, Material material);
    /**
    * Calculates whether the ray intersects the sphere and where it hits on the sphere.
    * @param ray The ray to test
    * @param distance This value is set to the closest distance between the sphere and ray->origin
    * @return Whether the ray hit or not
    */
    bool intersects(Ray *ray, float *distance) override;

    glm::vec3 getNormal(glm::vec3 &intersectionPoint) override;
};


#endif //INC_322COM_RAYCASTER_SPHERE_H
