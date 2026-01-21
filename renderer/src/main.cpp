#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "mesh.h"
#include "renderer.h"
#include "scene.h"
#include "shader.h"

using namespace SpatialRender;

int main(int argc, char** argv)
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int const width  = 1280;
    int const height = 720;

    GLFWwindow* window = glfwCreateWindow(width, height, "SpatialRender", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // VSync

    // Initialize renderer
    Renderer renderer(width, height);
    if (!renderer.Initialize())
    {
        std::cerr << "Failed to initialize renderer" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Load shaders
    std::shared_ptr<Shader> shader = std::make_shared<Shader>();
    if (!shader->LoadFromFiles("shaders/compiled/basic.vert", "shaders/compiled/basic.frag"))
    {
        std::cerr << "Failed to load shaders" << std::endl;
        return -1;
    }

    // Create scene
    Scene scene;
    std::shared_ptr<Mesh> cube = std::shared_ptr<Mesh>(CreateCubeMesh());
    scene.AddObject(cube, shader, glm::mat4(1.0f), glm::vec3(0.8f, 0.2f, 0.2f));

    // Setup camera
    Camera camera;
    camera.SetPerspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
    camera.SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    camera.SetTarget(glm::vec3(0.0f, 0.0f, 0.0f));

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        renderer.BeginFrame();
        renderer.Clear(glm::vec4(0.1f, 0.1f, 0.15f, 1.0f));
        renderer.RenderScene(scene, camera);
        renderer.EndFrame();

        glfwSwapBuffers(window);
    }

    // Cleanup
    renderer.Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
