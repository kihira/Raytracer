#include <ext.hpp>
#include "shape.h"

Shape::Shape(glm::vec3 position, Material material) : position(position), material(material) {
    modelMatrix = glm::mat4(1.f);
    modelMatrix = glm::translate(modelMatrix, position);
}

const Material &Shape::getMaterial() const {
    return material;
}

glm::vec3 Shape::getNormal(Intersect &intersect) {
    return normal;
}

Material::Material(glm::vec3 colour, glm::vec3 specular, float shininess)
: ambient(colour), diffuse(colour), specular(specular), shininess(shininess) {}

Material::Material(glm::vec3 &ambient, glm::vec3 &diffuse, glm::vec3 &specular, float shininess)
: ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess) {}
