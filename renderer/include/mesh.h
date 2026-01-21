#pragma once

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "renderer.h"

namespace SpatialRender
{

class Mesh
{
 public:
    Mesh();
    ~Mesh();

    void SetVertices(std::vector<Vertex> const& vertices);
    void SetIndices(std::vector<unsigned int> const& indices);

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
Mesh* CreateCubeMesh();
Mesh* CreateSphereMesh(int segments = 32);
Mesh* CreatePlaneMesh(float width = 1.0f, float height = 1.0f);

}  // namespace SpatialRender
