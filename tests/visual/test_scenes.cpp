#include <memory>

#include "camera.h"
#include "mesh.h"
#include "renderer.h"
#include "scene.h"
#include "shader.h"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
namespace
{
constexpr float kCubeColorR   = 0.8F;
constexpr float kCubeColorG   = 0.2F;
constexpr float kCubeColorB   = 0.2F;
constexpr float kSphereColorR = 0.2F;
constexpr float kSphereColorG = 0.8F;
constexpr float kSphereColorB = 0.2F;
}  // namespace
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

namespace spatial_render
{

// NOLINTBEGIN(misc-use-internal-linkage)
std::unique_ptr<Scene> createTestScene1()
{
    auto scene  = std::make_unique<Scene>();
    auto shader = std::make_shared<Shader>();

    if (shader->loadFromFiles("shaders/compiled/basic.vert", "shaders/compiled/basic.frag"))
    {
        auto cube = std::shared_ptr<Mesh>(createCubeMesh());
        scene->addObject(
            cube, shader, glm::mat4(1.0F), glm::vec3(kCubeColorR, kCubeColorG, kCubeColorB));
    }

    return scene;
}

std::unique_ptr<Scene> createTestScene2()
{
    auto scene  = std::make_unique<Scene>();
    auto shader = std::make_shared<Shader>();

    if (shader->loadFromFiles("shaders/compiled/basic.vert", "shaders/compiled/basic.frag"))
    {
        auto sphere = std::shared_ptr<Mesh>(createSphereMesh(kDefaultSphereSegments));
        scene->addObject(sphere,
                         shader,
                         glm::mat4(1.0F),
                         glm::vec3(kSphereColorR, kSphereColorG, kSphereColorB));
    }

    return scene;
}

std::unique_ptr<Scene> createNormalsScene(std::shared_ptr<Mesh> mesh)
{
    auto scene  = std::make_unique<Scene>();
    auto shader = std::make_shared<Shader>();

    if (shader->loadFromFiles("shaders/compiled/normals.vert", "shaders/compiled/normals.frag"))
    {
        // u_color is unused by the normals shader; pass white as a safe default
        scene->addObject(std::move(mesh), shader, glm::mat4(1.0F), glm::vec3(1.0F));
    }

    return scene;
}
// NOLINTEND(misc-use-internal-linkage)

}  // namespace spatial_render
