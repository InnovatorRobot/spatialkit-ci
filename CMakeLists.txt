cmake_minimum_required(VERSION 3.20)
project(SpatialRender VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_BENCHMARKS "Build benchmarks" ON)
option(BUILD_TOOLS "Build developer tools" ON)
option(ENABLE_CCACHE "Enable ccache for faster builds" ON)

# Use Ninja if available
find_program(NINJA_EXE ninja)
if(NINJA_EXE)
    set(CMAKE_GENERATOR "Ninja" CACHE STRING "CMake generator")
endif()

# Enable ccache if available
if(ENABLE_CCACHE)
    find_program(CCACHE_EXE ccache)
    if(CCACHE_EXE)
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_EXE})
        message(STATUS "ccache enabled: ${CCACHE_EXE}")
    endif()
endif()

# Find dependencies
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Use FetchContent for dependencies that may not have CMake config files
include(FetchContent)

# GLFW - try to find system package first, fallback to FetchContent
find_package(glfw3 QUIET)
if(NOT glfw3_FOUND)
    message(STATUS "GLFW not found, fetching from source...")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.8
    )
    FetchContent_MakeAvailable(glfw)
    set(GLFW_TARGET glfw)
    set(GLFW_INCLUDE_DIRS ${glfw_SOURCE_DIR}/include)
else()
    message(STATUS "Using system GLFW")
    set(GLFW_TARGET glfw)
endif()

# GLM (header-only)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.8
)
FetchContent_MakeAvailable(glm)

# stb_image (header-only)
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
FetchContent_MakeAvailable(stb)

# GoogleTest
if(BUILD_TESTS)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    FetchContent_MakeAvailable(googletest)
    enable_testing()
endif()

# nlohmann/json for benchmarks
if(BUILD_BENCHMARKS)
    FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
    )
    FetchContent_MakeAvailable(json)
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/include
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/compiled
)

# Renderer library
add_library(spatialrender_lib STATIC
    renderer/src/renderer.cpp
    renderer/src/shader.cpp
    renderer/src/mesh.cpp
    renderer/src/camera.cpp
    renderer/src/scene.cpp
)

target_include_directories(spatialrender_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/include
)

target_link_libraries(spatialrender_lib PUBLIC
    OpenGL::GL
    ${GLFW_TARGET}
    GLEW::GLEW
    glm::glm
)

# GLFW includes (if using FetchContent)
if(glfw_SOURCE_DIR)
    target_include_directories(spatialrender_lib PUBLIC
        ${glfw_SOURCE_DIR}/include
    )
endif()

# Add stb_image include
target_include_directories(spatialrender_lib PRIVATE
    ${stb_SOURCE_DIR}
)

# Main executable
add_executable(spatialrender
    renderer/src/main.cpp
)

target_link_libraries(spatialrender
    spatialrender_lib
)

# Copy shaders to build directory for runtime access
file(COPY ${CMAKE_SOURCE_DIR}/shaders/compiled/
     DESTINATION ${CMAKE_BINARY_DIR}/shaders/compiled
     FILES_MATCHING PATTERN "*.vert" PATTERN "*.frag")

# Tests
if(BUILD_TESTS)
    add_subdirectory(tests/unit)
    add_subdirectory(tests/visual)
endif()

# Benchmarks
if(BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

# Tools (Python scripts - no CMake build needed)
# Tools are installed via bootstrap script

# Installation
install(TARGETS spatialrender DESTINATION bin)
install(DIRECTORY renderer/include/ DESTINATION include/spatialrender)
