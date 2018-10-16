#include <iostream>
#include <glm.hpp>
#include <fstream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ext.hpp>
#include <thread>
#include <objloader.h>
#include "shapes/Sphere.h"
#include "shapes/Plane.h"
#include "Image.h"
#include "shapes/Triangle.h"

#define THREADS 4
#define RENDER_ON_UPDATE false
#define MULTITHREAD true
// #define LIGHTING true

static const char *vertexShaderSource = R"(
#version 330
layout (location = 0) in vec2 pos;
out vec2 uv;
void main() {
    uv = vec2((pos.x + 1) / 2, 1 - ((pos.y + 1) / 2));
    gl_Position = vec4(pos, 0, 1);
})";

static const char *fragmentShaderSource = R"(
#version 330
in vec2 uv;
uniform sampler2D tex;
out vec4 outCol;
void main() {
    outCol = vec4(texture(tex, uv).rgb, 1);
}
)";

static const float vertices[16]{
        -1, -1,
        1, -1,
        1, 1,
        1, 1,
        -1, 1,
        -1, -1
};

const float FOV = 120;
glm::vec3 CAMERA(0, 0, 0);

Image *image;
std::vector<Shape *> shapes;

// Lights
glm::vec3 ambientIntensity(.2f, .2f, .2f);

glm::vec3 pointLightPosition(10, 10, 0);
glm::vec3 pointLightIntensity(1.f, 1.f, 1.f);

void glErrorCheck() {
    GLuint err = glGetError();
    if (err != 0) {
        std::cerr << "GL Error 0x" << std::hex << err << std::endl;
    }
}

void write(Image *image) {
    std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
    ofs << "P6\n" << image->getWidth() << " " << image->getHeight() << "\n255\n";

    for (int y = 0; y < image->getHeight(); ++y) {
        for (int x = 0; x < image->getWidth(); ++x) {
            glm::vec3 data = image->getValue(x, y);
            ofs << (unsigned char) (data.x * 255)
                << (unsigned char) (data.y * 255)
                << (unsigned char) (data.z * 255);
        }
    }

    ofs.close();
}

glm::vec3 calculateLighting(Ray *ray, Shape *shape, float distance) {
#ifdef LIGHTING
    Material mat = shape->getMaterial();
    glm::vec3 intersectionPoint = ray->direction * distance;
    glm::vec3 lightRay = glm::normalize(pointLightPosition - intersectionPoint);

    // Check if we are in shadow. If so, just return ambient colour;
    Ray *shadowRay = new Ray(intersectionPoint, lightRay);
    Shape *closestShape = nullptr;
    if (shadowRay->cast(shapes, &closestShape, &distance, shape)) {
        delete shadowRay;
        return mat.ambient * ambientIntensity;
    }
    delete shadowRay;

    glm::vec3 normal = shape->getNormal(intersectionPoint);
    glm::vec3 reflection = 2.f * glm::dot(lightRay, normal) * normal - lightRay;
    glm::vec3 viewDir = ray->direction * -1.f;

    glm::vec3 colour(0.f, 0.f, 0.f);
    colour += mat.ambient * ambientIntensity; // Ambient
    colour += mat.diffuse * (pointLightIntensity * fmax(0.f, glm::dot(lightRay, normal))); // Diffuse
    colour += mat.specular * pointLightIntensity *
              pow(fmax(0.f, glm::dot(reflection, viewDir)), mat.shininess); // Specular

    return colour;
#else
    return shape->getMaterial().diffuse;
#endif
}

/**
 * Transforms a position into a world position
 * @param world
 * @param origin
 * @param out
 * @return
 */
glm::vec3 &getWorldOrigin(glm::mat4 &world, glm::vec3 &origin, glm::vec3 &out) {
    float w = origin[0] * world[0][3] + origin[1] * world[1][3] + origin[2] * world[2][3] + world[3][3];
    out.x = origin[0] * world[0][0] + origin[1] * world[1][0] + origin[2] * world[2][0] + world[3][0];
    out.y = origin[0] * world[0][1] + origin[1] * world[1][1] + origin[2] * world[2][1] + world[3][1];
    out.z = origin[0] * world[0][2] + origin[1] * world[1][2] + origin[2] * world[2][2] + world[3][2];

    out.x /= w;
    out.y /= w;
    out.z /= w;
    return out;
}

