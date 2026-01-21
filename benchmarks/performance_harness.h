#pragma once

#include <chrono>
#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace SpatialRender
{

struct BenchmarkResult
{
    double avg_fps;
    double avg_frame_time_us;
    double avg_render_time_us;
    double frame_variance;
    int scene_complexity;
    glm::ivec2 resolution;
    std::vector<double> frame_times;
    std::vector<double> render_times;
};

class PerformanceHarness
{
 public:
    PerformanceHarness();
    ~PerformanceHarness();

    void StartBenchmark();
    void RecordFrame(double frame_time_us, double render_time_us);
    void EndBenchmark();

    BenchmarkResult GetResult() const { return m_current_result; }

    void SaveResult(std::string const& directory, BenchmarkResult const& result);
    void SaveSummary(std::string const& path);

 private:
    BenchmarkResult m_current_result;
    std::vector<BenchmarkResult> m_all_results;
    std::chrono::high_resolution_clock::time_point m_benchmark_start;
    bool m_benchmarking;
};

}  // namespace SpatialRender
