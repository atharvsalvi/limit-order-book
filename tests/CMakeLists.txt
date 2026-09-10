add_executable(orderbook_tests
    orderbook_tests.cpp
    ${CMAKE_SOURCE_DIR}/src/engine/orderbook.cpp
    ${CMAKE_SOURCE_DIR}/src/logger/logger.cpp
    ${CMAKE_SOURCE_DIR}/src/tradelog/tradelog.cpp
)

target_include_directories(orderbook_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(orderbook_tests PRIVATE
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(orderbook_tests)