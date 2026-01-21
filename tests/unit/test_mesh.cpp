#include <gtest/gtest.h>

#include "mesh.h"

using namespace SpatialRender;

TEST(MeshTest, MeshCreation)
{
    Mesh mesh;
    EXPECT_EQ(mesh.GetVertexCount(), 0);
    EXPECT_EQ(mesh.GetIndexCount(), 0);
}

TEST(MeshTest, MeshVertices)
{
    Mesh mesh;
    std::vector<Vertex> vertices = {{{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
                                    {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                                    {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}};

    mesh.SetVertices(vertices);
    EXPECT_EQ(mesh.GetVertexCount(), 3);
}

TEST(MeshTest, FactoryFunctions)
{
    Mesh* cube = CreateCubeMesh();
    EXPECT_NE(cube, nullptr);
    EXPECT_GT(cube->GetVertexCount(), 0);
    delete cube;

    Mesh* sphere = CreateSphereMesh(16);
    EXPECT_NE(sphere, nullptr);
    EXPECT_GT(sphere->GetVertexCount(), 0);
    delete sphere;

    Mesh* plane = CreatePlaneMesh();
    EXPECT_NE(plane, nullptr);
    EXPECT_GT(plane->GetVertexCount(), 0);
    delete plane;
}
