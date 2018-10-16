
#include "light.h"

Light::Light(const glm::vec3 &position, const glm::vec3 &ambientIntensity, const glm::vec3 &intensity) : position(
        position), ambientIntensity(ambientIntensity), intensity(intensity) {}

const glm::vec3 &Light::getPosition() const {
    return position;
}

const glm::vec3 &Light::getAmbientIntensity() const {
    return ambientIntensity;
}

const glm::vec3 &Light::getIntensity() const {
    return intensity;
}
