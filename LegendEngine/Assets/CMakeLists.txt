cmake_minimum_required (VERSION 3.10)

# Relative to the build directory 
# Build/*name*/LegendEngine/Assets
set(APPLICATION_NAME LegendEngine)
set(COMMON_DIR Common)
set(SHADER_BINARY_DIR VulkanShaders)
set(SHADER_HEADER_DIR CompiledShaders)
set(COMPILED_ASSETS_DIR Compiled)

# The absolute path to the compiled assets directory
# Build/*name*/LegendEngine/Assets/${COMPILED_ASSETS_DIR}
set (BUILD_COMPILED_DIR
	"${CMAKE_CURRENT_BINARY_DIR}/${COMPILED_ASSETS_DIR}"
)

# The absolute path to the shader binary directory
# Build/*name*/LegendEngine/Assets/${SHADER_BINARY_DIR}
set (BUILD_SHADER_DIR
	"${CMAKE_CURRENT_BINARY_DIR}/${SHADER_BINARY_DIR}"
)

# The absolute path to the shader headers directory
# Build/*name*/LegendEngine/Assets/${SHADER_HEADER_DIR}
set (BUILD_SHADER_HEADER_DIR
	"${CMAKE_CURRENT_BINARY_DIR}/${SHADER_HEADER_DIR}"
)

if (LGENG_VULKAN_API)
	# Firstly, compile Vulkan shaders into .spv.

	add_custom_target(compile${APPLICATION_NAME}VKSpv)

	# Find all shader files
	file(GLOB_RECURSE SHADERS
		${SHADER_BINARY_DIR}/*.vert
		${SHADER_BINARY_DIR}/*.frag
	)

	# Make the shader bin directory (if it doesn't exist)
	file(MAKE_DIRECTORY ${BUILD_SHADER_DIR})

	# Compile all shaders with glslc
	foreach(source IN LISTS SHADERS)
		# Get the name of the file without path (with extension)
		get_filename_component(FILENAME ${source} NAME)

		# Run glslc for each shader
		add_custom_command(
			TARGET compile${APPLICATION_NAME}VKSpv
			PRE_BUILD
			COMMAND
				glslc
				# Output file
				-o \"${BUILD_SHADER_DIR}/${FILENAME}.spv\"
				# Source file
				\"${source}\"
		)

		list(APPEND SPV_SHADERS "${BUILD_SHADER_DIR}/${FILENAME}.spv")
	endforeach()


	# Compile SPV into .h
	add_custom_target(convert${APPLICATION_NAME}SpvToHeaders)
	add_dependencies(convert${APPLICATION_NAME}SpvToHeaders compile${APPLICATION_NAME}VKSpv)

	file(MAKE_DIRECTORY ${BUILD_SHADER_HEADER_DIR})

	foreach(source IN LISTS SPV_SHADERS)
		# Get the name of the file without path (with extension)
		get_filename_component(FILENAME ${source} NAME)

		add_custom_command(
			TARGET convert${APPLICATION_NAME}SpvToHeaders
			PRE_BUILD
			COMMAND
				bin2cpp
				-s ${source}
				-o \"${BUILD_SHADER_HEADER_DIR}/${FILENAME}.h\"
				-n LegendEngine::Resources::VulkanShaders
		)
	endforeach()
endif()

# ----- END SHADER-ASSET COMPILATION  -----
# ----- BEGIN ASSETS COMPILATION -----

# Convert all assets into header files with bin2cpp

# Target & deps
add_custom_target(compile${APPLICATION_NAME}Assets)
add_dependencies(compile${APPLICATION_NAME}Assets bin2cpp)

# Find all assets (relative to the list directory)
file(GLOB_RECURSE ASSETS
	${COMMON_DIR}/*.*
)

file(MAKE_DIRECTORY ${BUILD_COMPILED_DIR})

# If Tether.Rendering is compiled with Vulkan features, then add the target that
# compiles VULKAN shaders into SPV.
if (LGENG_VULKAN_API)
	add_dependencies(compile${APPLICATION_NAME}Assets convert${APPLICATION_NAME}SpvToHeaders)
endif()

foreach(source IN LISTS ASSETS)
	# Get the name of the file without path (with extension)
	get_filename_component(FILENAME ${source} NAME)

	file(RELATIVE_PATH FILE_RELPATH ${CMAKE_CURRENT_LIST_DIR}/${COMMON_DIR} ${source})
	get_filename_component(RELPATH ${FILE_RELPATH} DIRECTORY)

	# Potential issue: There may be multiple assets with the same name so they
	# should be stored in their folders relative to the Assets folder
	set(OUTPUT_DIRECTORY "${BUILD_COMPILED_DIR}/${RELPATH}")
	set(OUTPUT_FILE "${OUTPUT_DIRECTORY}/${FILENAME}.h")

	add_custom_command(
		TARGET compileTetherAssets 
		COMMAND
			bin2cpp
			-s ${source}
			-o ${OUTPUT_FILE}
			-n LegendEngine::Resources
			-mkd # -mkd tells bin2cpp to create the directory the output is in
	)
endforeach()
