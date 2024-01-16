#pragma once

#include "renderer.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

namespace SpatialRender {

class Mesh {
public:
  Mesh();
  ~Mesh();

  void SetVertices(const std::vector<Vertex> &vertices);
  void SetIndices(const std::vector<unsigned int> &indices);

  void Upload();
  void Render();
  void Cleanup();

  size_t GetVertexCount() const { return m_vertices.size(); }
  size_t GetIndexCount() const { return m_indices.size(); }

private:
  std::vector<Vertex> m_vertices;
  std::vector<unsigned int> m_indices;

  GLuint m_VAO;
  GLuint m_VBO;
  GLuint m_EBO;

  bool m_uploaded;
};

// Factory functions for common meshes
Mesh *CreateCubeMesh();
Mesh *CreateSphereMesh(int segments = 32);
Mesh *CreatePlaneMesh(float width = 1.0f, float height = 1.0f);

} // namespace SpatialRender
