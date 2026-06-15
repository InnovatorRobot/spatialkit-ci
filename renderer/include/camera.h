#ifndef SPATIAL_RENDER_CAMERA_H_
#define SPATIAL_RENDER_CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace spatial_render
{

class Camera
{
 public:
    Camera();
    Camera(glm::vec3 const& position, glm::vec3 const& target, glm::vec3 const& up);

    void setPosition(glm::vec3 const& position);
    void setTarget(glm::vec3 const& target);
    void setUp(glm::vec3 const& up);

    void setPerspective(float fov, float aspect, float near, float far);
    void setOrthographic(float left, float right, float bottom, float top, float near, float far);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getViewProjectionMatrix() const;

    glm::vec3 getPosition() const { return m_position_; }
    glm::vec3 getTarget() const { return m_target_; }
    glm::vec3 getUp() const { return m_up_; }

 private:
    glm::vec3 m_position_;
    glm::vec3 m_target_;
    glm::vec3 m_up_;

    float m_fov_;
    float m_aspect_;
    float m_near_;
    float m_far_;

    bool m_orthographic_;
    float m_orthoLeft_, m_orthoRight_, m_orthoBottom_, m_orthoTop_;
};

}  // namespace spatial_render

#endif  // SPATIAL_RENDER_CAMERA_H_
