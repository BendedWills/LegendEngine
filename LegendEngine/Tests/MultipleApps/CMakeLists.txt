cmake_minimum_required (VERSION 3.8)
project ("MultipleApps")

file(GLOB_RECURSE SOURCES 
    "Source/*.cpp" "Source/*.c"
    "Headers/*.hpp" "Headers/*.h"
)

add_definitions(-DVULKAN_API)
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} 
    PUBLIC "Headers"
    PUBLIC "../../Headers"
    PRIVATE "../../Vendor/Tether/Headers"
)

target_link_directories(${PROJECT_NAME}
    PRIVATE "../../Build"
    PRIVATE "../../Vendor/Tether/Build"
)

target_link_libraries(${PROJECT_NAME} 
    ${CMAKE_DL_LIBS} pthread LegendEngine X11 vulkan tether
)
