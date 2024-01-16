#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

namespace SpatialRender {

class Shader;
class Mesh;
class Camera;
class Scene;

// Forward declarations
struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoord;
};

class Renderer {
public:
  Renderer(int width, int height);
  ~Renderer();

  bool Initialize();
  void Shutdown();

  void BeginFrame();
  void EndFrame();
  void Clear(const glm::vec4 &color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

  void RenderScene(Scene &scene, Camera &camera);

  int GetWidth() const { return m_width; }
  int GetHeight() const { return m_height; }

  // Framebuffer capture for testing
  void CaptureFramebuffer(std::vector<uint8_t> &pixels);
  bool SaveFramebufferToFile(const std::string &path);

private:
  int m_width;
  int m_height;
  bool m_initialized;

  GLuint m_defaultFBO;
};

} // namespace SpatialRender
