#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "mesh.h"
#include "renderer.h"
#include "scene.h"
#include "shader.h"

namespace
{
constexpr float kCubeColorR = 0.8F;
constexpr float kCubeColorG = 0.2F;
constexpr float kCubeColorB = 0.2F;
constexpr float kCameraFov  = 45.0F;
constexpr float kCameraNear = 0.1F;
constexpr float kCameraFar  = 100.0F;
constexpr float kCameraZ    = 3.0F;
constexpr float kClearR     = 0.1F;
constexpr float kClearG     = 0.1F;
constexpr float kClearB     = 0.15F;
}  // namespace

int main([[maybe_unused]] int argc, [[maybe_unused]] char* const* const argv)
{
    // Initialize GLFW
    if (glfwInit() == 0)
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int const width  = 1280;
    int const height = 720;

    GLFWwindow* const window =  // NOLINT(misc-const-correctness) - GLFW API requires non-const
        glfwCreateWindow(width, height, "SpatialRender", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // VSync

    // Initialize renderer
    spatial_render::Renderer renderer(width, height);
    if (!renderer.initialize())
    {
        std::cerr << "Failed to initialize renderer" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Load shaders
    std::shared_ptr<spatial_render::Shader> const shader =
        std::make_shared<spatial_render::Shader>();
    if (!shader->loadFromFiles("shaders/compiled/basic.vert", "shaders/compiled/basic.frag"))
    {
        std::cerr << "Failed to load shaders" << std::endl;
        return -1;
    }

    // Create scene
    spatial_render::Scene scene;
    std::shared_ptr<spatial_render::Mesh> const cube =
        std::shared_ptr<spatial_render::Mesh>(spatial_render::createCubeMesh());
    scene.addObject(cube,
                    shader,
                    glm::mat4(1.0F),
                    glm::vec3(kCubeColorR, kCubeColorG, kCubeColorB));

    // Setup camera
    spatial_render::Camera camera;
    camera.setPerspective(kCameraFov,
                          static_cast<float>(width) / static_cast<float>(height),
                          kCameraNear,
                          kCameraFar);
    camera.setPosition(glm::vec3(0.0F, 0.0F, kCameraZ));
    camera.setTarget(glm::vec3(0.0F, 0.0F, 0.0F));

    // Main loop
    while (glfwWindowShouldClose(window) == 0)
    {
        glfwPollEvents();

        renderer.beginFrame();
        renderer.clear(glm::vec4(kClearR, kClearG, kClearB, 1.0F));
        renderer.renderScene(scene, camera);
        renderer.endFrame();

        glfwSwapBuffers(window);
    }

    // Cleanup
    renderer.shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
