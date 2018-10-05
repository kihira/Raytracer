#include <iostream>
#include <glm.hpp>
#include <fstream>
#include <vector>
#include "Sphere.h"

const int WIDTH = 640;
const int HEIGHT = 480;
const float FOV = 90;
const glm::vec3 CAMERA(0, 0, 0);
const glm::vec3 BACKGROUND(1.f, 1.f, 1.f);

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

/**
 * Calculates whether the ray intersects the sphere and where it hits on the sphere.
 * @param ray The ray to test
 * @param sphere The sphere to test
 * @param hitDistance This value is set to the closest distance between the sphere and CAMERA
 * @return Whether the ray hit or not
 */
bool intersects_sphere(glm::vec3& ray, Sphere* sphere, float* hitDistance) {
    // Construct a right angle triangle between center of sphere, origin and ray
    glm::vec3 L = sphere->position - CAMERA;
    float tca = glm::dot(L, ray);
    if (tca < 0) return false; // Object is opposite direction of the ray

    // d2 is the distance squared between the ray and sphere center
    float d2 = glm::dot(L, L) - (tca * tca);
    float radius2 = sphere->radius * sphere->radius;
    // We square the radius instead of sqrt the distance because it's cheaper
    if (d2 > radius2) return false; // If distance between ray and sphere is greater then radius, not hit

    // Calculate the closest hit point. TODO optimise
    float tHitCenter = sqrt(radius2 - d2);
    float hit1 = tca - tHitCenter;
    float hit2 = tca + tHitCenter;

    if (hit1 < hit2) hitDistance = &hit1;
    else hitDistance = &hit2;

    return true;
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
            Sphere* sphereClosest = nullptr;
            float sphereClosestDist = 1000;
            float* distance;
            *distance = 1000.f;
            for (auto& sphere : spheres) {
                if (intersects_sphere(ray, sphere, distance) && *distance < sphereClosestDist) {
                    sphereClosest = sphere;
                    sphereClosestDist = *distance;
                }
            }
            if (sphereClosest != nullptr) {
                image[x][y] = sphereClosest->colour;
            }
            delete distance;
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