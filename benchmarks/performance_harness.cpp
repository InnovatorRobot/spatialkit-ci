#include "performance_harness.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace SpatialRender {

PerformanceHarness::PerformanceHarness() : m_benchmarking(false) {
}

PerformanceHarness::~PerformanceHarness() {
}

void PerformanceHarness::StartBenchmark() {
    m_current_result = BenchmarkResult();
    m_current_result.frame_times.clear();
    m_current_result.render_times.clear();
    m_benchmark_start = std::chrono::high_resolution_clock::now();
    m_benchmarking = true;
}

void PerformanceHarness::RecordFrame(double frame_time_us, double render_time_us) {
    if (!m_benchmarking) return;
    
    m_current_result.frame_times.push_back(frame_time_us);
    m_current_result.render_times.push_back(render_time_us);
}

void PerformanceHarness::EndBenchmark() {
    if (!m_benchmarking || m_current_result.frame_times.empty()) {
        return;
    }
    
    // Calculate statistics
    double total_frame_time = 0.0;
    double total_render_time = 0.0;
    
    for (double ft : m_current_result.frame_times) {
        total_frame_time += ft;
    }
    for (double rt : m_current_result.render_times) {
        total_render_time += rt;
    }
    
    size_t frame_count = m_current_result.frame_times.size();
    m_current_result.avg_frame_time_us = total_frame_time / frame_count;
    m_current_result.avg_render_time_us = total_render_time / frame_count;
    m_current_result.avg_fps = 1000000.0 / m_current_result.avg_frame_time_us;
    
    // Calculate variance
    double variance = 0.0;
    for (double ft : m_current_result.frame_times) {
        double diff = ft - m_current_result.avg_frame_time_us;
        variance += diff * diff;
    }
    m_current_result.frame_variance = variance / frame_count;
    
    m_all_results.push_back(m_current_result);
    m_benchmarking = false;
}

void PerformanceHarness::SaveResult(const std::string& directory, const BenchmarkResult& result) {
    fs::create_directories(directory);
    
    std::string filename = directory + "/benchmark_" + 
                          std::to_string(result.scene_complexity) + "_objects.json";
    
    json j;
    j["avg_fps"] = result.avg_fps;
    j["avg_frame_time_us"] = result.avg_frame_time_us;
    j["avg_render_time_us"] = result.avg_render_time_us;
    j["frame_variance"] = result.frame_variance;
    j["scene_complexity"] = result.scene_complexity;
    j["resolution"] = {
        {"width", result.resolution.x},
        {"height", result.resolution.y}
    };
    j["frame_times"] = result.frame_times;
    j["render_times"] = result.render_times;
    
    std::ofstream file(filename);
    file << std::setw(2) << j << std::endl;
    
    std::cout << "Saved benchmark result: " << filename << std::endl;
}

void PerformanceHarness::SaveSummary(const std::string& path) {
    json summary;
    summary["benchmark_count"] = m_all_results.size();
    
    json results_array = json::array();
    for (const auto& result : m_all_results) {
        json r;
        r["scene_complexity"] = result.scene_complexity;
        r["avg_fps"] = result.avg_fps;
        r["avg_frame_time_us"] = result.avg_frame_time_us;
        r["avg_render_time_us"] = result.avg_render_time_us;
        r["frame_variance"] = result.frame_variance;
        results_array.push_back(r);
    }
    summary["results"] = results_array;
    
    fs::create_directories(fs::path(path).parent_path());
    std::ofstream file(path);
    file << std::setw(2) << summary << std::endl;
    
    std::cout << "Saved benchmark summary: " << path << std::endl;
}

} // namespace SpatialRender
