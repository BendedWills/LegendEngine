cmake_minimum_required (VERSION 3.10)
project ("Legend")

# Definitions
option(LE_VULKAN_API "Build LegendEngine with Vulkan capabilities" ON)
option(LE_OPENGL_API "Build LegendEngine with OpenGL capabilities" OFF)
option(LE_TETHER_EXTERN "Enable if tether is supplied externally" OFF)
option(LE_BUILD_TESTS "Build tests for LegendEngine" OFF)
option(LE_HEADLESS "Build LegendEngine without window capabilities (headless)" OFF)
message(STATUS "LE_VULKAN_API = ${LE_VULKAN_API}")
message(STATUS "LE_OPENGL_API = ${LE_OPENGL_API}")
message(STATUS "LE_TETHER_EXTERN = ${LE_TETHER_EXTERN}")
message(STATUS "LE_BUILD_TESTS = ${LE_BUILD_TESTS}")
message(STATUS "LE_HEADLESS = ${LE_HEADLESS}")

# Config
set(TETHER_VMA_DIR "")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(TETHER_HEADLESS ${LE_HEADLESS})

if (LE_VULKAN_API)
	find_package(Vulkan)
	if (NOT Vulkan_FOUND)
		message(FATAL_ERROR "LE_VULKAN_API is enabled, but the Vulkan SDK was not found. \nInstall it from https://vulkan.lunarg.com/, or run \"apt install libvulkan-dev vulkan-validationlayers vulkan-tools glslc\"")
	endif()
endif()

add_subdirectory(Legendary)
add_subdirectory(LegendEngine)