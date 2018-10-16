
#include <ext.hpp>
#include "light.h"
#include "mathelper.hpp"

Light::Light(const glm::vec3 &position, const glm::vec3 &ambientIntensity, const glm::vec3 &intensity) : ambientIntensity(ambientIntensity), intensity(intensity) {
    modelMatrix = glm::mat4(1.f);
    modelMatrix = glm::translate(modelMatrix, position);
    getWorldOrigin(modelMatrix, glm::vec3(0.f), Light::position);
}

const glm::vec3 &Light::getPosition() const {
    return position;
}

const glm::vec3 &Light::getAmbientIntensity() const {
    return ambientIntensity;
}

const glm::vec3 &Light::getIntensity() const {
    return intensity;
}
