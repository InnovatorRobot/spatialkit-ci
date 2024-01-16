#include "shader.h"
#include <gtest/gtest.h>

using namespace SpatialRender;

TEST(ShaderTest, ShaderCreation) {
  Shader shader;
  EXPECT_FALSE(shader.IsValid());
}

TEST(ShaderTest, ShaderUniforms) {
  Shader shader;
  // Test that setting uniforms on invalid shader doesn't crash
  shader.SetUniform("test", 1.0f);
  shader.SetUniform("test", 1);
  shader.SetUniform("test", glm::vec3(1.0f));
}
