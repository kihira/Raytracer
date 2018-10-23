
#include "boxLight.h"

BoxLight::BoxLight(const glm::vec3 &position, const glm::vec3 &size, const glm::vec3 &ambientIntensity,
                   const glm::vec3 &intensity) : Light(position, ambientIntensity, intensity), size(size) {

}

const glm::vec3 &BoxLight::getSize() const {
    return size;
}
