#include "mesh.h"

#include <algorithm>
#include <cmath>
#include <cstddef>

namespace spatial_render
{

Mesh::Mesh() = default;

Mesh::~Mesh()
{
    cleanup();
}

void Mesh::setVertices(  // NOLINT(readability-identifier-naming,misc-use-internal-linkage)
    std::vector<Vertex> const& vertices)
{
    m_vertices_ = vertices;
    m_uploaded_ = false;
}

void Mesh::setIndices(  // NOLINT(readability-identifier-naming,misc-use-internal-linkage)
    std::vector<unsigned int> const& indices)
{
    m_indices_  = indices;
    m_uploaded_ = false;
}

void Mesh::upload()
{
    if (m_uploaded_)
    {
        return;
    }

    glGenVertexArrays(1, &m_VAO_);
    glGenBuffers(1, &m_VBO_);

    glBindVertexArray(m_VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO_);
    glBufferData(GL_ARRAY_BUFFER,
                 m_vertices_.size() * sizeof(Vertex),
                 m_vertices_.data(),
                 GL_STATIC_DRAW);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<void*>(  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
            offsetof(Vertex, position)));

    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<void*>(  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
            offsetof(Vertex, normal)));

    // TexCoord
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<void*>(  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
            offsetof(Vertex, tex_coord)));

    if (!m_indices_.empty())
    {
        glGenBuffers(1, &m_EBO_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     m_indices_.size() * sizeof(unsigned int),
                     m_indices_.data(),
                     GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
    m_uploaded_ = true;
}

void Mesh::render()
{
    if (!m_uploaded_)
    {
        upload();
    }

    glBindVertexArray(m_VAO_);

    if (!m_indices_.empty())
    {
        glDrawElements(GL_TRIANGLES,
                       static_cast<GLsizei>(m_indices_.size()),
                       GL_UNSIGNED_INT,
                       nullptr);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertices_.size()));
    }

    glBindVertexArray(0);
}

void Mesh::cleanup()
{
    if (m_VAO_ != 0)
    {
        glDeleteVertexArrays(1, &m_VAO_);
        m_VAO_ = 0;
    }
    if (m_VBO_ != 0)
    {
        glDeleteBuffers(1, &m_VBO_);
        m_VBO_ = 0;
    }
    if (m_EBO_ != 0)
    {
        glDeleteBuffers(1, &m_EBO_);
        m_EBO_ = 0;
    }
    m_uploaded_ = false;
}

Mesh* createCubeMesh()  // NOLINT(cppcoreguidelines-owning-memory)
{
    Mesh* const mesh = new Mesh();  // NOLINT(cppcoreguidelines-owning-memory)

    std::vector<Vertex> vertices = {
        // Front face
        {{-0.5F, -0.5F, 0.5F}, {0.0F, 0.0F, 1.0F}, {0.0F, 0.0F}},
        {{0.5F, -0.5F, 0.5F}, {0.0F, 0.0F, 1.0F}, {1.0F, 0.0F}},
        {{0.5F, 0.5F, 0.5F}, {0.0F, 0.0F, 1.0F}, {1.0F, 1.0F}},
        {{-0.5F, 0.5F, 0.5F}, {0.0F, 0.0F, 1.0F}, {0.0F, 1.0F}},
        // Back face
        {{-0.5F, -0.5F, -0.5F}, {0.0F, 0.0F, -1.0F}, {1.0F, 0.0F}},
        {{0.5F, -0.5F, -0.5F}, {0.0F, 0.0F, -1.0F}, {0.0F, 0.0F}},
        {{0.5F, 0.5F, -0.5F}, {0.0F, 0.0F, -1.0F}, {0.0F, 1.0F}},
        {{-0.5F, 0.5F, -0.5F}, {0.0F, 0.0F, -1.0F}, {1.0F, 1.0F}},
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0,  // Front
        4, 5, 6, 6, 7, 4,  // Back
        0, 3, 7, 7, 4, 0,  // Left
        1, 2, 6, 6, 5, 1,  // Right
        0, 1, 5, 5, 4, 0,  // Bottom
        3, 2, 6, 6, 7, 3   // Top
    };

    mesh->setVertices(vertices);
    mesh->setIndices(indices);
    return mesh;
}

Mesh* createSphereMesh(int segments)  // NOLINT(cppcoreguidelines-owning-memory)
{
    Mesh* const mesh = new Mesh();  // NOLINT(cppcoreguidelines-owning-memory)
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (int y = 0; y <= segments; ++y)
    {
        for (int x = 0; x <= segments; ++x)
        {
            float const x_segment = static_cast<float>(x) / static_cast<float>(segments);
            float const y_segment = static_cast<float>(y) / static_cast<float>(segments);
            // NOLINTBEGIN(cppcoreguidelines-init-variables)
            float const x_pos = std::cos(x_segment * 2.0F * M_PI) * std::sin(y_segment * M_PI);
            float const y_pos = std::cos(y_segment * M_PI);
            float const z_pos = std::sin(x_segment * 2.0F * M_PI) * std::sin(y_segment * M_PI);
            // NOLINTEND(cppcoreguidelines-init-variables)

            Vertex const v{
                glm::vec3(x_pos, y_pos, z_pos) * 0.5F,
                glm::normalize(glm::vec3(x_pos, y_pos, z_pos)),
                glm::vec2(x_segment, y_segment),
            };
            vertices.push_back(v);
        }
    }

    for (int y = 0; y < segments; ++y)
    {
        for (int x = 0; x < segments; ++x)
        {
            int const first  = y * (segments + 1) + x;
            int const second = first + segments + 1;

            indices.push_back(static_cast<unsigned int>(first));
            indices.push_back(static_cast<unsigned int>(second));
            indices.push_back(static_cast<unsigned int>(first + 1));

            indices.push_back(static_cast<unsigned int>(second));
            indices.push_back(static_cast<unsigned int>(second + 1));
            indices.push_back(static_cast<unsigned int>(first + 1));
        }
    }

    mesh->setVertices(vertices);
    mesh->setIndices(indices);
    return mesh;
}

Mesh* createPlaneMesh(float width, float height)  // NOLINT(cppcoreguidelines-owning-memory)
{
    Mesh* const mesh = new Mesh();  // NOLINT(cppcoreguidelines-owning-memory)

    float const w = width * 0.5F;
    float const h = height * 0.5F;

    std::vector<Vertex> vertices = {{{-w, 0.0F, -h}, {0.0F, 1.0F, 0.0F}, {0.0F, 0.0F}},
                                    {{w, 0.0F, -h}, {0.0F, 1.0F, 0.0F}, {1.0F, 0.0F}},
                                    {{w, 0.0F, h}, {0.0F, 1.0F, 0.0F}, {1.0F, 1.0F}},
                                    {{-w, 0.0F, h}, {0.0F, 1.0F, 0.0F}, {0.0F, 1.0F}}};

    std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};

    mesh->setVertices(vertices);
    mesh->setIndices(indices);
    return mesh;
}

}  // namespace spatial_render
