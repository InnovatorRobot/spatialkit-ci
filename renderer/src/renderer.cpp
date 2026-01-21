#include "renderer.h"

#include <fstream>
#include <iostream>

#include "camera.h"
#include "scene.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace SpatialRender
{

Renderer::Renderer(int width, int height) :
    m_width(width),
    m_height(height),
    m_initialized(false),
    m_defaultFBO(0)
{}

Renderer::~Renderer()
{
    Shutdown();
}

bool Renderer::Initialize()
{
    if (m_initialized)
    {
        return true;
    }

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err       = glewInit();
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
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&m_defaultFBO);

    m_initialized = true;
    return true;
}

void Renderer::Shutdown()
{
    m_initialized = false;
}

void Renderer::BeginFrame()
{
    glViewport(0, 0, m_width, m_height);
}

void Renderer::EndFrame()
{
    // Frame end logic (if needed)
}

void Renderer::Clear(glm::vec4 const& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::RenderScene(Scene& scene, Camera& camera)
{
    glm::mat4 viewProj = camera.GetViewProjectionMatrix();

    for (auto const& obj : scene.GetObjects())
    {
        if (!obj.mesh || !obj.shader)
            continue;

        obj.shader->Use();
        obj.shader->SetUniform("u_model", obj.transform);
        obj.shader->SetUniform("u_viewProj", viewProj);
        obj.shader->SetUniform("u_color", obj.color);

        obj.mesh->Render();

        obj.shader->Unuse();
    }
}

void Renderer::CaptureFramebuffer(std::vector<uint8_t>& pixels)
{
    pixels.resize(m_width * m_height * 4);
    glReadPixels(0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    // Flip vertically (OpenGL reads from bottom-left)
    for (int y = 0; y < m_height / 2; ++y)
    {
        int top    = y;
        int bottom = m_height - 1 - y;
        for (int x = 0; x < m_width * 4; ++x)
        {
            std::swap(pixels[top * m_width * 4 + x], pixels[bottom * m_width * 4 + x]);
        }
    }
}

bool Renderer::SaveFramebufferToFile(std::string const& path)
{
    std::vector<uint8_t> pixels;
    CaptureFramebuffer(pixels);

    return stbi_write_png(path.c_str(), m_width, m_height, 4, pixels.data(), m_width * 4) != 0;
}

}  // namespace SpatialRender
