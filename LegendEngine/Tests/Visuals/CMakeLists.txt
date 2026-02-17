cmake_minimum_required(VERSION 3.10)
project("Visuals")

add_executable(${PROJECT_NAME} Main.cpp)

target_link_libraries(${PROJECT_NAME} LegendEngine)
