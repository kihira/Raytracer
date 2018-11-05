#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <fstream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ext.hpp>
#include <thread>
#include <objloader.h>
#include <gtx/euler_angles.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "shapes/sphere.h"
#include "shapes/plane.h"
#include "image.h"
#include "shapes/triangle.h"
#include "light.h"
#include "boxLight.h"
#include "shapes/mesh.h"

#define EPSILON 1e-4
#define THREADS 4
#define LIGHTING

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

struct Camera {
    glm::vec3 position = glm::vec3(0.f);
    glm::mat4 viewMatrix = glm::mat4(1.f);
    float fov = 90.f;

    void updateViewMatrix() {
        // viewMatrix = glm::lookAt(position, glm::vec3(0, 0, -10), glm::vec3(0, 1, 0));
        viewMatrix = glm::translate(glm::mat4(1.f), position);

    }
} camera;

struct RenderInfo {
    long long renderTime;
    unsigned int primaryRays;
    unsigned int shadowRays;
    unsigned int reflectionRays;

    void reset() {
        primaryRays = 0;
        shadowRays = 0;
        reflectionRays = 0;
    }
} gRenderInfos[THREADS];

struct RenderSettings {
    int softShadowSamples = 64;
    float sssSqrt = 8;
    float shadowJitter = 1.f;
    bool shadows = false;
    bool reflections = true;
    int maxReflectionDepth = 1;
} renderSettings;

std::unique_ptr<Image> image;
std::unique_ptr<BoxLight> light;
std::vector<Shape *> shapes;
std::vector<Ray *> rays;
std::thread threads[THREADS];

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

/**
 * Generates a number between 0.f and renderSettings.shadowJitter (1.f max)
 * @return The random shadow jitter
 */
inline float getShadowJitter() {
    return ((rand() % 100) / 100.f) * renderSettings.shadowJitter;
}

glm::vec3 calculateLighting(Ray *ray, Intersect &intersect, RenderInfo &renderInfo) {
#ifdef LIGHTING
    Material mat = intersect.hitShape->getMaterial();
    intersect.hitPoint = ray->origin + ray->direction * intersect.distance;
    glm::vec3 normal = intersect.hitShape->getNormal(intersect);
    glm::vec3 viewDir = ray->direction * -1.f;
    glm::vec3 colour(0.f);

    if (renderSettings.shadows) {
        float xSpacing = light->getSize().x / renderSettings.sssSqrt;
        float zSpacing = light->getSize().z / renderSettings.sssSqrt;
        float xStart = light->getPosition().x - light->getSize().x / 2.f;
        float zStart = light->getPosition().z - light->getSize().z / 2.f;

        Ray *shadowRay = new Ray(intersect.hitPoint, intersect.hitPoint);

        // Shoot out multiple rays for soft shadows
        for (int x = 0; x < renderSettings.sssSqrt; ++x) {
            for (int z = 0; z < renderSettings.sssSqrt; ++z) {
                glm::vec3 lightRay = glm::normalize(glm::vec3(xStart + xSpacing * x + (xSpacing * getShadowJitter()), light->getPosition().y, zStart + zSpacing * z + (xSpacing * getShadowJitter())) - intersect.hitPoint);
                shadowRay->setDirection(lightRay);

                Intersect shadowIntersect = Intersect();
                if (shadowRay->cast(shapes, shadowIntersect, intersect.hitShape)) {
                    colour += mat.ambient * light->getAmbientIntensity();
                    continue;
                }

                glm::vec3 reflection = 2.f * glm::dot(lightRay, normal) * normal - lightRay;

                colour += mat.ambient * light->getAmbientIntensity(); // Ambient
                colour += mat.diffuse * (light->getIntensity() * fmax(0.f, glm::dot(lightRay, normal))); // Diffuse
                colour += mat.specular * light->getIntensity() *
                          pow(fmax(0.f, glm::dot(reflection, viewDir)), mat.shininess); // Specular
            }
        }

        colour /= renderSettings.softShadowSamples; // Must divide here, not later otherwise it'll cancel out the reflection
        renderInfo.shadowRays += renderSettings.softShadowSamples;
        delete shadowRay;
    } else {
        glm::vec3 lightRay = glm::normalize(light->getPosition() - intersect.hitPoint);
        glm::vec3 reflection = 2.f * glm::dot(lightRay, normal) * normal - lightRay;

        colour += mat.ambient * light->getAmbientIntensity(); // Ambient
        colour += mat.diffuse * (light->getIntensity() * fmax(0.f, glm::dot(lightRay, normal))); // Diffuse
        colour += mat.specular * light->getIntensity() *
                  pow(fmax(0.f, glm::dot(reflection, viewDir)), mat.shininess); // Specular
    }

    // Calculate reflections
    if (mat.shininess > 0.f && renderSettings.reflections && ray->depth < renderSettings.maxReflectionDepth) {
        glm::vec3 reflectionDir = ray->direction - 2.f * glm::dot(ray->direction, normal) * normal;
        // Reuse main ray
        ray->setOrigin(intersect.hitPoint);
        ray->setDirection(reflectionDir);
        ray->depth += 1;
        Intersect reflectionIntersect = Intersect();
        if (ray->cast(shapes, reflectionIntersect, intersect.hitShape)) {
            colour += mat.specular * calculateLighting(ray, reflectionIntersect, renderInfo);
        }

        renderInfo.reflectionRays += 1;
    }
    ray->depth = 0;

    return colour;
#else
    return intersect.hitShape->getMaterial().diffuse;
#endif
}

