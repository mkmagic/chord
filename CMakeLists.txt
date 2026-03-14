cmake_minimum_required(VERSION 3.20)
project(chord VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------------------------
# Compiler Settings
# ------------------------------------------------------------------------------
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-stdlib=libc++)
    add_link_options(-stdlib=libc++)
endif()

# ------------------------------------------------------------------------------
# Options
# ------------------------------------------------------------------------------
option(CHORD_BUILD_TESTS "Build chord tests" ON)
option(ENABLE_COVERAGE "Enable code coverage instrumentation" OFF)

if(ENABLE_COVERAGE)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(FATAL_ERROR "ENABLE_COVERAGE is only supported with GCC and Clang compilers")
    endif()

    # Use an INTERFACE target so flags are scoped to chord and test targets only,
    # and not applied to third-party dependencies fetched via FetchContent.
    add_library(chord_coverage_flags INTERFACE)
    target_compile_options(chord_coverage_flags INTERFACE --coverage -O0 -g)
    target_link_options(chord_coverage_flags INTERFACE --coverage)
endif()

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------
include(FetchContent)

# Disable tests/examples for KFR to keep build times down
set(ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(KFR_ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)
set(KFR_ENABLE_TESTS OFF CACHE BOOL "" FORCE)

# Enable DFT to be used by chord
set(KFR_ENABLE_DFT ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    kfr
    GIT_REPOSITORY https://github.com/kfrlib/kfr.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(kfr)

# ------------------------------------------------------------------------------
# Main Library Target
# ------------------------------------------------------------------------------
add_library(chord)
add_library(chord::chord ALIAS chord)

if(ENABLE_COVERAGE)
    target_link_libraries(chord PRIVATE chord_coverage_flags)
endif()

# Include directories
target_include_directories(chord
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Link KFR
target_link_libraries(chord PUBLIC kfr kfr_dsp kfr_dft)

# ------------------------------------------------------------------------------
# Add Subdirectories
# ------------------------------------------------------------------------------
add_subdirectory(src)

if(CHORD_BUILD_TESTS AND PROJECT_IS_TOP_LEVEL)
    enable_testing()
    
    # Fetch GTest
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
    )
    # Prevent overriding the parent project's compiler/linker settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(tests)
endif()
