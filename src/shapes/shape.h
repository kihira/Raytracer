#ifndef INC_322COM_RAYCASTER_SHAPE_H
#define INC_322COM_RAYCASTER_SHAPE_H


#include <glm.hpp>
#include "../ray.h"

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;

    Material(glm::vec3 colour, glm::vec3 specular, float shininess);
    Material(glm::vec3 &ambient, glm::vec3 &diffuse, glm::vec3 &specular, float shininess);
};

class Shape {
public:
    Shape(glm::vec3 position, Material material);

    virtual ~Shape() = default;

    virtual bool intersects(Ray *ray, float *distance, glm::vec2 &uv, int *triangleIndex) = 0;

    virtual glm::vec3 getNormal(Intersect &intersect);

    const Material &getMaterial() const;

protected:
    glm::mat4 modelMatrix;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 normal;
    Material material;
};


#endif //INC_322COM_RAYCASTER_SHAPE_H
