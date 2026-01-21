#include <memory>

#include "camera.h"
#include "mesh.h"
#include "renderer.h"
#include "scene.h"
#include "shader.h"

namespace SpatialRender
{

// Test scene factory functions
std::unique_ptr<Scene> CreateTestScene1()
{
    auto scene  = std::make_unique<Scene>();
    auto shader = std::make_shared<Shader>();

    if (shader->LoadFromFiles("shaders/compiled/basic.vert", "shaders/compiled/basic.frag"))
    {
        auto cube = std::shared_ptr<Mesh>(CreateCubeMesh());
        scene->AddObject(cube, shader, glm::mat4(1.0f), glm::vec3(0.8f, 0.2f, 0.2f));
    }

    return scene;
}

std::unique_ptr<Scene> CreateTestScene2()
{
    auto scene  = std::make_unique<Scene>();
    auto shader = std::make_shared<Shader>();

    if (shader->LoadFromFiles("shaders/compiled/basic.vert", "shaders/compiled/basic.frag"))
    {
        auto sphere = std::shared_ptr<Mesh>(CreateSphereMesh(32));
        scene->AddObject(sphere, shader, glm::mat4(1.0f), glm::vec3(0.2f, 0.8f, 0.2f));
    }

    return scene;
}

}  // namespace SpatialRender
