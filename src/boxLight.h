
#ifndef INC_322COM_RAYCASTER_BOXLIGHT_H
#define INC_322COM_RAYCASTER_BOXLIGHT_H


#include <glm.hpp>
#include "light.h"

class BoxLight : public Light {
private:
    glm::vec3 position;
    glm::vec3 size;
public:
    BoxLight(const glm::vec3 &position, const glm::vec3 &size, const glm::vec3 &ambientIntensity, const glm::vec3 &intensity);

    const glm::vec3 &getSize() const;
};


#endif //INC_322COM_RAYCASTER_BOXLIGHT_H
