cmake_minimum_required (VERSION 3.8)
project ("LegendEngine")

add_subdirectory(Vendor)
add_subdirectory(Tests/Math)
add_subdirectory(Assets)

set(CMAKE_CXX_STANDARD 23)

add_library(${PROJECT_NAME} STATIC 
		Source/Application.cpp
		Headers/LegendEngine/Application.hpp
		Headers/LegendEngine/Graphics/Renderer.hpp
		Headers/LegendEngine/Graphics/GraphicsContext.hpp
		Source/Graphics/GraphicsContext.cpp
		Source/Common/Stopwatch.cpp
		Headers/LegendEngine/Objects/Camera.hpp
		Headers/LegendEngine/Objects/Object.hpp
		Headers/LegendEngine/Resources/Resource.hpp
		Headers/LegendEngine/Components/Component.hpp
		Headers/LegendEngine/Components/ComponentHolder.hpp
		Headers/LegendEngine/Resources/Texture2D.hpp
		Headers/LegendEngine/Resources/Material.hpp
		Headers/LegendEngine/Script.hpp
		Headers/LegendEngine/Scene.hpp
		Headers/LegendEngine/Common/Logger.hpp
		Headers/LegendEngine/Graphics/VertexBuffer.hpp
		Headers/LegendEngine/Objects/ObjectCreator.hpp
		Headers/LegendEngine/Objects/Object.hpp
		Headers/LegendEngine/Components/MeshComponent.hpp
		Source/Common/Logger.cpp
		Source/Script.cpp
		Headers/LegendEngine/Events/EventBus.hpp
		Headers/LegendEngine/Events/EventBusSubscriber.hpp
		Source/Events/EventBusSubscriber.cpp
		Source/Events/EventBus.cpp
		Source/Events/Event.cpp
		Source/Scene.cpp
		Source/Events/ComponentAddedEvent.cpp
		Source/Events/ComponentRemovedEvent.cpp
		Source/Components/ComponentHolder.cpp
		Source/Objects/Object.cpp
		Source/ScriptHolder.cpp
		Headers/LegendEngine/Events/ObjectDestroyedEvent.hpp
		Source/Events/ObjectDestroyedEvent.cpp
		Source/Objects/Camera.cpp
		Headers/LegendEngine/Events/RenderEvent.hpp
		Headers/LegendEngine/Events/UpdateEvent.hpp
		Source/Events/UpdateEvent.cpp
		Source/Events/RenderEvent.cpp
		Source/IO/TextureLoader.cpp
		Headers/LegendEngine/IO/TextureLoader.hpp
		Source/Resources/Texture2D.cpp
		Source/Resources/Material.cpp
		Source/Resources/Shader.cpp
		Source/Components/MeshComponent.cpp
		Headers/LegendEngine/Resources/Shader.hpp
		Source/Graphics/Renderer.cpp
		Source/Components/Component.cpp
		Source/Resources/Resource.cpp
		Source/Graphics/VertexBuffer.cpp
		Source/IO/StbImageImplementation.cpp)
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
	if (NOT Vulkan_FOUND)
		message(FATAL_ERROR "Vulkan SDK not found. Install it from https://vulkan.lunarg.com/, or use the vulkan-sdk apt package")
	endif()

	add_definitions(-D_VULKAN_API)
	set(TETHER_RENDERING_VULKAN_API ON)

	target_sources(${PROJECT_NAME} PRIVATE
			Source/Graphics/Vulkan/VulkanGraphicsContext.cpp
			Headers/LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp
			Source/Graphics/Vulkan/VulkanRenderer.cpp
			Source/Graphics/Vulkan/VulkanTexture2D.cpp
			Source/Graphics/Vulkan/VulkanShader.cpp
			Source/Graphics/Vulkan/VulkanMaterial.cpp
			Headers/LegendEngine/Graphics/Vulkan/VulkanTexture2D.hpp
			Headers/LegendEngine/Graphics/Vulkan/VulkanMaterial.hpp
			Headers/LegendEngine/Graphics/Vulkan/VulkanShader.hpp
			Headers/LegendEngine/Graphics/Vulkan/VulkanVertexBuffer.hpp
			Source/Graphics/Vulkan/VulkanVertexBuffer.cpp
			Source/Graphics/Vulkan/ShaderModule.cpp
			Headers/LegendEngine/Graphics/Vulkan/ShaderModule.hpp
			Source/Graphics/Vulkan/Pipeline.cpp
			Source/Graphics/Vulkan/VmaImplementation.cpp
	)

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
