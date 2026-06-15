#include <filesystem>
#include <fstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gtest/gtest.h>

#include "camera.h"
#include "mesh.h"
#include "renderer.h"
#include "scene.h"
#include "shader.h"

using namespace spatial_render;
namespace fs = std::filesystem;

class VisualRegressionTest : public ::testing::Test
{
 protected:
    void SetUp() override
    {
        if (!glfwInit())
        {
            GTEST_SKIP() << "GLFW initialization failed";
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);  // Headless

        window = glfwCreateWindow(800, 600, "Test", nullptr, nullptr);
        if (!window)
        {
            GTEST_SKIP() << "Window creation failed";
        }

        glfwMakeContextCurrent(window);

        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK)
        {
            GTEST_SKIP() << "GLEW initialization failed";
        }

        renderer = std::make_unique<Renderer>(800, 600);
        renderer->initialize();

        // Create output directory
        fs::create_directories("tests/visual/output");
        fs::create_directories("tests/visual/golden");
    }

    void TearDown() override
    {
        if (window)
        {
            glfwDestroyWindow(window);
        }
        glfwTerminate();
    }

    GLFWwindow* window = nullptr;
    std::unique_ptr<Renderer> renderer;
};

TEST_F(VisualRegressionTest, RenderCubeScene)
{
    // Load shader
    auto shader = std::make_shared<Shader>();
    ASSERT_TRUE(
        shader->loadFromFiles("shaders/compiled/basic.vert", "shaders/compiled/basic.frag"));

    // Create scene
    Scene scene;
    auto cube = std::shared_ptr<Mesh>(createCubeMesh());
    scene.addObject(cube, shader, glm::mat4(1.0f), glm::vec3(0.8f, 0.2f, 0.2f));

    // Setup camera
    Camera camera;
    camera.setPerspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    camera.setPosition(glm::vec3(0.0f, 0.0f, 3.0f));

    // Render
    renderer->beginFrame();
    renderer->clear();
    renderer->renderScene(scene, camera);
    renderer->endFrame();

    // Capture framebuffer
    std::string output_path = "tests/visual/output/cube_scene.png";
    ASSERT_TRUE(renderer->saveFramebufferToFile(output_path));

    // Verify file was created
    ASSERT_TRUE(fs::exists(output_path));
}

TEST_F(VisualRegressionTest, RenderSphereScene)
{
    auto shader = std::make_shared<Shader>();
    ASSERT_TRUE(
        shader->loadFromFiles("shaders/compiled/basic.vert", "shaders/compiled/basic.frag"));

    Scene scene;
    auto sphere = std::shared_ptr<Mesh>(createSphereMesh(32));
    scene.addObject(sphere, shader, glm::mat4(1.0f), glm::vec3(0.2f, 0.8f, 0.2f));

    Camera camera;
    camera.setPerspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    camera.setPosition(glm::vec3(0.0f, 0.0f, 3.0f));

    renderer->beginFrame();
    renderer->clear();
    renderer->renderScene(scene, camera);
    renderer->endFrame();

    std::string output_path = "tests/visual/output/sphere_scene.png";
    ASSERT_TRUE(renderer->saveFramebufferToFile(output_path));
    ASSERT_TRUE(fs::exists(output_path));
}
