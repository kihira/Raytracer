#include <ext.hpp>
#include "shape.h"

Shape::Shape(glm::vec3 position, Material material) : position(position), material(material) {
    modelMatrix = glm::mat4(1.f);
    modelMatrix = glm::translate(modelMatrix, position);
}

const Material &Shape::getMaterial() const {
    return material;
}

glm::vec3 Shape::getNormal(glm::vec3 &intersectionPoint) {
    return normal;
}

