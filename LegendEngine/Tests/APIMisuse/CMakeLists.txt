cmake_minimum_required (VERSION 3.8)
project ("APIMisuse")

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES 
    "Source/*.cpp" "Source/*.c"
    "Headers/*.hpp" "Headers/*.h"
)

add_definitions(-DVULKAN_API)
add_executable(${PROJECT_NAME} ${SOURCES})

if (UNIX)
    target_link_libraries(${PROJECT_NAME} 
        ${CMAKE_DL_LIBS} pthread LegendEngine X11 vulkan tether
    )

    target_include_directories(${PROJECT_NAME} 
        PUBLIC "Headers"
        PUBLIC "../../Headers"
        PRIVATE "../../Vendor/Tether/Headers"
    )

    target_link_directories(${PROJECT_NAME}
        PRIVATE "../../Build"
        PRIVATE "../../Vendor/Tether/Build"
    )
elseif(WIN32)
    set(VULKAN_SDK_PATH "C:\\VulkanSDK\\1.3.204.1")
    set(ENV{VULKAN_SDK} ${VULKAN_SDK_PATH})
    find_package(Vulkan REQUIRED)

    target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES} tether LegendEngine)

    target_include_directories(${PROJECT_NAME} 
        PUBLIC "Headers"
        PUBLIC "../../Headers"
        PRIVATE "../../Vendor/Tether/Headers"
        PRIVATE "${VULKAN_SDK_PATH}\\Include"
    )

    target_link_directories(${PROJECT_NAME}
        PRIVATE "../../Build"
        PRIVATE "../../Vendor/Tether/Build"
    )
endif()