static void raycast(int xStart, int xCount, RenderInfo &renderInfo) {
    float aspectRatio = (float) image->getWidth() / image->getHeight();
    float fovHalfTan = tanf(glm::radians(camera.fov) / 2.f);
    glm::vec2 normalised, remapped;
    glm::vec3 cameraOrigin = camera.viewMatrix * glm::vec4(0.f, 0.f, 0.f, 1.f);
	Ray *ray = new Ray(cameraOrigin, cameraOrigin);
    ray->setOrigin(cameraOrigin);

    for (int x = xStart; x < xStart + xCount; ++x) {
        for (int y = 0; y < image->getHeight(); ++y) {
            // Remap to 0:1
            normalised.x = (x + .5f) / image->getWidth();
            normalised.y = (y + .5f) / image->getHeight();

            // Remap to -1:1
            remapped.x = (2.f * normalised.x - 1.f) * aspectRatio;
            remapped.y = 1.f - 2.f * normalised.y;

            glm::vec3 cameraSpace(remapped.x * fovHalfTan, remapped.y * fovHalfTan, -1.f);
            ray->setOrigin(cameraOrigin);
            ray->setDirection(glm::normalize(camera.viewMatrix * glm::vec4(cameraSpace, 0.f)));

            // Reset intersect
            Intersect intersect = Intersect();

            // Loop through the shapes and see if we hit
            if (ray->cast(shapes, intersect)) {
                // Calculate lighting
                image->setValue(x, y, calculateLighting(ray, intersect, renderInfo));
            } else {
                image->setValue(x, y, image->getBackground());
            }
            renderInfo.primaryRays += 1;
        }
    }

	delete ray;
}

/**
 * Renders the main scene
 */
void renderScene() {
    using namespace std::chrono;
    std::cout << "Starting render..." << std::endl << std::endl;
    milliseconds startTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

    // Pre calculate soft shadow samples
    renderSettings.sssSqrt = sqrtf(renderSettings.softShadowSamples);

    int xPortions = image->getWidth() / THREADS;

    for (int i = 0; i < THREADS; ++i) {
        threads[i] = std::thread([xPortions, i](){ return raycast(xPortions * i, xPortions, gRenderInfos[i]);});
    }

    for (auto &thread : threads) {
        thread.join();
    }

    // Write image to texture (Don't need to rebind, should be bound)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->getWidth(), image->getHeight(), 0, GL_RGB, GL_FLOAT,
                 image->getData());
    glErrorCheck();

    // HACK: A bit of a hack but works for now
    for (int j = 1; j < THREADS; ++j) {
        gRenderInfos[0].primaryRays += gRenderInfos[j].primaryRays;
        gRenderInfos[0].reflectionRays += gRenderInfos[j].reflectionRays;
        gRenderInfos[0].shadowRays += gRenderInfos[j].shadowRays;
    }
    gRenderInfos[0].renderTime = (duration_cast<milliseconds>(system_clock::now().time_since_epoch()) - startTime).count();

    std::cout << "Render Details" << std::endl
              << "================" << std::endl
              << "Time: " << gRenderInfos[0].renderTime << "ms" << std::endl
              << "Objects: " << shapes.size() << std::endl
              << "Primary Rays: " << gRenderInfos[0].primaryRays << std::endl
              << "Reflection Rays: " << gRenderInfos[0].reflectionRays << std::endl
              << "Shadow Rays: " << gRenderInfos[0].shadowRays << std::endl
              << "================" << std::endl << std::endl;
}

/**
 * Creates the objects in the scene
 */
