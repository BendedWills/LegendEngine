cmake_minimum_required (VERSION 3.8)
project ("LegendEngine")

add_definitions(-DVULKAN_API -DWINDOW_MANAGEMENT -DTHREE_DIM -DTWO_DIM)
add_subdirectory(Tests/Triangle)
add_subdirectory(Vendor/Tether)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

file(GLOB_RECURSE SOURCES 
    "Source/*.cpp" 
    "Source/*.c" 
    "Headers/*.hpp" 
    "Headers/*.h" 
    "Assets/Compiled/*.h" 
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
add_dependencies(${PROJECT_NAME} tether)

target_include_directories(${PROJECT_NAME} 
    PUBLIC "Headers"
    PUBLIC "Assets/Compiled" 
    PUBLIC "Vendor/Tether/Headers"
)
