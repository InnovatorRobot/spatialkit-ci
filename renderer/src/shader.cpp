#include "shader.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace SpatialRender {

Shader::Shader() : m_program(0), m_linked(false) {}

Shader::~Shader() {
  if (m_program != 0) {
    glDeleteProgram(m_program);
  }
}

std::string Shader::ReadFile(const std::string &path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << path << std::endl;
    return "";
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

GLuint Shader::CompileShader(GLenum type, const std::string &source) {
  GLuint shader = glCreateShader(type);
  const char *src = source.c_str();
  glShaderSource(shader, 1, &src, nullptr);
  glCompileShader(shader);

  CheckCompileErrors(shader,
                     (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT");

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

bool Shader::LinkProgram(GLuint vertex, GLuint fragment) {
  m_program = glCreateProgram();
  glAttachShader(m_program, vertex);
  glAttachShader(m_program, fragment);
  glLinkProgram(m_program);

  GLint success;
  glGetProgramiv(m_program, GL_LINK_STATUS, &success);
  if (!success) {
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

void Shader::CheckCompileErrors(GLuint shader, const std::string &type) {
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[1024];
    glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
    std::cerr << "Shader compilation error (" << type << "): " << infoLog
              << std::endl;
  }
}

bool Shader::LoadFromFiles(const std::string &vertexPath,
                           const std::string &fragmentPath) {
  std::string vertexSource = ReadFile(vertexPath);
  std::string fragmentSource = ReadFile(fragmentPath);

  if (vertexSource.empty() || fragmentSource.empty()) {
    return false;
  }

  return LoadFromSource(vertexSource, fragmentSource);
}

bool Shader::LoadFromSource(const std::string &vertexSource,
                            const std::string &fragmentSource) {
  GLuint vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
  if (vertex == 0)
    return false;

  GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
  if (fragment == 0) {
    glDeleteShader(vertex);
    return false;
  }

  return LinkProgram(vertex, fragment);
}

void Shader::Use() {
  if (m_program != 0) {
    glUseProgram(m_program);
  }
}

void Shader::Unuse() { glUseProgram(0); }

void Shader::SetUniform(const std::string &name, float value) {
  GLint location = glGetUniformLocation(m_program, name.c_str());
  if (location >= 0) {
    glUniform1f(location, value);
  }
}

void Shader::SetUniform(const std::string &name, int value) {
  GLint location = glGetUniformLocation(m_program, name.c_str());
  if (location >= 0) {
    glUniform1i(location, value);
  }
}

void Shader::SetUniform(const std::string &name, const glm::vec3 &value) {
  GLint location = glGetUniformLocation(m_program, name.c_str());
  if (location >= 0) {
    glUniform3fv(location, 1, &value[0]);
  }
}

void Shader::SetUniform(const std::string &name, const glm::vec4 &value) {
  GLint location = glGetUniformLocation(m_program, name.c_str());
  if (location >= 0) {
    glUniform4fv(location, 1, &value[0]);
  }
}

void Shader::SetUniform(const std::string &name, const glm::mat4 &value) {
  GLint location = glGetUniformLocation(m_program, name.c_str());
  if (location >= 0) {
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
  }
}

} // namespace SpatialRender