/**
 * Transform a direction into a world direction
 * @param world
 * @param dir
 * @param out
 * @return
 */
glm::vec3 &getWorldDirection(glm::mat4 &world, glm::vec3 &dir, glm::vec3 &out) {
    out.x = dir[0] * world[0][0] + dir[1] * world[1][0] + dir[2] * world[2][0];
    out.y = dir[0] * world[0][1] + dir[1] * world[1][1] + dir[2] * world[2][1];
    out.z = dir[0] * world[0][2] + dir[1] * world[1][2] + dir[2] * world[2][2];

    return out;
}

static void raycast(Image *image, int xStart, int xCount) {
    float aspectRatio = (float) image->getWidth() / image->getHeight();
    float fovHalfTan = tanf(glm::radians(FOV) / 2.f);
    glm::mat4 camToWorld(1.f);
    glm::vec2 normalised, remapped;
    glm::vec3 rayOrigin, rayDirection;
    Ray *ray = new Ray(getWorldOrigin(camToWorld, CAMERA, rayOrigin), CAMERA);

    for (int x = xStart; x < xStart + xCount; ++x) {
        for (int y = 0; y < image->getHeight(); ++y) {
            // Remap to 0:1
            normalised.x = (x + 0.5f) / image->getWidth();
            normalised.y = (y + 0.5f) / image->getHeight();

            // Remap to -1:1
            remapped.x = (2.f * normalised.x - 1.f) * aspectRatio;
            remapped.y = 1.f - 2.f * normalised.y;

            glm::vec3 cameraSpace(remapped.x * fovHalfTan, remapped.y * fovHalfTan, -1);
            ray->setDirection(glm::normalize(getWorldDirection(camToWorld, cameraSpace, rayDirection)));

            // Loop through the shapes and see if we hit
            Shape *shapeClosest = nullptr;
            float distance;

            if (ray->cast(shapes, &shapeClosest, &distance)) {
                // Calculate lighting
                image->setValue(x, y, calculateLighting(ray, shapeClosest, distance));
            } else {
                image->setValue(x, y, image->getBackground());
            }
        }
    }
    delete ray;
}

void renderScene(Image *image) {
    using namespace std::chrono;
    std::cout << "Starting render..." << std::endl;
    milliseconds startTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

#ifdef MULTITHREAD
    std::thread threads[THREADS];
    int xPortions = image->getWidth() / 4;

    // Need to run these in a lambda to capture the functions and variables
    threads[0] = std::thread([image, xPortions](){ return raycast(image, 0, xPortions);});
    threads[1] = std::thread([image, xPortions](){ return raycast(image, xPortions, xPortions);});
    threads[2] = std::thread([image, xPortions](){ return raycast(image, xPortions * 2, xPortions);});
    threads[3] = std::thread([image, xPortions](){ return raycast(image, xPortions * 3, xPortions);});

    for (auto &thread : threads) {
        thread.join();
    }
#else
    raycast(image, 0, image->getWidth());
#endif

    // Write image to texture (Don't need to rebind, should be bound)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->getWidth(), image->getHeight(), 0, GL_RGB, GL_FLOAT,
                 image->getData());
    glErrorCheck();

    std::cout << "Render Details" << std::endl << "================" << std::endl;
    std::cout << "Time: " << (duration_cast<milliseconds>(system_clock::now().time_since_epoch()) - startTime).count()
              << "ms" << std::endl;
}

