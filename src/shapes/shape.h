#ifndef INC_322COM_RAYCASTER_SHAPE_H
#define INC_322COM_RAYCASTER_SHAPE_H


#include <glm.hpp>
#include "../ray.h"
#include "../boundingBox.h"

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;

    Material(glm::vec3 color, glm::vec3 specular, float shininess);
    Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);
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
	glm::vec3 rotation{ 0.f, 0.f, 0.f };
    glm::vec3 normal;
    Material material;
    BoundingBox *aabb;
};


#endif //INC_322COM_RAYCASTER_SHAPE_H
