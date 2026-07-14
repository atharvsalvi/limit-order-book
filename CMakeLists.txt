cmake_minimum_required(VERSION 3.14)
project(QuantSandbox LANGUAGES CXX)

# Enforce C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Low-Latency & Strict Compiler Flags
# -O3: Aggressive optimization
# -march=native: Optimizes code specifically for your local CPU architecture
# -Wall -Wextra -Werror: Treats all warnings as errors to enforce clean code
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-O3 -march=native -Wall -Wextra -Werror)
endif()

# Fetch and configure Google Benchmark automatically
include(FetchContent)
FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG        v1.8.3
)

set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE INTERNAL "Disable benchmark gtest")

FetchContent_MakeAvailable(benchmark)

# 1. Main Executable Target
add_executable(quant_run 
    src/main.cpp
    src/orderbook.cpp
    src/tradelog.cpp
    src/pnltracker.cpp
    src/data/parser.cpp
    src/strategy.cpp
)
target_include_directories(quant_run PRIVATE src)
target_compile_definitions(quant_run PRIVATE 
    DATA_PATH="${CMAKE_SOURCE_DIR}/src/data/RELIANCE.csv"
)

# 2. Benchmark Target
add_executable(cache_bench benchmarks/cache_benchmark.cpp)
target_link_libraries(cache_bench PRIVATE benchmark::benchmark)
