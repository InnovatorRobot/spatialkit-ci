#include "scene.h"

namespace SpatialRender
{

Scene::Scene()
{}

Scene::~Scene()
{
    Clear();
}

void Scene::AddObject(std::shared_ptr<Mesh> mesh,
                      std::shared_ptr<Shader> shader,
                      glm::mat4 const& transform,
                      glm::vec3 const& color)
{
    SceneObject obj;
    obj.mesh      = mesh;
    obj.shader    = shader;
    obj.transform = transform;
    obj.color     = color;
    m_objects.push_back(obj);
}

void Scene::Clear()
{
    m_objects.clear();
}

}  // namespace SpatialRender
