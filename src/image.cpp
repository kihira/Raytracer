#include "image.h"

glm::vec3 &Image::getValue(int &x, int &y) {
    return data[width * y + x];
}

void Image::setValue(int &x, int &y, glm::vec3 colour) {
    data[width * y + x] = colour;
}

glm::vec3* Image::getData() const {
    return data;
}

Image::Image(int width, int height, const glm::vec3 background) : width(width), height(height), background(background) {
    data = new glm::vec3[width * height];
}

Image::~Image() {
    delete[] data;
}

const glm::vec3 &Image::getBackground() const {
    return background;
}

void Image::resize(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;

    // Delete old data and recreate
    delete[] data;
    data = new glm::vec3[width * height];
}

int Image::getWidth() const {
    return width;
}

int Image::getHeight() const {
    return height;
}
