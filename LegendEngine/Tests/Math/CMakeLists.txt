cmake_minimum_required(VERSION 3.10)
project("MathTest")

add_executable(${PROJECT_NAME} Main.cpp)

target_link_libraries(${PROJECT_NAME} GTest::gtest_main LegendEngine)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})