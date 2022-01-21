cmake_minimum_required (VERSION 3.8)
project ("TriangleTest")

file(GLOB_RECURSE SOURCES 
    "Source/*.cpp" "Source/*.c"
    "Headers/*.hpp" "Source/*.h"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} 
    PUBLIC "Headers"
    PUBLIC "../../Headers"
)

target_link_directories(${PROJECT_NAME}
    PRIVATE "../../Build"
)

target_link_libraries(${PROJECT_NAME} 
    ${CMAKE_DL_LIBS} pthread LegendEngine X11 GL
)
