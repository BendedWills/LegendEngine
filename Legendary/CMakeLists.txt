cmake_minimum_required (VERSION 3.10)
project ("Legendary")

find_package(Vulkan REQUIRED)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SOURCES 
    "Source/*.cpp" "Source/*.c"
    "Headers/*.hpp" "Headers/*.h"
)

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
add_dependencies(${PROJECT_NAME} LegendEngine tether tetherRendering)

set_property(TARGET ${PROJECT_NAME} PROPERTY DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")

if (MSVC)
	target_compile_options(${PROJECT_NAME} 
        PRIVATE
            "/wd4251;" # Disable the dll-interface warning thingy
    )
endif()

target_include_directories(${PROJECT_NAME} 
PUBLIC 
    "Headers"
PRIVATE
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
