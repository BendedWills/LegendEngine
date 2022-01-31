cmake_minimum_required (VERSION 3.8)
project ("LegendEngine")

add_definitions(-DVULKAN_SUPPORT -DWINDOW_MANAGEMENT -DTHREE_DIM -DTWO_DIM)
add_subdirectory(Tests/Triangle)
add_subdirectory(Vendor/Tether)

file(GLOB_RECURSE SOURCES 
    "Source/*.cpp" 
    "Source/*.c" 
    "Headers/*.hpp" 
    "Headers/*.h" 
)

add_definitions(-DVULKAN_API)
add_library(${PROJECT_NAME} STATIC ${SOURCES})
add_dependencies(${PROJECT_NAME} tether)

target_include_directories(${PROJECT_NAME} 
    PUBLIC "Headers"
    PUBLIC "Vendor/Tether/Headers"
)
