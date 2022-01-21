cmake_minimum_required (VERSION 3.8)
project ("LegendEngine")

add_subdirectory(Tests/Triangle)

file(GLOB_RECURSE SOURCES 
    "Source/*.cpp" 
    "Source/*.c" 
    "Headers/*.hpp" 
    "Headers/*.h" 
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} 
    PUBLIC "Headers"
)
