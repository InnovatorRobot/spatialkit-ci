#include <glm/gtc/matrix_transform.hpp>
#include <gtest/gtest.h>

#include "camera.h"

using namespace SpatialRender;

TEST(CameraTest, DefaultCamera)
{
    Camera camera;
    glm::vec3 pos = camera.GetPosition();
    EXPECT_FLOAT_EQ(pos.z, 3.0f);
}

TEST(CameraTest, CameraPosition)
{
    Camera camera;
    camera.SetPosition(glm::vec3(1.0f, 2.0f, 3.0f));
    glm::vec3 pos = camera.GetPosition();
    EXPECT_FLOAT_EQ(pos.x, 1.0f);
    EXPECT_FLOAT_EQ(pos.y, 2.0f);
    EXPECT_FLOAT_EQ(pos.z, 3.0f);
}

TEST(CameraTest, PerspectiveProjection)
{
    Camera camera;
    camera.SetPerspective(60.0f, 16.0f / 9.0f, 0.1f, 100.0f);
    glm::mat4 proj = camera.GetProjectionMatrix();
    EXPECT_NE(proj[0][0], 0.0f);
}

TEST(CameraTest, OrthographicProjection)
{
    Camera camera;
    camera.SetOrthographic(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
    glm::mat4 proj = camera.GetProjectionMatrix();
    EXPECT_NE(proj[0][0], 0.0f);
}
