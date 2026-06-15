#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

#include "camera.h"
#include "mesh.h"
#include "renderer.h"
#include "scene.h"
#include "shader.h"

namespace fs = std::filesystem;

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
namespace
{
constexpr int kWidth          = 800;
constexpr int kHeight         = 600;
constexpr float kFov          = 45.0F;
constexpr float kNear         = 0.1F;
constexpr float kFar          = 100.0F;
constexpr float kCameraZ      = 3.0F;
constexpr float kCubeColorR   = 0.8F;
constexpr float kCubeColorG   = 0.2F;
constexpr float kCubeColorB   = 0.2F;
constexpr float kSphereColorR = 0.2F;
constexpr float kSphereColorG = 0.8F;
constexpr float kSphereColorB = 0.2F;
constexpr float kAspect       = static_cast<float>(kWidth) / static_cast<float>(kHeight);
}  // namespace
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

// NOLINTBEGIN(misc-use-internal-linkage)
class VisualRegressionTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        if (glfwInit() == 0)
        {
            GTEST_SKIP() << "GLFW initialization failed";
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);  // Headless

        window_ = glfwCreateWindow(kWidth, kHeight, "Test", nullptr, nullptr);
        if (window_ == nullptr)
        {
            GTEST_SKIP() << "Window creation failed";
        }

        glfwMakeContextCurrent(window_);

        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK)
        {
            GTEST_SKIP() << "GLEW initialization failed";
        }

        renderer_ = std::make_unique<spatial_render::Renderer>(kWidth, kHeight);
        renderer_->initialize();

        fs::create_directories("tests/visual/output");
        fs::create_directories("tests/visual/golden");
    }

    void TearDown() override
    {
        renderer_.reset();
        if (window_ != nullptr)
        {
            glfwDestroyWindow(window_);
        }
        glfwTerminate();
    }

    // NOLINTBEGIN(misc-non-private-member-variables-in-classes,cppcoreguidelines-non-private-member-variables-in-classes)
    GLFWwindow* window_                                 = nullptr;
    std::unique_ptr<spatial_render::Renderer> renderer_ = nullptr;
    // NOLINTEND(misc-non-private-member-variables-in-classes,cppcoreguidelines-non-private-member-variables-in-classes)
};
// NOLINTEND(misc-use-internal-linkage)

namespace
{
spatial_render::Camera makeDefaultCamera()
{
    spatial_render::Camera camera;
    camera.setPerspective(kFov, kAspect, kNear, kFar);
    camera.setPosition(glm::vec3(0.0F, 0.0F, kCameraZ));
    return camera;
}
}  // namespace

TEST_F(VisualRegressionTest, RenderCubeScene)
{
    auto shader = std::make_shared<spatial_render::Shader>();
    ASSERT_TRUE(
        shader->loadFromFiles("shaders/compiled/basic.vert", "shaders/compiled/basic.frag"));

    spatial_render::Scene scene;
    auto cube = std::shared_ptr<spatial_render::Mesh>(spatial_render::createCubeMesh());
    scene.addObject(
        cube, shader, glm::mat4(1.0F), glm::vec3(kCubeColorR, kCubeColorG, kCubeColorB));

    auto camera = makeDefaultCamera();

    renderer_->beginFrame();
    renderer_->clear();
    renderer_->renderScene(scene, camera);
    renderer_->endFrame();

    std::string const output_path = "tests/visual/output/cube_scene.png";
    ASSERT_TRUE(renderer_->saveFramebufferToFile(output_path));
    ASSERT_TRUE(fs::exists(output_path));
}

TEST_F(VisualRegressionTest, RenderSphereScene)
{
    auto shader = std::make_shared<spatial_render::Shader>();
    ASSERT_TRUE(
        shader->loadFromFiles("shaders/compiled/basic.vert", "shaders/compiled/basic.frag"));

    spatial_render::Scene scene;
    auto sphere = std::shared_ptr<spatial_render::Mesh>(
        spatial_render::createSphereMesh(spatial_render::kDefaultSphereSegments));
    scene.addObject(
        sphere, shader, glm::mat4(1.0F), glm::vec3(kSphereColorR, kSphereColorG, kSphereColorB));

    auto camera = makeDefaultCamera();

    renderer_->beginFrame();
    renderer_->clear();
    renderer_->renderScene(scene, camera);
    renderer_->endFrame();

    std::string const output_path = "tests/visual/output/sphere_scene.png";
    ASSERT_TRUE(renderer_->saveFramebufferToFile(output_path));
    ASSERT_TRUE(fs::exists(output_path));
}

TEST_F(VisualRegressionTest, NormalsShaderCube)
{
    auto shader = std::make_shared<spatial_render::Shader>();
    ASSERT_TRUE(
        shader->loadFromFiles("shaders/compiled/normals.vert", "shaders/compiled/normals.frag"));

    spatial_render::Scene scene;
    auto cube = std::shared_ptr<spatial_render::Mesh>(spatial_render::createCubeMesh());
    scene.addObject(cube, shader, glm::mat4(1.0F), glm::vec3(1.0F));

    auto camera = makeDefaultCamera();

    renderer_->beginFrame();
    renderer_->clear();
    renderer_->renderScene(scene, camera);
    renderer_->endFrame();

    std::string const output_path = "tests/visual/output/normals_cube.png";
    ASSERT_TRUE(renderer_->saveFramebufferToFile(output_path));
    ASSERT_TRUE(fs::exists(output_path));
}

TEST_F(VisualRegressionTest, NormalsShaderSphere)
{
    auto shader = std::make_shared<spatial_render::Shader>();
    ASSERT_TRUE(
        shader->loadFromFiles("shaders/compiled/normals.vert", "shaders/compiled/normals.frag"));

    spatial_render::Scene scene;
    auto sphere = std::shared_ptr<spatial_render::Mesh>(
        spatial_render::createSphereMesh(spatial_render::kDefaultSphereSegments));
    scene.addObject(sphere, shader, glm::mat4(1.0F), glm::vec3(1.0F));

    auto camera = makeDefaultCamera();

    renderer_->beginFrame();
    renderer_->clear();
    renderer_->renderScene(scene, camera);
    renderer_->endFrame();

    std::string const output_path = "tests/visual/output/normals_sphere.png";
    ASSERT_TRUE(renderer_->saveFramebufferToFile(output_path));
    ASSERT_TRUE(fs::exists(output_path));
}
