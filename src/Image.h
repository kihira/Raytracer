#ifndef INC_322COM_RAYCASTER_IMAGE_H
#define INC_322COM_RAYCASTER_IMAGE_H


#include <glm.hpp>

/**
 * A class that stores data about an image
 * The data is stored as a 1D array to allow for it to be uploaded to OpenGL easier
 */
class Image {
public:
    glm::vec3 & getValue(int &x, int &y);
    void setValue(int &x, int &y, glm::vec3 colour);
    glm::vec3 *getData() const;
    glm::vec3 & operator [](int i);

    Image(int width, int height);
    ~Image();

private:
    int width;
    int height;
    glm::vec3 *data;
};


#endif //INC_322COM_RAYCASTER_IMAGE_H
