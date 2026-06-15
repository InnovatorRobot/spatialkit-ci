#ifndef SPATIAL_RENDER_SCENE_H_
#define SPATIAL_RENDER_SCENE_H_

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "mesh.h"
#include "shader.h"

namespace spatial_render
{

// NOLINTBEGIN(misc-non-private-member-variables-in-classes)
struct SceneObject
{
    std::shared_ptr<Mesh> mesh{};
    std::shared_ptr<Shader> shader{};
    glm::mat4 transform{1.0F};
    glm::vec3 color{1.0F, 1.0F, 1.0F};

    SceneObject() = default;
};
// NOLINTEND(misc-non-private-member-variables-in-classes)

class Scene
{
public:
    Scene();
    ~Scene();

    Scene(Scene const&)            = delete;
    Scene& operator=(Scene const&) = delete;
    Scene(Scene&&)                 = delete;
    Scene& operator=(Scene&&)      = delete;

    void addObject(std::shared_ptr<Mesh> mesh,
                   std::shared_ptr<Shader> shader,
                   glm::mat4 const& transform = glm::mat4(1.0F),
                   glm::vec3 const& color     = glm::vec3(1.0F));

    void clear();

    std::vector<SceneObject> const& getObjects() const { return m_objects_; }
    size_t getObjectCount() const { return m_objects_.size(); }

private:
    std::vector<SceneObject> m_objects_;
};

}  // namespace spatial_render

#endif  // SPATIAL_RENDER_SCENE_H_
