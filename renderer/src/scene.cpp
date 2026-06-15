#include "scene.h"

namespace spatial_render
{

Scene::Scene() = default;

Scene::~Scene()
{
    clear();
}

void Scene::addObject(  // NOLINT(readability-identifier-naming,misc-use-internal-linkage)
    std::shared_ptr<Mesh> mesh,
    std::shared_ptr<Shader> shader,
    glm::mat4 const& transform,
    glm::vec3 const& color)
{
    SceneObject obj;
    obj.mesh      = mesh;
    obj.shader    = shader;
    obj.transform = transform;
    obj.color     = color;
    m_objects_.push_back(obj);
}

void Scene::clear()
{
    m_objects_.clear();
}

}  // namespace spatial_render
