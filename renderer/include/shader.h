#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

namespace SpatialRender {

class Shader {
public:
  Shader();
  ~Shader();

  bool LoadFromFiles(const std::string &vertexPath,
                     const std::string &fragmentPath);
  bool LoadFromSource(const std::string &vertexSource,
                      const std::string &fragmentSource);

  void Use();
  void Unuse();

  void SetUniform(const std::string &name, float value);
  void SetUniform(const std::string &name, int value);
  void SetUniform(const std::string &name, const glm::vec3 &value);
  void SetUniform(const std::string &name, const glm::vec4 &value);
  void SetUniform(const std::string &name, const glm::mat4 &value);

  GLuint GetProgram() const { return m_program; }
  bool IsValid() const { return m_program != 0; }

private:
  GLuint CompileShader(GLenum type, const std::string &source);
  bool LinkProgram(GLuint vertex, GLuint fragment);
  std::string ReadFile(const std::string &path);
  void CheckCompileErrors(GLuint shader, const std::string &type);

  GLuint m_program;
  bool m_linked;
};

} // namespace SpatialRender
