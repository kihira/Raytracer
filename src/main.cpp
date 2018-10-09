#include <iostream>
#include <glm.hpp>
#include <fstream>
#include <vector>
#include "Sphere.h"
#include "Plane.h"

const int WIDTH = 640;
const int HEIGHT = 480;
const float FOV = 90;
const glm::vec3 CAMERA(0, 0, 0);
const glm::vec3 BACKGROUND(1.f, 1.f, 1.f);

std::vector<Shape*> shapes;

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
            // todo should move this into the Ray class
            // Remap to 0:1
            float xNormalised = (x + 0.5f) / WIDTH;
            float yNormalised = (y + 0.5f) / HEIGHT;

            // Remap to -1:1
            float xRemapped = (2 * xNormalised - 1) * aspectRatio;
            float yRemapped = 1 - 2 * yNormalised;

            float xCamera = xRemapped * tanf(glm::radians(FOV) / 2);
            float yCamera = yRemapped * tanf(glm::radians(FOV) / 2);

            glm::vec3 cameraSpace(xCamera, yCamera, -1);
            Ray *ray = new Ray(glm::normalize(cameraSpace - CAMERA), CAMERA);

            // Loop through the shapes and see if we hit
            Shape* shapeClosest = nullptr;
            float shapeClosestDist = 1000;
            float distance = 1000.f;
            for (auto& shape : shapes) {
                if (shape->intersects(ray, &distance) && distance < shapeClosestDist) {
                    shapeClosest = shape;
                    shapeClosestDist = distance;
                }
            }
            if (shapeClosest != nullptr) {
                image[x][y] = shapeClosest->colour;
            }
        }
    }
}

int main() {
    // Initialise the image
    auto **image = new glm::vec3*[WIDTH];

    for (int x = 0; x < WIDTH; ++x) {
        image[x] = new glm::vec3[HEIGHT];

        // Set the initial color
        for (int y = 0; y < HEIGHT; ++y) {
            image[x][y] = BACKGROUND;
        }
    }

    // Create spheres
    shapes.push_back(new Sphere(glm::vec3(0, 0, -20), 4, glm::vec3(1, .32, .36)));
    shapes.push_back(new Sphere(glm::vec3(5, -1, -15), 2, glm::vec3(.9, .76, .46)));
    shapes.push_back(new Sphere(glm::vec3(5, 0, -25), 3, glm::vec3(.65, .77, .97)));
    shapes.push_back(new Sphere(glm::vec3(-5.5, 0, -15), 3, glm::vec3(.9, .9, .9)));
    // Floor
    shapes.push_back(new Plane(glm::vec3(0, -10, 0), glm::vec3(0, -1, 0), glm::vec3(.2, .2, .2)));

    raycast(image);

    write(image);

    return 0;
}