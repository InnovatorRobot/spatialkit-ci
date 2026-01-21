#include "shader.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace SpatialRender
{

Shader::Shader() : m_program(0), m_linked(false)
{}

Shader::~Shader()
{
    if (m_program != 0)
    {
        glDeleteProgram(m_program);
    }
}

std::string Shader::ReadFile(std::string const& path)
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

GLuint Shader::CompileShader(GLenum type, std::string const& source)
{
    GLuint shader   = glCreateShader(type);
    char const* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    CheckCompileErrors(shader, (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT");

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

bool Shader::LinkProgram(GLuint vertex, GLuint fragment)
{
    m_program = glCreateProgram();
    glAttachShader(m_program, vertex);
    glAttachShader(m_program, fragment);
    glLinkProgram(m_program);

    GLint success;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        glGetProgramInfoLog(m_program, 1024, nullptr, infoLog);
        std::cerr << "Shader linking failed: " << infoLog << std::endl;
        glDeleteProgram(m_program);
        m_program = 0;
        return false;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    m_linked = true;
    return true;
}

void Shader::CheckCompileErrors(GLuint shader, std::string const& type)
{
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "Shader compilation error (" << type << "): " << infoLog << std::endl;
    }
}

bool Shader::LoadFromFiles(std::string const& vertexPath, std::string const& fragmentPath)
{
    std::string vertexSource   = ReadFile(vertexPath);
    std::string fragmentSource = ReadFile(fragmentPath);

    if (vertexSource.empty() || fragmentSource.empty())
    {
        return false;
    }

    return LoadFromSource(vertexSource, fragmentSource);
}

bool Shader::LoadFromSource(std::string const& vertexSource, std::string const& fragmentSource)
{
    GLuint vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
    if (vertex == 0)
        return false;

    GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (fragment == 0)
    {
        glDeleteShader(vertex);
        return false;
    }

    return LinkProgram(vertex, fragment);
}

void Shader::Use()
{
    if (m_program != 0)
    {
        glUseProgram(m_program);
    }
}

void Shader::Unuse()
{
    glUseProgram(0);
}

void Shader::SetUniform(std::string const& name, float value)
{
    GLint location = glGetUniformLocation(m_program, name.c_str());
    if (location >= 0)
    {
        glUniform1f(location, value);
    }
}

void Shader::SetUniform(std::string const& name, int value)
{
    GLint location = glGetUniformLocation(m_program, name.c_str());
    if (location >= 0)
    {
        glUniform1i(location, value);
    }
}

void Shader::SetUniform(std::string const& name, glm::vec3 const& value)
{
    GLint location = glGetUniformLocation(m_program, name.c_str());
    if (location >= 0)
    {
        glUniform3fv(location, 1, &value[0]);
    }
}

void Shader::SetUniform(std::string const& name, glm::vec4 const& value)
{
    GLint location = glGetUniformLocation(m_program, name.c_str());
    if (location >= 0)
    {
        glUniform4fv(location, 1, &value[0]);
    }
}

void Shader::SetUniform(std::string const& name, glm::mat4 const& value)
{
    GLint location = glGetUniformLocation(m_program, name.c_str());
    if (location >= 0)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
    }
}

}  // namespace SpatialRender
