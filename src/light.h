
#ifndef INC_322COM_RAYCASTER_LIGHT_H
#define INC_322COM_RAYCASTER_LIGHT_H


#include <glm.hpp>

class Light {
private:
    glm::vec3 position;
    glm::vec3 ambientIntensity;
    glm::vec3 intensity;
public:
    Light(const glm::vec3 &position, const glm::vec3 &ambientIntensity, const glm::vec3 &intensity);

    const glm::vec3 &getPosition() const;

    const glm::vec3 &getAmbientIntensity() const;

    const glm::vec3 &getIntensity() const;
};


#endif //INC_322COM_RAYCASTER_LIGHT_H
