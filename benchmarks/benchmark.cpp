#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "mesh.h"
#include "performance_harness.h"
#include "renderer.h"
#include "scene.h"
#include "shader.h"

using namespace SpatialRender;

int main(int argc, char** argv)
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);  // Headless benchmarking

    int const width  = 1920;
    int const height = 1080;

    GLFWwindow* window = glfwCreateWindow(width, height, "Benchmark", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);  // Disable VSync for benchmarking

    Renderer renderer(width, height);
    if (!renderer.Initialize())
    {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return -1;
    }

    // Load shader
    auto shader = std::make_shared<Shader>();
    if (!shader->LoadFromFiles("shaders/compiled/basic.vert", "shaders/compiled/basic.frag"))
    {
        std::cerr << "Failed to load shaders" << std::endl;
        return -1;
    }

    // Create performance harness
    PerformanceHarness harness;

    // Benchmark different scene complexities
    std::vector<int> object_counts = {1, 10, 50, 100, 500};

    for (int obj_count : object_counts)
    {
        std::cout << "Benchmarking scene with " << obj_count << " objects..." << std::endl;

        Scene scene;
        for (int i = 0; i < obj_count; ++i)
        {
            auto cube           = std::shared_ptr<Mesh>(CreateCubeMesh());
            glm::mat4 transform = glm::mat4(1.0f);
            transform =
                glm::translate(transform,
                               glm::vec3((i % 10) * 0.5f - 2.5f, (i / 10) * 0.5f - 2.5f, 0.0f));
            scene.AddObject(cube, shader, transform, glm::vec3(0.8f, 0.2f, 0.2f));
        }

        Camera camera;
        camera.SetPerspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
        camera.SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));

        // Warmup
        for (int i = 0; i < 10; ++i)
        {
            renderer.BeginFrame();
            renderer.Clear();
            renderer.RenderScene(scene, camera);
            renderer.EndFrame();
            glfwSwapBuffers(window);
        }

        // Benchmark
        int const frame_count = 100;
        harness.StartBenchmark();

        for (int i = 0; i < frame_count; ++i)
        {
            auto frame_start = std::chrono::high_resolution_clock::now();

            renderer.BeginFrame();
            renderer.Clear();

            auto render_start = std::chrono::high_resolution_clock::now();
            renderer.RenderScene(scene, camera);
            auto render_end = std::chrono::high_resolution_clock::now();

            renderer.EndFrame();
            glfwSwapBuffers(window);

            auto frame_end = std::chrono::high_resolution_clock::now();

            auto frame_time =
                std::chrono::duration_cast<std::chrono::microseconds>(frame_end - frame_start)
                    .count();
            auto render_time =
                std::chrono::duration_cast<std::chrono::microseconds>(render_end - render_start)
                    .count();

            harness.RecordFrame(frame_time, render_time);
        }

        harness.EndBenchmark();

        BenchmarkResult result  = harness.GetResult();
        result.scene_complexity = obj_count;
        result.resolution       = {width, height};

        std::cout << "  FPS: " << result.avg_fps << std::endl;
        std::cout << "  Avg Frame Time: " << result.avg_frame_time_us << " μs" << std::endl;
        std::cout << "  Avg Render Time: " << result.avg_render_time_us << " μs" << std::endl;
        std::cout << "  Frame Variance: " << result.frame_variance << std::endl;
        std::cout << std::endl;

        harness.SaveResult("benchmarks/results", result);
    }

    // Save summary
    harness.SaveSummary("benchmarks/results/benchmark_summary.json");

    renderer.Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
