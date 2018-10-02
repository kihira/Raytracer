#include <iostream>
#include <glm.hpp>
#include <fstream>

const int WIDTH = 100;
const int HEIGHT = 100;
const float FOV = 30;
const glm::vec3 CAMERA(0, 0, 0);

void write(glm::vec3 **image) {
    std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
    ofs << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";

    for (int x = 0; x < WIDTH; ++x) {
        for (int y = 0; y < HEIGHT; ++y) {
            ofs << (unsigned char)(image[x][y].x * 255)
                << (unsigned char)(image[x][y].y * 255)
                << (unsigned char)(image[x][y].z * 255);
        }
    }

    ofs.close();
}

void raycast(glm::vec3 **image) {
    float aspectRatio = WIDTH / HEIGHT;

    for (int x = 0; x < WIDTH; ++x) {
        for (int y = 0; y < HEIGHT; ++y) {
            // Remap to 0:1
            float xNormalised = (x + 0.5f) / WIDTH;
            float yNormalised = (y + 0.5f) / HEIGHT;

            // Remap to -1:1
            float xRemapped = (2 * xNormalised - 1) * aspectRatio;
            float yRemapped = 1 - 2 * yNormalised;

            float xCamera = xRemapped * tanf(glm::radians(FOV) / 2);
            float yCamera = yRemapped * tanf(glm::radians(FOV) / 2);

            glm::vec3 cameraSpace(xCamera, yCamera, -1);
            glm::vec3 ray = glm::normalize(cameraSpace - CAMERA);

            // Debug: Set pixel output to ray direction
            image[x][y].r = ray.x;
            image[x][y].g = ray.y;
            image[x][y].b = ray.z;
        }
    }
}

int main() {
    // Initialise the image
    glm::vec3 **image = new glm::vec3*[WIDTH];

    for (int x = 0; x < WIDTH; ++x) {
        image[x] = new glm::vec3[HEIGHT];
        for (int y = 0; y < HEIGHT; ++y) {
            image[x][y] = glm::vec3(1, 1, 0);
        }
    }

    raycast(image);

    write(image);

    return 0;
}