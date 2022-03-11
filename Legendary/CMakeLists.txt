cmake_minimum_required (VERSION 3.8)
project ("Legendary")

find_package(Vulkan REQUIRED)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES 
    "Source/*.cpp" "Source/*.c"
    "Headers/*.hpp" "Headers/*.h"
)

add_definitions(-DVULKAN_API -DOPENGL_API -DWINDOW_MANAGEMENT)
add_executable(${PROJECT_NAME} ${SOURCES})
add_dependencies(${PROJECT_NAME} VulkanMemoryAllocator)

if (UNIX)
    target_link_libraries(${PROJECT_NAME} 
        ${Vulkan_LIBRARIES} ${CMAKE_DL_LIBS} pthread LegendEngine X11 
        VulkanMemoryAllocator
    )

    target_include_directories(${PROJECT_NAME} 
        PUBLIC "Headers"
        PUBLIC "../LegendEngine/Headers"
        PRIVATE "../LegendEngine/Vendor/Tether/Headers"
        PRIVATE "../LegendEngine/Vendor/VMA/include"
    )

    target_link_directories(${PROJECT_NAME}
        PRIVATE "../../Build"
        PRIVATE "../LegendEngine/Vendor/Tether/Build"
    )
elseif(WIN32)
    set(VULKAN_SDK_PATH "C:\\VulkanSDK\\1.3.204.1")
    set(ENV{VULKAN_SDK} ${VULKAN_SDK_PATH})
    find_package(Vulkan REQUIRED)
    
    target_link_libraries(${PROJECT_NAME} LegendEngine)

    target_include_directories(${PROJECT_NAME} 
        PUBLIC "Headers"
        PUBLIC "../LegendEngine/Headers"
        PRIVATE "../LegendEngine/Vendor/Tether/Headers"
        PRIVATE "../LegendEngine/Vendor/VMA/include"
        PRIVATE "${VULKAN_SDK_PATH}\\Include"
    )

    target_link_directories(${PROJECT_NAME}
        PRIVATE "../LegendEngine/Build"
        PRIVATE "../LegendEngine/Vendor/Tether/Build"
    )
endif()
