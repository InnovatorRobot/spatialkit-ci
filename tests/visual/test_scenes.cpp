#include <memory>

#include "camera.h"
#include "mesh.h"
#include "renderer.h"
#include "scene.h"
#include "shader.h"

namespace spatial_render
{

// Test scene factory functions
std::unique_ptr<Scene> createTestScene1()
{
    auto scene  = std::make_unique<Scene>();
    auto shader = std::make_shared<Shader>();

    if (shader->loadFromFiles("shaders/compiled/basic.vert", "shaders/compiled/basic.frag"))
    {
        auto cube = std::shared_ptr<Mesh>(createCubeMesh());
        scene->addObject(cube, shader, glm::mat4(1.0f), glm::vec3(0.8f, 0.2f, 0.2f));
    }

    return scene;
}

std::unique_ptr<Scene> createTestScene2()
{
    auto scene  = std::make_unique<Scene>();
    auto shader = std::make_shared<Shader>();

    if (shader->loadFromFiles("shaders/compiled/basic.vert", "shaders/compiled/basic.frag"))
    {
        auto sphere = std::shared_ptr<Mesh>(createSphereMesh(32));
        scene->addObject(sphere, shader, glm::mat4(1.0f), glm::vec3(0.2f, 0.8f, 0.2f));
    }

    return scene;
}

}  // namespace spatial_render
