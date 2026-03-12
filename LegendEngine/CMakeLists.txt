cmake_minimum_required (VERSION 3.10)
project ("LegendEngine")

add_subdirectory(Vendor)
add_subdirectory(Assets)

if (LGENG_BUILD_TESTS)
	add_subdirectory(Tests)
endif()

set(CMAKE_CXX_STANDARD 26)

file(GLOB_RECURSE LGENG_SOURCES "Source/*.cpp" "Headers/*.hpp")
list(FILTER LGENG_SOURCES EXCLUDE REGEX "Vulkan.")

add_library(${PROJECT_NAME} STATIC ${LGENG_SOURCES})
add_dependencies(${PROJECT_NAME} tether tetherRendering compileLegendEngineAssets)

if (MSVC)
	target_compile_options(${PROJECT_NAME} 
        PRIVATE
            "/wd4251;" # Disable the dll-interface warning thingy
    )
endif()

set(VK_LIBS)
set(VK_INCLUDES)

if (LGENG_HEADLESS)
	target_compile_definitions(${PROJECT_NAME} PUBLIC -DLGENG_HEADLESS)
else()
	target_sources(${PROJECT_NAME} PRIVATE "Source/Graphics/WindowRenderTarget.cpp")
endif ()

if (LGENG_VULKAN_API)
	find_package(Vulkan REQUIRED)
	if (NOT Vulkan_FOUND)
		message(FATAL_ERROR "Vulkan SDK not found. Install it from https://vulkan.lunarg.com/, or use the vulkan-sdk apt package")
	endif()

	add_definitions(-D_VULKAN_API)
	set(TETHER_RENDERING_VULKAN_API ON)

	file(GLOB_RECURSE LGENG_VK_SOURCES "Source/Graphics/Vulkan/*.cpp" "Headers/LegendEngine/Graphics/Vulkan/*.hpp")
	target_sources(${PROJECT_NAME} PRIVATE ${LGENG_VK_SOURCES})

	set(VK_LIBS ${VK_LIBS} ${Vulkan_LIBRARIES})
	set(VK_INCLUDES ${VK_INCLUDES} ${Vulkan_INCLUDE_DIR})
endif()

target_include_directories(${PROJECT_NAME} 
PUBLIC
	"Headers"
PRIVATE
	${CMAKE_CURRENT_BINARY_DIR} # For including assets
	${VK_INCLUDES}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
	tether
	tetherRendering
	${VK_LIBS}
	stdc++exp
)
