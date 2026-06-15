#ifndef SPATIAL_RENDER_RENDERER_H_
#define SPATIAL_RENDER_RENDERER_H_

#include <memory>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace spatial_render
{

class Shader;
class Mesh;
class Camera;
class Scene;

// Forward declarations
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coord;
};

class Renderer
{
 public:
    Renderer(int width, int height);
    ~Renderer();

    Renderer(Renderer const&)            = delete;
    Renderer& operator=(Renderer const&) = delete;
    Renderer(Renderer&&)                 = delete;
    Renderer& operator=(Renderer&&)      = delete;

    bool initialize();
    void shutdown();

    void beginFrame() const;
    void endFrame();
    void clear(glm::vec4 const& color = glm::vec4(0.0F, 0.0F, 0.0F, 1.0F));

    void renderScene(Scene& scene, Camera& camera);

    int getWidth() const { return m_width_; }
    int getHeight() const { return m_height_; }

    // Framebuffer capture for testing
    void captureFramebuffer(std::vector<uint8_t>& pixels);
    bool saveFramebufferToFile(std::string const& path);

 private:
    int m_width_;
    int m_height_;
    bool m_initialized_{false};

    GLuint m_defaultFBO_{0};
};

}  // namespace spatial_render

#endif  // SPATIAL_RENDER_RENDERER_H_
