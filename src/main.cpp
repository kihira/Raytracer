#include <iostream>
#include <glm.hpp>
#include <fstream>
#include <vector>
#include "Sphere.h"

const int WIDTH = 640;
const int HEIGHT = 480;
const float FOV = 30;
const glm::vec3 CAMERA(0, 0, 0);

std::vector<Sphere*> spheres;

void write(glm::vec3 **image) {
    std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
    ofs << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
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

            // Loop through the spheres and see if we hit
            for (auto& sphere : spheres) {

            }

            // Debug: Set pixel output to ray direction
            image[x][y].r = ray.x;
            image[x][y].g = ray.y;
            image[x][y].b = ray.z;
        }
    }
}

int main() {
    // Initialise the image
    auto **image = new glm::vec3*[WIDTH];

    for (int x = 0; x < WIDTH; ++x) {
        image[x] = new glm::vec3[HEIGHT];
    }

    // Create spheres
    spheres.push_back(new Sphere(glm::vec3(0, 0, -20), 4, glm::vec3(1, .32, .36)));
    spheres.push_back(new Sphere(glm::vec3(5, -1, -15), 2, glm::vec3(.9, .76, .46)));
    spheres.push_back(new Sphere(glm::vec3(5, 0, -25), 3, glm::vec3(.65, .77, .97)));
    spheres.push_back(new Sphere(glm::vec3(-5.5, 0, -15), 3, glm::vec3(.9, .9, .9)));
    // Floor
    spheres.push_back(new Sphere(glm::vec3(0, -10004, -20), 10000, glm::vec3(.2, .2, .2)));

    raycast(image);

    write(image);

    return 0;
}