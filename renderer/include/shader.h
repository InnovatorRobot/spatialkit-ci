#ifndef SPATIAL_RENDER_SHADER_H_
#define SPATIAL_RENDER_SHADER_H_

#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace spatial_render
{

class Shader
{
 public:
    Shader();
    ~Shader();

    Shader(Shader const&)            = delete;
    Shader& operator=(Shader const&) = delete;
    Shader(Shader&&)                 = delete;
    Shader& operator=(Shader&&)      = delete;

    bool loadFromFiles(std::string const& vertex_path, std::string const& fragment_path);
    bool loadFromSource(std::string const& vertex_source, std::string const& fragment_source);

    void use() const;
    void unuse();

    void setUniform(std::string const& name, float value) const;
    void setUniform(std::string const& name, int value) const;
    void setUniform(std::string const& name, glm::vec3 const& value) const;
    void setUniform(std::string const& name, glm::vec4 const& value) const;
    void setUniform(std::string const& name, glm::mat4 const& value) const;

    GLuint getProgram() const { return m_program_; }
    bool isValid() const { return m_program_ != 0; }

 private:
    GLuint compileShader(GLenum type, std::string const& source);
    bool linkProgram(GLuint vertex, GLuint fragment);
    std::string readFile(std::string const& path);
    void checkCompileErrors(GLuint shader, std::string const& type);

    GLuint m_program_{0};
    bool m_linked_{false};
};

}  // namespace spatial_render

#endif  // SPATIAL_RENDER_SHADER_H_
