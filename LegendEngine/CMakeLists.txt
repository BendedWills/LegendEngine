cmake_minimum_required (VERSION 3.8)
project ("LegendEngine")

# Definitions
option(LGENG_VULKAN_API "Build LegendEngine with Vulkan capabilities" ON)
option(LGENG_OPENGL_API "Build LegendEngine with OpenGL capabilities" ON)
option(LGENG_THREE_DIM "Build LegendEngine with 3D capabilities" ON)
option(LGENG_TWO_DIM "Build LegendEngine with 2D capabilities" ON)
option(LGENG_TETHER_EXTERN "Enable if tether is supplied externally" ON)
message(STATUS "LGENG_VULKAN_API = ${LGENG_VULKAN_API}")
message(STATUS "LGENG_OPENGL_API = ${LGENG_OPENGL_API}")
message(STATUS "LGENG_THREE_DIM = ${LGENG_THREE_DIM}")
message(STATUS "LGENG_TWO_DIM = ${LGENG_TWO_DIM}")
message(STATUS "LGENG_TETHER_EXTERN = ${LGENG_TETHER_EXTERN}")

# Config
set(CMAKE_CXX_STANDARD 23)
set(TETHER_RENDERING_VULKAN_API ON)
set(TETHER_VMA_DIR "")

# Tests
add_subdirectory(Tests/Math)

if (NOT LGENG_TETHER_EXTERN)
	add_subdirectory(Vendor/Tether)
endif()

# Collect all sources
file(GLOB_RECURSE SOURCES 
	"Vendor/stb_image/*.cpp" 
	"Source/*.cpp" 
	"Source/*.c" 
	"Headers/*.hpp" 
	"Headers/*.h" 
	"Assets/Compiled/*.h" 
	"Vendor/stb_image/*.h" 
)

add_subdirectory(Assets)

add_library(${PROJECT_NAME} STATIC ${SOURCES} "Source/Common/Logger.cpp")
add_dependencies(${PROJECT_NAME} tether tetherRendering compileLegendEngineAssets)

if (MSVC)
	target_compile_options(${PROJECT_NAME} 
        PRIVATE
            "/wd4251;" # Disable the dll-interface warning thingy
    )
endif()

set(VK_LIBS)
set(VK_INCLUDES)

if (LGENG_VULKAN_API)
	find_package(Vulkan REQUIRED)

	add_definitions(-D_VULKAN_API)

	set(VK_LIBS ${VK_LIBS} ${Vulkan_LIBRARIES})
	set(VK_INCLUDES ${VK_INCLUDES} ${Vulkan_INCLUDE_DIR})
endif()

target_include_directories(${PROJECT_NAME} 
PUBLIC
	"Headers"
	"Vendor/Tether/Headers"
	"Vendor/Tether.Rendering/Headers"
	"Vendor/VMA/include"
	"Vendor/stb_image/include"
PRIVATE
	${CMAKE_CURRENT_BINARY_DIR} # For including assets
	${VK_INCLUDES}
)

target_link_libraries(${PROJECT_NAME} 
	tether
	tetherRendering
	${VK_LIBS}
)
