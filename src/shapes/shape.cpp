#include <ext.hpp>
#include "shape.h"

Shape::Shape(const glm::vec3 position, const Material material) : position(position), material(material) {
    modelMatrix = glm::mat4(1.f);
    modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3(1.f, 0.f, 0.f));
	modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0.f, 1.f, 0.f));
	modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3(0.f, 0.f, 1.f));
}

const Material &Shape::getMaterial() const {
    return material;
}

glm::vec3 Shape::getNormal(Intersect &intersect) {
    return normal;
}

Material::Material(const glm::vec3 color, const glm::vec3 specular, const float shininess)
: ambient(color), diffuse(color), specular(specular), shininess(shininess) {}

Material::Material(const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular, const float shininess)
: ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess) {}
