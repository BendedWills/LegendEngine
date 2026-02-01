cmake_minimum_required (VERSION 3.8)
project ("Legend")

# Definitions
option(LGENG_VULKAN_API "Build LegendEngine with Vulkan capabilities" ON)
option(LGENG_OPENGL_API "Build LegendEngine with OpenGL capabilities" OFF)
option(LGENG_TETHER_EXTERN "Enable if tether is supplied externally" OFF)
message(STATUS "LGENG_VULKAN_API = ${LGENG_VULKAN_API}")
message(STATUS "LGENG_OPENGL_API = ${LGENG_OPENGL_API}")
message(STATUS "LGENG_TETHER_EXTERN = ${LGENG_TETHER_EXTERN}")

# Config
set(TETHER_VMA_DIR "")

if (LGENG_VULKAN_API)
	find_package(Vulkan)
	if (NOT Vulkan_FOUND)
		message(FATAL_ERROR "LGENG_VULKAN_API is enabled, but the Vulkan SDK was not found. \nInstall it from https://vulkan.lunarg.com/, or use the vulkan-sdk apt package\nMissing: Vulkan_LIBRARY and Vulkan_INCLUDE_DIR")
	endif()
endif()

add_subdirectory(Legendary)
add_subdirectory(LegendEngine)