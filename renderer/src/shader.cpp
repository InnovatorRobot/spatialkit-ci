#include "shader.h"

#include <array>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

namespace spatial_render
{

namespace
{
constexpr std::size_t kInfoLogSize = 1024U;
}  // namespace

Shader::Shader() = default;

Shader::~Shader()
{
    if (m_program_ != 0)
    {
        glDeleteProgram(m_program_);
    }
}

std::string Shader::readFile(std::string const& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << path << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint Shader::compileShader(GLenum type, std::string const& source)
{
    GLuint const shader   = glCreateShader(type);
    char const* const src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    checkCompileErrors(shader, (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT");

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == 0)
    {
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

bool Shader::linkProgram(GLuint vertex, GLuint fragment)
{
    m_program_ = glCreateProgram();
    glAttachShader(m_program_, vertex);
    glAttachShader(m_program_, fragment);
    glLinkProgram(m_program_);

    GLint success = 0;
    glGetProgramiv(m_program_, GL_LINK_STATUS, &success);
    if (success == 0)
    {
        std::array<char, kInfoLogSize> info_log{};
        glGetProgramInfoLog(
            m_program_, static_cast<GLsizei>(kInfoLogSize), nullptr, info_log.data());
        std::cerr << "Shader linking failed: " << info_log.data() << std::endl;
        glDeleteProgram(m_program_);
        m_program_ = 0;
        return false;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    m_linked_ = true;
    return true;
}

void Shader::checkCompileErrors(GLuint shader, std::string const& type)
{
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == 0)
    {
        std::array<char, kInfoLogSize> info_log{};
        glGetShaderInfoLog(shader, static_cast<GLsizei>(kInfoLogSize), nullptr, info_log.data());
        std::cerr << "Shader compilation error (" << type << "): " << info_log.data() << std::endl;
    }
}

bool Shader::loadFromFiles(std::string const& vertex_path, std::string const& fragment_path)
{
    std::string const vertex_source   = readFile(vertex_path);
    std::string const fragment_source = readFile(fragment_path);

    if (vertex_source.empty() || fragment_source.empty())
    {
        return false;
    }

    return loadFromSource(vertex_source, fragment_source);
}

bool Shader::loadFromSource(std::string const& vertex_source, std::string const& fragment_source)
{
    GLuint const vertex =  // NOLINT(cppcoreguidelines-init-variables)
        compileShader(GL_VERTEX_SHADER, vertex_source);
    if (vertex == 0)
    {
        return false;
    }

    GLuint const fragment =  // NOLINT(cppcoreguidelines-init-variables)
        compileShader(GL_FRAGMENT_SHADER, fragment_source);
    if (fragment == 0)
    {
        glDeleteShader(vertex);
        return false;
    }

    return linkProgram(vertex, fragment);
}

void Shader::use() const
{
    if (m_program_ != 0)
    {
        glUseProgram(m_program_);
    }
}

void Shader::unuse()
{
    glUseProgram(0);
}

void Shader::setUniform(std::string const& name, float value) const
{
    if (m_program_ == 0)
    {
        return;
    }
    GLint const location = glGetUniformLocation(m_program_, name.c_str());
    if (location >= 0)
    {
        glUniform1f(location, value);
    }
}

void Shader::setUniform(std::string const& name, int value) const
{
    if (m_program_ == 0)
    {
        return;
    }
    GLint const location = glGetUniformLocation(m_program_, name.c_str());
    if (location >= 0)
    {
        glUniform1i(location, value);
    }
}

void Shader::setUniform(std::string const& name, glm::vec3 const& value) const
{
    if (m_program_ == 0)
    {
        return;
    }
    GLint const location = glGetUniformLocation(m_program_, name.c_str());
    if (location >= 0)
    {
        glUniform3fv(location, 1, glm::value_ptr(value));
    }
}

void Shader::setUniform(std::string const& name, glm::vec4 const& value) const
{
    if (m_program_ == 0)
    {
        return;
    }
    GLint const location = glGetUniformLocation(m_program_, name.c_str());
    if (location >= 0)
    {
        glUniform4fv(location, 1, glm::value_ptr(value));
    }
}

void Shader::setUniform(std::string const& name, glm::mat4 const& value) const
{
    if (m_program_ == 0)
    {
        return;
    }
    GLint const location = glGetUniformLocation(m_program_, name.c_str());
    if (location >= 0)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
}

}  // namespace spatial_render
