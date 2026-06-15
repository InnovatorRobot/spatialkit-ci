#include "camera.h"

namespace spatial_render
{

namespace
{
constexpr float kDefaultFov          = 45.0F;
constexpr float kDefaultAspectWidth  = 16.0F;
constexpr float kDefaultAspectHeight = 9.0F;
constexpr float kDefaultNear         = 0.1F;
constexpr float kDefaultFar          = 100.0F;
constexpr float kDefaultCameraZ      = 3.0F;
}  // namespace

Camera::Camera() :
    m_position_(0.0F, 0.0F, kDefaultCameraZ),
    m_target_(0.0F, 0.0F, 0.0F),
    m_up_(0.0F, 1.0F, 0.0F),
    m_fov_(kDefaultFov),
    m_aspect_(kDefaultAspectWidth / kDefaultAspectHeight),
    m_near_(kDefaultNear),
    m_far_(kDefaultFar),
    m_orthographic_(false),
    m_orthoLeft_(0.0F),
    m_orthoRight_(0.0F),
    m_orthoBottom_(0.0F),
    m_orthoTop_(0.0F)
{}

Camera::Camera(glm::vec3 const& position, glm::vec3 const& target, glm::vec3 const& up) :
    m_position_(position),
    m_target_(target),
    m_up_(up),
    m_fov_(kDefaultFov),
    m_aspect_(kDefaultAspectWidth / kDefaultAspectHeight),
    m_near_(kDefaultNear),
    m_far_(kDefaultFar),
    m_orthographic_(false),
    m_orthoLeft_(0.0F),
    m_orthoRight_(0.0F),
    m_orthoBottom_(0.0F),
    m_orthoTop_(0.0F)
{}

void Camera::setPosition(glm::vec3 const& position)
{
    m_position_ = position;
}

void Camera::setTarget(glm::vec3 const& target)
{
    m_target_ = target;
}

void Camera::setUp(glm::vec3 const& up)
{
    m_up_ = up;
}

void Camera::setPerspective(float fov, float aspect, float near, float far)
{
    m_fov_          = fov;
    m_aspect_       = aspect;
    m_near_         = near;
    m_far_          = far;
    m_orthographic_ = false;
}

void Camera::setOrthographic(float left,
                             float right,
                             float bottom,
                             float top,
                             float near,
                             float far)
{
    m_orthoLeft_    = left;
    m_orthoRight_   = right;
    m_orthoBottom_  = bottom;
    m_orthoTop_     = top;
    m_near_         = near;
    m_far_          = far;
    m_orthographic_ = true;
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(m_position_, m_target_, m_up_);
}

glm::mat4 Camera::getProjectionMatrix() const
{
    if (m_orthographic_)
    {
        return glm::ortho(m_orthoLeft_,
                          m_orthoRight_,
                          m_orthoBottom_,
                          m_orthoTop_,
                          m_near_,
                          m_far_);
    }
    return glm::perspective(glm::radians(m_fov_), m_aspect_, m_near_, m_far_);
}

glm::mat4 Camera::getViewProjectionMatrix() const
{
    return getProjectionMatrix() * getViewMatrix();
}

}  // namespace spatial_render
