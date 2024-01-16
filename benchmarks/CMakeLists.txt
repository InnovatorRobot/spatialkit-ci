add_executable(spatialrender_benchmark
    benchmark.cpp
    performance_harness.cpp
)

target_include_directories(spatialrender_benchmark PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(spatialrender_benchmark PRIVATE
    spatialrender_lib
    nlohmann_json::nlohmann_json
)
