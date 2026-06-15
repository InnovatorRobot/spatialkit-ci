#ifndef SPATIAL_RENDER_MESH_H_
#define SPATIAL_RENDER_MESH_H_

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "renderer.h"

namespace spatial_render
{

class Mesh
{
 public:
    Mesh();
    ~Mesh();

    Mesh(Mesh const&)            = delete;
    Mesh& operator=(Mesh const&) = delete;
    Mesh(Mesh&&)                 = delete;
    Mesh& operator=(Mesh&&)      = delete;

    void setVertices(std::vector<Vertex> const& vertices);
    void setIndices(std::vector<unsigned int> const& indices);

    void upload();
    void render();
    void cleanup();

    size_t getVertexCount() const { return m_vertices_.size(); }
    size_t getIndexCount() const { return m_indices_.size(); }

 private:
    std::vector<Vertex> m_vertices_;
    std::vector<unsigned int> m_indices_;

    GLuint m_VAO_{0};
    GLuint m_VBO_{0};
    GLuint m_EBO_{0};

    bool m_uploaded_{false};
};

// Factory functions for common meshes
constexpr int kDefaultSphereSegments = 32;
Mesh* createCubeMesh();
Mesh* createSphereMesh(int segments = kDefaultSphereSegments);
Mesh* createPlaneMesh(float width = 1.0F, float height = 1.0F);

}  // namespace spatial_render

#endif  // SPATIAL_RENDER_MESH_H_
