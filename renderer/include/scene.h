#pragma once

#include "mesh.h"
#include "shader.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace SpatialRender {

struct SceneObject {
  std::shared_ptr<Mesh> mesh;
  std::shared_ptr<Shader> shader;
  glm::mat4 transform;
  glm::vec3 color;

  SceneObject() : transform(1.0f), color(1.0f, 1.0f, 1.0f) {}
};

class Scene {
public:
  Scene();
  ~Scene();

  void AddObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader,
                 const glm::mat4 &transform = glm::mat4(1.0f),
                 const glm::vec3 &color = glm::vec3(1.0f));

  void Clear();

  const std::vector<SceneObject> &GetObjects() const { return m_objects; }
  size_t GetObjectCount() const { return m_objects.size(); }

private:
  std::vector<SceneObject> m_objects;
};

} // namespace SpatialRender
