add_executable(spatialrender_visual_tests
    visual_test.cpp
    test_scenes.cpp
)

target_link_libraries(spatialrender_visual_tests
    spatialrender_lib
    GTest::gtest
    GTest::gtest_main
)

target_include_directories(spatialrender_visual_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Python script for visual regression testing
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/visual_regression.py
    ${CMAKE_BINARY_DIR}/visual_regression.py
    COPYONLY
)
