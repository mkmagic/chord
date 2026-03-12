add_executable(chord_tests
    math/test_phase.cpp
    math/test_zero_cross.cpp
)

target_link_libraries(chord_tests
    PRIVATE
        chord::chord
        GTest::gtest_main
)

target_compile_definitions(chord_tests
    PRIVATE
        TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
)

include(GoogleTest)
gtest_discover_tests(chord_tests)