inline void initScene() {
    // Create spheres
    shapes.push_back(new Sphere(glm::vec3(0, 0, -20), 4,
                                {glm::vec3(1.f, .32f, .36f), glm::vec3(1.f, .32f, .36f), glm::vec3(.7f, .7f, .7f), 128.f}));
    shapes.push_back(new Sphere(glm::vec3(5, -1, -15), 2,
                                {glm::vec3(.9f, .76f, .46f), glm::vec3(.9f, .76f, .46f), glm::vec3(.7f, .7f, .7f), 128.f}));
    shapes.push_back(new Sphere(glm::vec3(5, 0, -25), 3,
                                {glm::vec3(.65f, .77f, .97f), glm::vec3(.65f, .77f, .97f), glm::vec3(.7f, .7f, .7f), 128.f}));
    shapes.push_back(new Sphere(glm::vec3(-5.5, 0, -15), 3,
                                {glm::vec3(.9f, .9f, .9f), glm::vec3(.9f, .9f, .9f), glm::vec3(.7f, .7f, .7f), 128.f}));

    // Triangle
//    shapes.push_back(new Triangle(
//            new glm::vec3[3]{{0, 1, -2}, {-1.9, -1, -2}, {1.6, -0.5, -2}},
//            new glm::vec3[3]{{0, 0.6, 1}, {-0.4, -0.4, 1}, {0.4, -0.4, 1}},
//            {{0.5, 0.5, 0}, {0.5, 0.5, 0}, {0.7, 0.7, 0.7}, 100}));

    // Floor
    shapes.push_back(new Plane(glm::vec3(0, -5, 0), glm::vec3(0, -1, 0),
                               {glm::vec3(.8f, .8f, .8f), glm::vec3(.8f, .8f, .8f), glm::vec3(.7f, .7f, .7f), 0.f}));

    // Teapot
//    glm::vec3 teapotPosition(0, 0, -10);
//    std::vector<glm::vec3> vertices;
//    std::vector<glm::vec3> normals;
//    Material teapotMat {
//        glm::vec3(.5f, .5f, 0.f),
//        glm::vec3(.5f, .5f, 0.f),
//        glm::vec3(.7f, .7f, .7f),
//        100.f
//    };
//    loadOBJ("./teapot_smooth.obj", vertices, normals);
//    for (int i = 0; i < vertices.size(); i+=3) {
//        // Offset vertices position
//        vertices[i] += teapotPosition;
//        vertices[i+1] += teapotPosition;
//        vertices[i+2] += teapotPosition;
//        shapes.push_back(new Triangle(&vertices[i], &normals[i], teapotMat));
//    }
}

void glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) return;

    switch (key) {
        case GLFW_KEY_W:
            CAMERA.z -= 1.f;
            break;
        case GLFW_KEY_S:
            CAMERA.z += 1.f;
            break;
        case GLFW_KEY_A:
            CAMERA.x -= 1.f;
            break;
        case GLFW_KEY_D:
            CAMERA.x += 1.f;
            break;
        case GLFW_KEY_F:
            CAMERA.y -= 1.f;
            break;
        case GLFW_KEY_R:
            CAMERA.y += 1.f;
            break;
        case GLFW_KEY_ENTER:
            renderScene(image);
            break;
        default:break;
    }
}

void glfwFramebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    image->resize(width, height);

    renderScene(image);
}

int main() {
    GLFWwindow *window;
    GLuint vao, vertexBuffer, vertexShader, fragmentShader, program, raycastTexture;

    // Initialize OpenGL and GLFW for realtime rendering
    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif

    window = glfwCreateWindow(640, 480, "322COM Raycaster", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to load GLAD" << std::endl;
        return -1;
    }
    glfwSwapInterval(1);

    // Setup callbacks
    glfwSetFramebufferSizeCallback(window, glfwFramebufferSizeCallback);
    glfwSetKeyCallback(window, glfwKeyCallback);

    // Create image/texture raycaster will write to
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    image = new Image(width, height, glm::vec3(1.f, 1.f, 1.f));

    // Generate basic shader program
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glUseProgram(program);
    glErrorCheck();

    // Create basic rectangle to draw our image to
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void *) 0);
    glErrorCheck();

    // Generate texture to store our raycast output to
    glGenTextures(1, &raycastTexture);
    glBindTexture(GL_TEXTURE_2D, raycastTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glErrorCheck();

    initScene();
    renderScene(image);

    // Update loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render
#if RENDER_ON_UPDATE
        renderScene(image);
#endif

        // Draw result
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glErrorCheck();

        glfwSwapBuffers(window);
        //glfwPollEvents();
        glfwWaitEvents(); // Wait for new input before rendering
    }

    // Cleanup resources
    delete image;
    for (auto shape : shapes) {
        delete shape;
    }
    glfwTerminate();

    return 0;
}