inline void initScene() {
    // Create spheres
//    shapes.push_back(new Sphere(glm::vec3(0, 0, -20), 4, {glm::vec3(1.f, .32f, .36f), glm::vec3(.2f), 20.f}));
//    shapes.push_back(new Sphere(glm::vec3(5, -1, -15), 2, {glm::vec3(.9f, .76f, .46f), glm::vec3(.9f), 20.f}));
//    shapes.push_back(new Sphere(glm::vec3(5, 0, -25), 3, {glm::vec3(.65f, .77f, .97f), glm::vec3(.5f), 20.f}));
//    shapes.push_back(new Sphere(glm::vec3(-5.5, 0, -15), 3, {glm::vec3(.9f), glm::vec3(.5f), 20.f}));

    // triangle
//    shapes.push_back(new Triangle(
//            {0, 0, -2},
//            new glm::vec3[3]{{0, 1, -2}, {-1.9, -1, -2}, {1.6, -0.5, -2}},
//            new glm::vec3[3]{{0, 0.6, 1}, {-0.4, -0.4, 1}, {0.4, -0.4, 1}},
//            {{0.5, 0.5, 0}, {0.5, 0.5, 0}, {0.7, 0.7, 0.7}, 100}));

    // Floor
    // shapes.push_back(new Plane(glm::vec3(0.f, -4.f, 0.f), glm::vec3(0.f, -1.f, 0.f), {glm::vec3(.8f), glm::vec3(.7f), 0.f}));

    // Teapot
    glm::vec3 teapotPosition(0, 2, -10);
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<Triangle *> triangles;
    Material teapotMat {
        glm::vec3(.5f, .5f, 0.f),
        glm::vec3(.7f, .7f, .7f),
        100.f
    };
    loadOBJ("./teapot_smooth.obj", vertices, normals);
    for (int i = 0; i < vertices.size(); i+=3) {
        triangles.push_back(new Triangle(teapotPosition, &vertices[i], &normals[i], teapotMat));
    }
    shapes.push_back(new Mesh(teapotPosition, glm::vec3(-7, 2, -13), glm::vec3(1, 6, -7), triangles, teapotMat));

    // Lights
    light = std::unique_ptr<BoxLight>(new BoxLight(glm::vec3(-4.5f, 20.f, -4.5f), glm::vec3(9.f, .1f, 9.f), glm::vec3(.2f), glm::vec3(1.f)));
}

/**
 * Handles key input
 */
void glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) return;

    switch (key) {
        case GLFW_KEY_W:
            camera.position.z -= 1.f;
            break;
        case GLFW_KEY_S:
            camera.position.z += 1.f;
            break;
        case GLFW_KEY_A:
            camera.position.x -= 1.f;
            break;
        case GLFW_KEY_D:
            camera.position.x += 1.f;
            break;
        case GLFW_KEY_F:
            camera.position.y -= 1.f;
            break;
        case GLFW_KEY_R:
            camera.position.y += 1.f;
            break;
        case GLFW_KEY_ENTER:
            renderScene();
            break;
        default:break;
    }

    camera.updateViewMatrix();
}

void glfwFramebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    image->resize(width, height);

    renderScene();
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

    // Setup imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplGlfw_InitForOpenGL(window, false);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();

    // Create image/texture raycaster will write to
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    image = std::unique_ptr<Image>(new Image(width, height, glm::vec3(1.f, 1.f, 1.f)));

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
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);
    glErrorCheck();

    // Generate texture to store our raycast output to
    glGenTextures(1, &raycastTexture);
    glBindTexture(GL_TEXTURE_2D, raycastTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glErrorCheck();

    // Pre allocate rays
    rays.reserve(THREADS);
    for (int i = 0; i < THREADS; ++i) {
        rays.push_back(new Ray(glm::vec3(0.f), glm::vec3(0.f)));
    }

    camera.updateViewMatrix();
    initScene();
    renderScene();

    // Update loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Start imgui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (ImGui::Begin("Scene Settings")) {
            ImGui::Text("%s", "Camera");
            ImGui::Separator();
            ImGui::DragFloat3("Position", &camera.position[0], 1.f);
            ImGui::SliderFloat("FOV", &camera.fov, 45.f, 180.f);

            ImGui::Separator();
            ImGui::Text("%s", "Shadow");
            ImGui::Separator();
            ImGui::Checkbox("Shadows", &renderSettings.shadows);
            ImGui::DragFloat("Shadow Jitter", &renderSettings.shadowJitter, .1f, 0.f, 1.f);
            ImGui::InputInt("Soft Shadow Samples", &renderSettings.softShadowSamples);

            ImGui::Separator();
            ImGui::Text("%s", "Reflections");
            ImGui::Separator();
            ImGui::Checkbox("Reflections", &renderSettings.reflections);
            ImGui::InputInt("Max Reflection Depth", &renderSettings.maxReflectionDepth);

            ImGui::Separator();
            if (ImGui::Button("Render")) {
                renderScene();
            }
        }
        ImGui::End();

        if (ImGui::Begin("Render Statistics")) {
            ImGui::LabelText("Time", "%lli ms", gRenderInfos[0].renderTime);
            ImGui::LabelText("Objects", "%lu", shapes.size());
            ImGui::LabelText("Primary rays", "%i", gRenderInfos[0].primaryRays);
            ImGui::LabelText("Shadow rays", "%i", gRenderInfos[0].shadowRays);
            ImGui::LabelText("Reflection rays", "%i", gRenderInfos[0].reflectionRays);
        }
        ImGui::End();

        // Draw result
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glErrorCheck();

        // Render imgui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwWaitEvents(); // Wait for new input before rendering
    }

    // Shutdown imgui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Cleanup resources
    for (auto shape : shapes) {
        delete shape;
    }
    glfwTerminate();

    return 0;
}