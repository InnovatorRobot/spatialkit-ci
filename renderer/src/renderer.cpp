#include "renderer.h"

#include <fstream>
#include <iostream>

#include "camera.h"
#include "scene.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace spatial_render
{

Renderer::Renderer(int width, int height) : m_width_(width), m_height_(height)
{
}

Renderer::~Renderer()
{
    shutdown();
}

bool Renderer::initialize()
{
    if (m_initialized_)
    {
        return true;
    }

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum const err = glewInit();
    if (err != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        return false;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Enable backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Get default framebuffer
    glGetIntegerv(GL_FRAMEBUFFER_BINDING,
                  reinterpret_cast<GLint*>(  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                      &m_defaultFBO_));

    m_initialized_ = true;
    return true;
}

void Renderer::shutdown()
{
    m_initialized_ = false;
}

void Renderer::beginFrame() const
{
    glViewport(0, 0, m_width_, m_height_);
}

void Renderer::endFrame()
{
    // Frame end logic (if needed)
}

void Renderer::clear(glm::vec4 const& color)
{
    // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
    glClearColor(color.r, color.g, color.b, color.a);
    // NOLINTEND(cppcoreguidelines-pro-type-union-access)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::renderScene(Scene& scene, Camera& camera)
{
    glm::mat4 const view_proj = camera.getViewProjectionMatrix();

    for (auto const& obj : scene.getObjects())
    {
        if (!obj.mesh || !obj.shader)
        {
            continue;
        }

        obj.shader->use();
        obj.shader->setUniform("u_model", obj.transform);
        obj.shader->setUniform("u_viewProj", view_proj);
        obj.shader->setUniform("u_color", obj.color);

        obj.mesh->render();

        obj.shader->unuse();
    }
}

void Renderer::captureFramebuffer(std::vector<uint8_t>& pixels) const
{
    pixels.resize(m_width_ * m_height_ * 4);
    glReadPixels(0, 0, m_width_, m_height_, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    // Flip vertically (OpenGL reads from bottom-left)
    for (int y = 0; y < m_height_ / 2; ++y)
    {
        int const top    = y;
        int const bottom = m_height_ - 1 - y;
        for (int x = 0; x < m_width_ * 4; ++x)
        {
            // NOLINTBEGIN(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
            std::swap(pixels[top * m_width_ * 4 + x], pixels[bottom * m_width_ * 4 + x]);
            // NOLINTEND(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
        }
    }
}

bool Renderer::saveFramebufferToFile(std::string const& path) const
{
    std::vector<uint8_t> pixels;
    captureFramebuffer(pixels);

    return stbi_write_png(path.c_str(), m_width_, m_height_, 4, pixels.data(), m_width_ * 4) != 0;
}

}  // namespace spatial_render
