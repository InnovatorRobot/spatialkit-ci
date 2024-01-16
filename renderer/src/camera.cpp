#include "camera.h"

namespace SpatialRender {

Camera::Camera()
    : m_position(0.0f, 0.0f, 3.0f), m_target(0.0f, 0.0f, 0.0f),
      m_up(0.0f, 1.0f, 0.0f), m_fov(45.0f), m_aspect(16.0f / 9.0f),
      m_near(0.1f), m_far(100.0f), m_orthographic(false) {}

Camera::Camera(const glm::vec3 &position, const glm::vec3 &target,
               const glm::vec3 &up)
    : m_position(position), m_target(target), m_up(up), m_fov(45.0f),
      m_aspect(16.0f / 9.0f), m_near(0.1f), m_far(100.0f),
      m_orthographic(false) {}

void Camera::SetPosition(const glm::vec3 &position) { m_position = position; }

void Camera::SetTarget(const glm::vec3 &target) { m_target = target; }

void Camera::SetUp(const glm::vec3 &up) { m_up = up; }

void Camera::SetPerspective(float fov, float aspect, float near, float far) {
  m_fov = fov;
  m_aspect = aspect;
  m_near = near;
  m_far = far;
  m_orthographic = false;
}

void Camera::SetOrthographic(float left, float right, float bottom, float top,
                             float near, float far) {
  m_orthoLeft = left;
  m_orthoRight = right;
  m_orthoBottom = bottom;
  m_orthoTop = top;
  m_near = near;
  m_far = far;
  m_orthographic = true;
}

glm::mat4 Camera::GetViewMatrix() const {
  return glm::lookAt(m_position, m_target, m_up);
}

glm::mat4 Camera::GetProjectionMatrix() const {
  if (m_orthographic) {
    return glm::ortho(m_orthoLeft, m_orthoRight, m_orthoBottom, m_orthoTop,
                      m_near, m_far);
  } else {
    return glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
  }
}

glm::mat4 Camera::GetViewProjectionMatrix() const {
  return GetProjectionMatrix() * GetViewMatrix();
}

} // namespace SpatialRender
