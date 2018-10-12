#include "Shape.h"

Shape::Shape(Material material) : material(material) {}

const Material &Shape::getMaterial() const {
    return material;
}

glm::vec3 Shape::getNormal(glm::vec3 &intersectionPoint) {
    return normal;
}

glm::vec3 Shape::getPosition() const {
    return glm::vec3();
}
