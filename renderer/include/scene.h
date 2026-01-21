#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "mesh.h"
#include "shader.h"

namespace SpatialRender
{

struct SceneObject
{
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Shader> shader;
    glm::mat4 transform;
    glm::vec3 color;

    SceneObject() : transform(1.0f), color(1.0f, 1.0f, 1.0f) {}
};

class Scene
{
 public:
    Scene();
    ~Scene();

    void AddObject(std::shared_ptr<Mesh> mesh,
                   std::shared_ptr<Shader> shader,
                   glm::mat4 const& transform = glm::mat4(1.0f),
                   glm::vec3 const& color     = glm::vec3(1.0f));

    void Clear();

    std::vector<SceneObject> const& GetObjects() const { return m_objects; }
    size_t GetObjectCount() const { return m_objects.size(); }

 private:
    std::vector<SceneObject> m_objects;
};

}  // namespace SpatialRender
