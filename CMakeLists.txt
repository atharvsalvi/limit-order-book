cmake_minimum_required(VERSION 3.14)
project(QuantSandbox LANGUAGES CXX)

# ============================================================
# C++ Standard
# ============================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================
# Compiler Flags
# ============================================================
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -O3
        -march=native
        -Wall
        -Wextra
        -Werror
    )
endif()

# ============================================================
# Fetch Dependencies
# ============================================================
include(FetchContent)

# ---------- Google Benchmark ----------
FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.8.3
)

set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE INTERNAL "Disable benchmark gtest")

# ---------- FTXUI ----------
FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
    GIT_TAG v6.1.9
)

FetchContent_MakeAvailable(
    benchmark
    ftxui
)

# ============================================================
# Main Executable
# ============================================================
add_executable(quant_run
    src/main.cpp
    src/engine/orderbook.cpp
    src/tradelog/tradelog.cpp
    src/pipeline/parser.cpp
    src/ui/tui.cpp
    src/ui/dashboard.cpp
    src/logger/logger.cpp
    src/pipeline/simulation.cpp
)

target_include_directories(quant_run PRIVATE src)

target_compile_definitions(quant_run PRIVATE
    DATA_PATH="${CMAKE_SOURCE_DIR}/src/pipeline/data/simulation_input.csv"
)

# Link FTXUI
target_link_libraries(quant_run PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
)

# ============================================================
# Benchmark Target
# ============================================================
add_executable(cache_bench
    benchmarks/cache_benchmark.cpp
)

target_link_libraries(cache_bench PRIVATE
    benchmark::benchmark
)