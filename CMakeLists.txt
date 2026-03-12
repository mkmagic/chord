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

# Include directories
target_include_directories(chord
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Link KFR
target_link_libraries(chord PUBLIC kfr)

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
