cmake_minimum_required(VERSION 3.25)

project(stellar
    VERSION 0.1.0
    DESCRIPTION "Stellar Toolkit Extensions — zero-allocation, coroutine-ready C++23 utilities (ste::)"
    LANGUAGES CXX
)

# -------- Options --------
option(STELLAR_BUILD_TESTS       "Build unit tests with GoogleTest"       ON)
option(STELLAR_BUILD_BENCHMARKS  "Build micro-benchmarks with Google Benchmark" ON)
option(STELLAR_BUILD_EXAMPLES    "Build example programs"                 ON)
option(STELLAR_ENABLE_WARNINGS   "Enable strict warnings"                 ON)

# -------- Global settings --------
set(CMAKE_CXX_STANDARD           23)
set(CMAKE_CXX_STANDARD_REQUIRED  ON)
set(CMAKE_CXX_EXTENSIONS         OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

# -------- Library target (INTERFACE / header-only) --------
add_library(stellar INTERFACE)
add_library(stellar::stellar ALIAS stellar)

target_include_directories(stellar INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(stellar INTERFACE cxx_std_23)

if(STELLAR_ENABLE_WARNINGS)
    if(MSVC)
        target_compile_options(stellar INTERFACE /W4 /permissive- /Zc:__cplusplus /Zc:preprocessor)
    else()
        target_compile_options(stellar INTERFACE -Wall -Wextra -Wpedantic -Wshadow -Wconversion)
    endif()
endif()

# -------- Tests --------
if(STELLAR_BUILD_TESTS)
    enable_testing()
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.15.2
        GIT_SHALLOW    TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(tests)
endif()

# -------- Benchmarks --------
if(STELLAR_BUILD_BENCHMARKS)
    set(BENCHMARK_ENABLE_TESTING  OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_INSTALL  OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG        v1.9.0
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(googlebenchmark)

    add_subdirectory(benchmarks)
endif()

# -------- Examples --------
if(STELLAR_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# -------- Install --------
include(GNUInstallDirs)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS stellar EXPORT stellarTargets)
install(EXPORT stellarTargets
    FILE stellarTargets.cmake
    NAMESPACE stellar::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/stellar
)
