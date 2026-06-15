#include <gtest/gtest.h>

#include "camera.h"
#include "mesh.h"
#include "renderer.h"
#include "scene.h"
#include "shader.h"

using namespace spatial_render;

class RendererTest : public ::testing::Test
{
 protected:
    void SetUp() override
    {
        // Note: These tests require OpenGL context
        // In a real scenario, you'd use a headless OpenGL context
    }
};

TEST_F(RendererTest, RendererInitialization)
{
    Renderer renderer(800, 600);
    // Basic initialization test
    EXPECT_EQ(renderer.getWidth(), 800);
    EXPECT_EQ(renderer.getHeight(), 600);
}

TEST_F(RendererTest, CameraViewMatrix)
{
    Camera camera;
    camera.setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    camera.setTarget(glm::vec3(0.0f, 0.0f, 0.0f));

    glm::mat4 view = camera.getViewMatrix();
    EXPECT_NE(view[0][0], 0.0f);  // Should be a valid matrix
}

TEST_F(RendererTest, CameraProjectionMatrix)
{
    Camera camera;
    camera.setPerspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);

    glm::mat4 proj = camera.getProjectionMatrix();
    EXPECT_NE(proj[0][0], 0.0f);
}
