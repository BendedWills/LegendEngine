cmake_minimum_required (VERSION 3.8)

# Definitions
set(ASSETS_DIR Assets)
set(ASSETS_BIN_DIR ${ASSETS_DIR}/Bin)
set(COMPILED_ASSETS_DIR ${ASSETS_BIN_DIR}/CompiledAssets)
set(VULKAN_SDK_PATH "C:\\VulkanSDK\\1.3.204.1") # Only needed on Windows
set(VK_SHADER_BINARY_DIR ${ASSETS_BIN_DIR}/Shaders)
set(VK_SHADER_SOURCE_DIR ${ASSETS_DIR}/Shaders)

# Add compiler definitions here.
# Compiler definitions will also need to be defined in the application using
# Legend engine.
# Valid definitions are:
# VULKAN_API
# THREE_DIM
# TWO_DIM
add_definitions(-DVULKAN_API -DTHREE_DIM -DTWO_DIM)

file(REMOVE_RECURSE ${CMAKE_SOURCE_DIR}/LegendEngine/${COMPILED_ASSETS_DIR})
file(GLOB_RECURSE ASSETS
    ${ASSETS_DIR}/*.*
)

# Compile vulkan shaders
# Only if VULKAN_API is defined above
get_directory_property(COMPDEFS COMPILE_DEFINITIONS)
if (COMPDEFS MATCHES "^VULKAN_API")
    # VULKAN_API defined

    add_subdirectory("Vendor/VMA")
    if (UNIX)
        set(VMA_DYNAMIC_VULKAN_FUNCTIONS ON)
        set(VMA_STATIC_VULKAN_FUNCTIONS OFF)
    endif()

    # Compile shaders

    # GLSLC is required for Vulkan shader compilation
    find_program(glslc_executable NAMES glslc)
    find_package(Vulkan REQUIRED)

    file(GLOB_RECURSE SHADERS
        ${VK_SHADER_SOURCE_DIR}/*.vert
        ${VK_SHADER_SOURCE_DIR}/*.frag
    )
    
    # Make the shader directory
    set (SHADER_BINARY_DIR "\"${CMAKE_SOURCE_DIR}/LegendEngine/${VK_SHADER_BINARY_DIR}\"")

    if (WIN32)
        add_custom_command(
            COMMAND
                IF not exist ${SHADER_BINARY_DIR} ( mkdir ${SHADER_BINARY_DIR} )
            OUTPUT shaderdir
            COMMENT "Creating compiled shaders directory"
        )
    elseif(UNIX)
        add_custom_command(
            COMMAND
                mkdir -p ${SHADER_BINARY_DIR}
            OUTPUT shaderdir
            COMMENT "Creating compiled shaders directory"
        )
    endif()

    foreach(source IN LISTS SHADERS)
        # Get the name of the file without path or extention
        get_filename_component(FILENAME ${source} NAME)

        # Run glslc for each shader
        add_custom_command(
            COMMAND
                ${glslc_executable}
                # Output file
                -o ${CMAKE_SOURCE_DIR}/LegendEngine/${VK_SHADER_BINARY_DIR}/${FILENAME}.spv
                # Source file
                ${source}
            OUTPUT ${FILENAME}.spv
            COMMENT "Compiling shader: ${FILENAME}"
        )

        list(APPEND SPV_SHADERS ${FILENAME}.spv)
    endforeach()

    add_custom_target(vkshaders ALL DEPENDS ${SPV_SHADERS} shaderdir)
endif()

set (FULL_COMPILED_DIR "\"${CMAKE_SOURCE_DIR}/LegendEngine/${COMPILED_ASSETS_DIR}\"")
if (WIN32)
    add_custom_command(
        COMMAND
            IF not exist ${FULL_COMPILED_DIR} ( mkdir ${FULL_COMPILED_DIR} )
        OUTPUT assetscompileddir
        COMMENT "Creating compiled assets directory"
    )
elseif(UNIX)
    add_custom_command(
        COMMAND
            mkdir -p ${FULL_COMPILED_DIR}
        OUTPUT assetscompileddir
        COMMENT "Creating compiled assets directory"
    )
endif()

add_custom_target(compileassets ALL DEPENDS assetscompileddir)
# Add vulkan shaders dependency
if (COMPDEFS MATCHES "^VULKAN_API")
    add_dependencies(compileassets vkshaders)
endif()

foreach(source IN LISTS ASSETS)
    # Get the name of the file without path or extention
    get_filename_component(FILENAME ${source} NAME)
    string(REPLACE "." "_" ARRAY_NAME "${FILENAME}")
    set(OUTPUT_FILE 
        ${CMAKE_SOURCE_DIR}/LegendEngine/${COMPILED_ASSETS_DIR}/${FILENAME}.h)

    find_program(bin2c_executable NAMES bin2c)

    if (WIN32)
        # Set a variable for pragma once because the hashtag is a comment.
        set (PRAGMA \#pragma once)
        
        add_custom_command(TARGET compileassets COMMAND
            echo ${PRAGMA} > ${OUTPUT_FILE})
        add_custom_command(TARGET compileassets COMMAND
            echo namespace LegendEngine::Resources { >> ${OUTPUT_FILE})

        add_custom_command(
            TARGET compileassets
            COMMAND
                ${bin2c_executable}
                --name ${ARRAY_NAME}
                ${source} 
                >> ${OUTPUT_FILE}
        )

        add_custom_command(TARGET compileassets COMMAND
            echo } >> ${OUTPUT_FILE})
    elseif (UNIX)
        add_custom_command(TARGET compileassets COMMAND
            echo \"\#pragma once\" > ${OUTPUT_FILE})
        add_custom_command(TARGET compileassets COMMAND
            echo \"namespace LegendEngine::Resources {\" >> ${OUTPUT_FILE})

        add_custom_command(
            TARGET compileassets
            COMMAND
                bin2c
                --name ${ARRAY_NAME}
                ${source} 
                >> ${OUTPUT_FILE}
        )

        add_custom_command(TARGET compileassets COMMAND
            echo \"}\" >> ${OUTPUT_FILE})
    endif()
endforeach()

project ("LegendEngine")

# Add Tests
add_subdirectory(Tests/Triangle)
add_subdirectory(Tests/APIMisuse)

# Add tether
add_subdirectory(Vendor/Tether)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_STANDARD 17)

# Collect all sources
file(GLOB_RECURSE SOURCES 
    "Source/*.cpp" 
    "Source/*.c" 
    "Headers/*.hpp" 
    "Headers/*.h" 
    "Assets/Compiled/*.h" 
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
add_dependencies(${PROJECT_NAME} tether compileassets)

set(VK_LIBS)
set(VK_INCLUDES)

if (COMPDEFS MATCHES "^VULKAN_API")
    add_dependencies(${PROJECT_NAME} vkshaders VulkanMemoryAllocator)

    set(VK_LIBS ${VK_LIBS} ${Vulkan_LIBRARIES})
    set(VK_INCLUDES ${VK_INCLUDES} ${Vulkan_INCLUDE_DIR})
endif()

target_include_directories(${PROJECT_NAME} 
PUBLIC
    "Headers"
    "${ASSETS_BIN_DIR}"
    "Vendor/Tether/Headers"
    "Vendor/VMA/include"
PRIVATE
    ${VK_INCLUDES}
)
target_link_libraries(${PROJECT_NAME} 
    tether
    ${VK_LIBS}
)
