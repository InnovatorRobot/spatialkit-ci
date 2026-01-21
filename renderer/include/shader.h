#pragma once

#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace SpatialRender
{

class Shader
{
 public:
    Shader();
    ~Shader();

    bool LoadFromFiles(std::string const& vertexPath, std::string const& fragmentPath);
    bool LoadFromSource(std::string const& vertexSource, std::string const& fragmentSource);

    void Use();
    void Unuse();

    void SetUniform(std::string const& name, float value);
    void SetUniform(std::string const& name, int value);
    void SetUniform(std::string const& name, glm::vec3 const& value);
    void SetUniform(std::string const& name, glm::vec4 const& value);
    void SetUniform(std::string const& name, glm::mat4 const& value);

    GLuint GetProgram() const { return m_program; }
    bool IsValid() const { return m_program != 0; }

 private:
    GLuint CompileShader(GLenum type, std::string const& source);
    bool LinkProgram(GLuint vertex, GLuint fragment);
    std::string ReadFile(std::string const& path);
    void CheckCompileErrors(GLuint shader, std::string const& type);

    GLuint m_program;
    bool m_linked;
};

}  // namespace SpatialRender
