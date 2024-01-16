#include "mesh.h"
#include <algorithm>
#include <cmath>
#include <cstddef>

namespace SpatialRender {

Mesh::Mesh() : m_VAO(0), m_VBO(0), m_EBO(0), m_uploaded(false) {}

Mesh::~Mesh() { Cleanup(); }

void Mesh::SetVertices(const std::vector<Vertex> &vertices) {
  m_vertices = vertices;
  m_uploaded = false;
}

void Mesh::SetIndices(const std::vector<unsigned int> &indices) {
  m_indices = indices;
  m_uploaded = false;
}

void Mesh::Upload() {
  if (m_uploaded)
    return;

  glGenVertexArrays(1, &m_VAO);
  glGenBuffers(1, &m_VBO);

  glBindVertexArray(m_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex),
               m_vertices.data(), GL_STATIC_DRAW);

  // Position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, position));

  // Normal
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, normal));

  // TexCoord
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, texCoord));

  if (!m_indices.empty()) {
    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 m_indices.size() * sizeof(unsigned int), m_indices.data(),
                 GL_STATIC_DRAW);
  }

  glBindVertexArray(0);
  m_uploaded = true;
}

void Mesh::Render() {
  if (!m_uploaded) {
    Upload();
  }

  glBindVertexArray(m_VAO);

  if (!m_indices.empty()) {
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
  } else {
    glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
  }

  glBindVertexArray(0);
}

void Mesh::Cleanup() {
  if (m_VAO != 0) {
    glDeleteVertexArrays(1, &m_VAO);
    m_VAO = 0;
  }
  if (m_VBO != 0) {
    glDeleteBuffers(1, &m_VBO);
    m_VBO = 0;
  }
  if (m_EBO != 0) {
    glDeleteBuffers(1, &m_EBO);
    m_EBO = 0;
  }
  m_uploaded = false;
}

Mesh *CreateCubeMesh() {
  Mesh *mesh = new Mesh();

  std::vector<Vertex> vertices = {
      // Front face
      {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
      {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
      {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
      {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
      // Back face
      {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
      {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
      {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
      {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
  };

  std::vector<unsigned int> indices = {
      0, 1, 2, 2, 3, 0, // Front
      4, 5, 6, 6, 7, 4, // Back
      0, 3, 7, 7, 4, 0, // Left
      1, 2, 6, 6, 5, 1, // Right
      0, 1, 5, 5, 4, 0, // Bottom
      3, 2, 6, 6, 7, 3  // Top
  };

  mesh->SetVertices(vertices);
  mesh->SetIndices(indices);
  return mesh;
}

Mesh *CreateSphereMesh(int segments) {
  Mesh *mesh = new Mesh();
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  for (int y = 0; y <= segments; ++y) {
    for (int x = 0; x <= segments; ++x) {
      float xSegment = (float)x / (float)segments;
      float ySegment = (float)y / (float)segments;
      float xPos = std::cos(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);
      float yPos = std::cos(ySegment * M_PI);
      float zPos = std::sin(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);

      Vertex v;
      v.position = glm::vec3(xPos, yPos, zPos) * 0.5f;
      v.normal = glm::normalize(glm::vec3(xPos, yPos, zPos));
      v.texCoord = glm::vec2(xSegment, ySegment);
      vertices.push_back(v);
    }
  }

  for (int y = 0; y < segments; ++y) {
    for (int x = 0; x < segments; ++x) {
      int first = y * (segments + 1) + x;
      int second = first + segments + 1;

      indices.push_back(first);
      indices.push_back(second);
      indices.push_back(first + 1);

      indices.push_back(second);
      indices.push_back(second + 1);
      indices.push_back(first + 1);
    }
  }

  mesh->SetVertices(vertices);
  mesh->SetIndices(indices);
  return mesh;
}

Mesh *CreatePlaneMesh(float width, float height) {
  Mesh *mesh = new Mesh();

  float w = width * 0.5f;
  float h = height * 0.5f;

  std::vector<Vertex> vertices = {
      {{-w, 0.0f, -h}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
      {{w, 0.0f, -h}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
      {{w, 0.0f, h}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
      {{-w, 0.0f, h}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}};

  std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};

  mesh->SetVertices(vertices);
  mesh->SetIndices(indices);
  return mesh;
}

} // namespace SpatialRender
