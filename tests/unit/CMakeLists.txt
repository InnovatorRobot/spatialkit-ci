add_executable(spatialrender_tests
    test_renderer.cpp
    test_shader.cpp
    test_mesh.cpp
    test_camera.cpp
)

target_link_libraries(spatialrender_tests
    spatialrender_lib
    GTest::gtest
    GTest::gtest_main
)

target_include_directories(spatialrender_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(NAME UnitTests COMMAND spatialrender_tests)
