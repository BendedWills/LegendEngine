cmake_minimum_required (VERSION 3.8)
project ("Legendary")

find_package(Vulkan REQUIRED)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES 
    "Source/*.cpp" "Source/*.c"
    "Headers/*.hpp" "Headers/*.h"
)

set(WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_definitions(-DVULKAN_API -DOPENGL_API -DWINDOW_MANAGEMENT)
if (WIN32 AND ("${CMAKE_BUILD_TYPE}" MATCHES "Release"))
    add_executable(
        ${PROJECT_NAME} 
        WIN32 
        ${SOURCES} 
    )
else()
    add_executable(
        ${PROJECT_NAME} 
        ${SOURCES} 
    )
endif()
add_dependencies(${PROJECT_NAME} LegendEngine VulkanMemoryAllocator tether)

target_include_directories(${PROJECT_NAME} 
PUBLIC 
    "Headers"
    "../LegendEngine/Headers"
PRIVATE 
    "../LegendEngine/Vendor/Tether/Headers"
    "../LegendEngine/Vendor/VMA/include"
    ${Vulkan_INCLUDE_DIR}
)

if (UNIX)
    target_link_libraries(${PROJECT_NAME} 
        ${Vulkan_LIBRARIES} 
        ${CMAKE_DL_LIBS} 
        LegendEngine 
        tether 
        pthread 
        X11
    )
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME} 
        ${Vulkan_LIBRARIES}
        LegendEngine
    )
endif()
