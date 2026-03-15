add_executable(chord_tests
    math/test_phase.cpp
    math/test_zero_cross.cpp
    math/test_stats.cpp
    math/test_correlation.cpp
    demod/test_fm.cpp
    dsp/test_mix.cpp
    dsp/test_nco.cpp
    dsp/test_pulse_design.cpp
    dsp/test_spectrum.cpp
)

target_link_libraries(chord_tests
    PRIVATE
        chord::chord
        GTest::gtest_main
)

if(ENABLE_COVERAGE)
    target_link_libraries(chord_tests PRIVATE chord_coverage_flags)
endif()

target_compile_definitions(chord_tests
    PRIVATE
        TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
)

include(GoogleTest)
gtest_discover_tests(chord_tests)
