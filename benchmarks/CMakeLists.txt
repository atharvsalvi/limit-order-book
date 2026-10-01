add_executable(orderbook_bench
    orderbook_benchmark.cpp

    ${CMAKE_SOURCE_DIR}/src/engine/orderbook.cpp
    ${CMAKE_SOURCE_DIR}/src/tradelog/tradelog.cpp
    ${CMAKE_SOURCE_DIR}/src/logger/logger.cpp
    ${CMAKE_SOURCE_DIR}/src/pipeline/simulation.cpp
    ${CMAKE_SOURCE_DIR}/src/pipeline/parser.cpp
)

target_compile_definitions(orderbook_bench PRIVATE
    DATA_PATH="${CMAKE_SOURCE_DIR}/data/simulation_input.csv"
)

target_include_directories(orderbook_bench PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(orderbook_bench PRIVATE
    benchmark::benchmark
)

target_compile_definitions(orderbook_bench PRIVATE
    LOB_BENCHMARK_NO_LOCKS
)