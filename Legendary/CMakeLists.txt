cmake_minimum_required (VERSION 3.8)
project ("Legendary")

file(GLOB_RECURSE SOURCES 
    "Source/*.cpp" "Source/*.c"
    "Headers/*.hpp" "Source/*.h"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} 
    PUBLIC "Headers"
    PUBLIC "../Vendor/Tether/Headers"
)

target_link_directories(${PROJECT_NAME}
    PRIVATE "../Vendor/Tether/Build"
)

target_link_libraries(${PROJECT_NAME} 
    ${CMAKE_DL_LIBS} pthread tether X11 GL
)
