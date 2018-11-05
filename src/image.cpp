#include "image.h"

glm::vec3 &Image::getValue(int &x, int &y) const
{
    return data[width * y + x];
}

void Image::setValue(int &x, int &y, const glm::vec3 color) const
{
    data[width * y + x] = glm::clamp(color, glm::vec3(0.f), glm::vec3(1.f));
}

glm::vec3* Image::getData() const {
    return data;
}

Image::Image(const int width, const int height, const glm::vec3 background) : width(width), height(height), background(background) {
    data = new glm::vec3[width * height];
}

Image::~Image() {
    delete[] data;
}

const glm::vec3 &Image::getBackground() const {
    return background;
}

void Image::resize(const int newWidth, const int newHeight) {
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
