#include <gtest/gtest.h>

#include "shader.h"

using namespace spatial_render;

TEST(ShaderTest, ShaderCreation)
{
    Shader shader;
    EXPECT_FALSE(shader.isValid());
}

TEST(ShaderTest, ShaderUniforms)
{
    Shader shader;
    // Test that setting uniforms on invalid shader doesn't crash
    shader.setUniform("test", 1.0f);
    shader.setUniform("test", 1);
    shader.setUniform("test", glm::vec3(1.0f));
}
