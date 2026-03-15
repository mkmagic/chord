add_executable(chord_tests
    math/test_phase.cpp
    math/test_zero_cross.cpp
    math/test_stats.cpp
    math/test_correlation.cpp
    demod/test_fm.cpp
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
    # Only link the gcov runtime to the executable. 
    # Do NOT pass --coverage to target_compile_options for the tests.
    target_link_options(chord_tests PRIVATE --coverage)
endif()

target_compile_definitions(chord_tests
    PRIVATE
        TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
)

include(GoogleTest)
gtest_discover_tests(chord_tests)
