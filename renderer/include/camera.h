#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace SpatialRender {

class Camera {
public:
  Camera();
  Camera(const glm::vec3 &position, const glm::vec3 &target,
         const glm::vec3 &up);

  void SetPosition(const glm::vec3 &position);
  void SetTarget(const glm::vec3 &target);
  void SetUp(const glm::vec3 &up);

  void SetPerspective(float fov, float aspect, float near, float far);
  void SetOrthographic(float left, float right, float bottom, float top,
                       float near, float far);

  glm::mat4 GetViewMatrix() const;
  glm::mat4 GetProjectionMatrix() const;
  glm::mat4 GetViewProjectionMatrix() const;

  glm::vec3 GetPosition() const { return m_position; }
  glm::vec3 GetTarget() const { return m_target; }
  glm::vec3 GetUp() const { return m_up; }

private:
  glm::vec3 m_position;
  glm::vec3 m_target;
  glm::vec3 m_up;

  float m_fov;
  float m_aspect;
  float m_near;
  float m_far;

  bool m_orthographic;
  float m_orthoLeft, m_orthoRight, m_orthoBottom, m_orthoTop;
};

} // namespace SpatialRender
