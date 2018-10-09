#include <iostream>
#include <glm.hpp>
#include <fstream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ext.hpp>
#include "shapes/Sphere.h"
#include "shapes/Plane.h"
#include "Image.h"
#include "shapes/Triangle.h"

const int WIDTH = 640;
const int HEIGHT = 480;
const float FOV = 120;
const glm::vec3 CAMERA(0, 0, 0);
const glm::vec3 BACKGROUND(1.f, 1.f, 1.f);

std::vector<Shape*> shapes;

static const char* vertexShaderSource = R"(
#version 330
layout (location = 0) in vec2 pos;
out vec2 uv;
void main() {
    uv = vec2((pos.x + 1) / 2, 1 - ((pos.y + 1) / 2));
    gl_Position = vec4(pos, 0, 1);
})";

static const char* fragmentShaderSource = R"(
#version 330
in vec2 uv;
uniform sampler2D tex;
out vec4 outCol;
void main() {
    outCol = vec4(texture(tex, uv).rgb, 1);
}
)";

static const float vertices[16] {
        -1, -1,
        1, -1,
        1, 1,
        1, 1,
        -1, 1,
        -1, -1
};

void glErrorCheck() {
    GLuint err = glGetError();
    if (err != 0) {
        std::cerr << "GL Error 0x" << std::hex << err << std::endl;
    }
}

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

void raycast(Image *image) {
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
                image->setValue(x, y, shapeClosest->colour);
            }
        }
    }
}

inline void initScene() {
    // Create spheres
    shapes.push_back(new Sphere(glm::vec3(0, 0, -20), 4, glm::vec3(1, .32, .36)));
    shapes.push_back(new Sphere(glm::vec3(5, -1, -15), 2, glm::vec3(.9, .76, .46)));
    shapes.push_back(new Sphere(glm::vec3(5, 0, -25), 3, glm::vec3(.65, .77, .97)));
    shapes.push_back(new Sphere(glm::vec3(-5.5, 0, -15), 3, glm::vec3(.9, .9, .9)));

    // Triangle
    shapes.push_back(new Triangle(new glm::vec3[3]{glm::vec3(0, 1, -2), glm::vec3(-1.9, -1, -2), glm::vec3(1.6, -0.5, -2)}, glm::vec3(.6, .3, .5)));

    // Floor
    shapes.push_back(new Plane(glm::vec3(0, -10, 0), glm::vec3(0, -1, 0), glm::vec3(.2, .2, .2)));
}

void glfwFramebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
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

    window = glfwCreateWindow(WIDTH, HEIGHT, "322COM Raycaster", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, glfwFramebufferSizeCallback);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to load GLAD" << std::endl;
        return -1;
    }
    glfwSwapInterval(1);

    // Create image/texture raycaster will write to
    auto image = new Image(WIDTH, HEIGHT);

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
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*) 0);
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

    // Update loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Raycast
        raycast(image);

        // Write image to texture (Don't need to rebind, should be bound)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, image->getData());
        glErrorCheck();

        // Draw result
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glErrorCheck();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup resources
    delete image;
    for (auto shape : shapes) {
        delete shape;
    }
    glfwTerminate();

    return 0;
}