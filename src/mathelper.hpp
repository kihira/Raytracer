
#ifndef INC_322COM_RAYCASTER_MATHELPER_H
#define INC_322COM_RAYCASTER_MATHELPER_H

#include <glm.hpp>

namespace {
/**
 * Transforms a position into a world position
 * @param world
 * @param origin
 * @param out
 * @return
 */
    glm::vec3 &getWorldOrigin(glm::mat4 &world, const glm::vec3 &origin, glm::vec3 &out) {
        float x, y, z, w;
        x = origin[0] * world[0][0] + origin[1] * world[1][0] + origin[2] * world[2][0] + world[3][0];
        y = origin[0] * world[0][1] + origin[1] * world[1][1] + origin[2] * world[2][1] + world[3][1];
        z = origin[0] * world[0][2] + origin[1] * world[1][2] + origin[2] * world[2][2] + world[3][2];
        w = origin[0] * world[0][3] + origin[1] * world[1][3] + origin[2] * world[2][3] + world[3][3];

        out.x = x / w;
        out.y = y / w;
        out.z = z / w;
        return out;
    }

/**
 * Transform a direction into a world direction
 * @param world
 * @param dir
 * @param out
 * @return
 */
    glm::vec3 &getWorldDirection(glm::mat4 &world, const glm::vec3 &dir, glm::vec3 &out) {
        out.x = dir[0] * world[0][0] + dir[1] * world[1][0] + dir[2] * world[2][0];
        out.y = dir[0] * world[0][1] + dir[1] * world[1][1] + dir[2] * world[2][1];
        out.z = dir[0] * world[0][2] + dir[1] * world[1][2] + dir[2] * world[2][2];

        return out;
    }
}

#endif // INC_322COM_RAYCASTER_MATHELPER_H