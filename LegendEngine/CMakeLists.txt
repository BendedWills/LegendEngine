cmake_minimum_required (VERSION 3.8)
project ("LegendEngine")

# Definitions
option(
	LGENG_TETHER_EXTERN
	"Enable if tether is supplied externally" 
	ON
)

# Add compiler definitions here.
# Compiler definitions will also need to be defined in the application using
# Legend engine.
# Valid definitions are:
# VULKAN_API
# THREE_DIM
# TWO_DIM
add_definitions(-DVULKAN_API -DTHREE_DIM -DTWO_DIM)

# Add Tests
add_subdirectory(Tests/Math)

# Add libraries

# Only if VULKAN_API is defined above
get_directory_property(COMPDEFS COMPILE_DEFINITIONS)
if (COMPDEFS MATCHES "^VULKAN_API")
	# VULKAN_API defined

	add_subdirectory("Vendor/VMA")
endif()

if (NOT LGENG_TETHER_EXTERN)
	add_subdirectory(Vendor/Tether)
endif()

set(CMAKE_CXX_STANDARD 17)

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

add_library(${PROJECT_NAME} STATIC ${SOURCES} "Source/Common/Logger.cpp")
add_dependencies(${PROJECT_NAME} tether tetherRendering)

set(VK_LIBS)
set(VK_INCLUDES)

if (COMPDEFS MATCHES "^VULKAN_API")
	add_dependencies(${PROJECT_NAME} VulkanMemoryAllocator)

	set(VK_LIBS ${VK_LIBS} ${Vulkan_LIBRARIES})
	set(VK_INCLUDES ${VK_INCLUDES} ${Vulkan_INCLUDE_DIR})
endif()

target_include_directories(${PROJECT_NAME} 
PUBLIC
	"Headers"
	"${ASSETS_BIN_DIR}"
	"Vendor/Tether/Headers"
	"Vendor/Tether.Rendering/Headers"
	"Vendor/VMA/include"
	"Vendor/stb_image/include"
PRIVATE
	${VK_INCLUDES}
)

target_link_libraries(${PROJECT_NAME} 
	tether
	tetherRendering
	${VK_LIBS}
)
