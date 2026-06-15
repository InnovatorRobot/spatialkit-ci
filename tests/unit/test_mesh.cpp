#include <gtest/gtest.h>

#include "mesh.h"

using namespace spatial_render;

TEST(MeshTest, MeshCreation)
{
    Mesh mesh;
    EXPECT_EQ(mesh.getVertexCount(), 0);
    EXPECT_EQ(mesh.getIndexCount(), 0);
}

TEST(MeshTest, MeshVertices)
{
    Mesh mesh;
    std::vector<Vertex> vertices = {{{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
                                    {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                                    {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}};

    mesh.setVertices(vertices);
    EXPECT_EQ(mesh.getVertexCount(), 3);
}

TEST(MeshTest, FactoryFunctions)
{
    Mesh* cube = createCubeMesh();
    EXPECT_NE(cube, nullptr);
    EXPECT_GT(cube->getVertexCount(), 0);
    delete cube;

    Mesh* sphere = createSphereMesh(16);
    EXPECT_NE(sphere, nullptr);
    EXPECT_GT(sphere->getVertexCount(), 0);
    delete sphere;

    Mesh* plane = createPlaneMesh();
    EXPECT_NE(plane, nullptr);
    EXPECT_GT(plane->getVertexCount(), 0);
    delete plane;
}
