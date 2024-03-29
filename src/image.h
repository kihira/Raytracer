#ifndef INC_322COM_RAYCASTER_IMAGE_H
#define INC_322COM_RAYCASTER_IMAGE_H


#include <glm.hpp>

/**
 * A class that stores data about an image
 * The data is stored as a 1D array to allow for it to be uploaded to OpenGL easier
 */
class Image {
public:
    /**
     * Gets the colour value of the pixel at this position
     * @param x The x coordination
     * @param y The y coordination
     * @return The colour value
     */
    glm::vec3 &getValue(int &x, int &y) const;

    /**
     * Sets the color value of the pixel at this position
     * @param x The x coordination
     * @param y The y coordination
     * @param color The color value
     */
    void setValue(int &x, int &y, glm::vec3 color) const;

    /**
     * A pointer to the pixel data for this image
     * @return
     */
    glm::vec3 *getData() const;

    /**
     * Gets the default background colour for the image
     * @return The colour
     */
    const glm::vec3 &getBackground() const;

    void resize(int newWidth, int newHeight);

    int getWidth() const;

    int getHeight() const;

    Image(int width, int height, const glm::vec3 background);

    ~Image();

private:
    int width;
    int height;
    glm::vec3 background;
    glm::vec3 *data;
};


#endif //INC_322COM_RAYCASTER_IMAGE_H
