cmake_minimum_required (VERSION 3.8)
project ("Legendary")

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE SOURCES 
    "Source/*.cpp" "Source/*.c"
    "Headers/*.hpp" "Headers/*.h"
)

set(WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

set(CMAKE_CXX_STANDARD 23)

add_definitions(-DVULKAN_API -DWINDOW_MANAGEMENT)
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
add_dependencies(${PROJECT_NAME} LegendEngine tether tetherRendering)

if (MSVC)
	target_compile_options(${PROJECT_NAME} 
        PRIVATE
            "/wd4251;" # Disable the dll-interface warning thingy
    )
endif()

target_include_directories(${PROJECT_NAME} 
PUBLIC 
    "Headers"
    "../LegendEngine/Headers"
PRIVATE 
    "../LegendEngine/Vendor/Tether/Headers"
    "../LegendEngine/Vendor/Tether.Rendering/Headers"
    "../LegendEngine/Vendor/VMA/include"
    ${Vulkan_INCLUDE_DIR}
)

if (UNIX)
    target_link_libraries(${PROJECT_NAME} 
        ${Vulkan_LIBRARIES} 
        ${CMAKE_DL_LIBS} 
        LegendEngine 
        tether 
        tetherRendering
        pthread 
        X11
    )
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME} 
        ${Vulkan_LIBRARIES}
        LegendEngine
    )
endif()
