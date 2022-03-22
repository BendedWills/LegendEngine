/*
 * The Application class is an interface to be implemented by other types of
 * applications.
 * Using this class as an interface is optional.
 */

#ifndef _LEGENDENGINE_APPLICATION_HPP
#define _LEGENDENGINE_APPLICATION_HPP

#include <string>

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Common/Types.hpp>
#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/Stopwatch.hpp>
#include <LegendEngine/Graphics/Vulkan/Instance.hpp>
#include <LegendEngine/Scene.hpp>

#include <Tether/Tether.hpp>

namespace LegendEngine::Objects
{
	class Object;

	namespace Components
	{
		class Component;
	}
}

namespace LegendEngine
{
	class IRenderer;
	class Application : public IDisposable
	{
		friend Scene;
		friend Objects::Object;
	public:
		LEGENDENGINE_NO_COPY(Application);
		LEGENDENGINE_DISPOSE_ON_DESTRUCT(Application);

		Application() 
		{}

		/**
		 * @brief Initializes the application
		 * 
		 * @param applicationName The name of the application.
		 * @param logging If the application should log.
		 * @param debug If the application should log debug/verbose 
		 *  information.
		 * @param api The rendering api to use.
		 * 
		 * @returns True if initialization was successful; otherwise, false.
		 */
		bool Init(
			const std::string& applicationName,
			bool logging = false, 
			bool debug = false,
			RenderingAPI api = RenderingAPI::VULKAN
		);
		
		/**
		 * @param message The message to log
		 * @param type The type of log.
		 */
		void Log(const std::string& message, LogType type);

		/**
		 * @brief Creates an object.
		 *
		 * @returns The created object. A "Ref" (std::shared_ptr)
		 */
		template<typename T>
		Ref<T> CreateObject();
		/**
		 * @brief Initializes a scene to this application.
		 *
		 * @param scene The scene to initialize.
		 *
		 * @returns True if the scene was successfully initialized,
		 *  false if the scene is already initialized.
		 */
		bool InitScene(Scene& scene);
		/**
		 * @brief Initializes a scene to this application.
		 *
		 * @param pScene A pointer to the scene to initialize.
		 *
		 * @returns True if the object was successfully initialized,
		 *  false if the object is already initialized.
		 */
		bool InitScene(Scene* pScene);

		void RenderFrame();

		/**
		 * @brief Sets the active scene.
		 *
		 * @param scene The scene.
		 */
		void SetActiveScene(Scene& scene);
		/**
		 * @brief Sets the active scene.
		 *
		 * @param pScene A pointer to the scene.
		 */
		void SetActiveScene(Scene* pScene);
		void RemoveActiveScene();
		/**
		 * @returns The active scene (wow really?)
		 */
		Scene* GetActiveScene();
		Scene* GetDefaultScene();

		bool IsCloseRequested();

		Application* Get();
		IRenderer* GetRenderer();
		std::string GetName();
		Tether::IWindow* GetWindow();
	protected:
		/**
		 * @brief Called before the Init function is executed.
		 * 
		 * @return Should return true if starting was successful;
		 *  otherwise, false.
		 */
		virtual bool OnPreInit() { return true; }
		/**
		 * @brief Called after the Init function is called.
		 * 
		 * @return Should return true if starting was successful;
		 *  otherwise, false.
		 */
		virtual bool OnInit() { return true; }
		/**
		 * @brief Called every frame before screen clearing.
		 */
		virtual void OnPreUpdate() {}
		/**
		 * @brief Called every frame
		 */
		virtual void OnUpdate() {}
		/**
		 * @brief Called before the application is disposed.
		 */
		virtual void OnStop() {}
		/**
		 * @brief Called after the application is disposed.
		 */
		virtual void OnDisposed() {}
	protected:
		bool InitWindow(const std::string& title);

		void UpdateWindow();
		void Update(bool updateWindow = true);
		void Render();

		void DisposeGraphics();
		
		void OnDispose();

		std::string applicationName = "";
		bool logging = false;
		bool debug = false;

		Tether::SimpleWindow window;

	#pragma region Graphics
		Ref<IRenderer> pRenderer;

	#ifdef VULKAN_API
		bool vulkanInitialized = false;
		bool InitVulkan();

		class DebugCallback : public Vulkan::DebugCallback
		{
		public:
			Application* pApplication = nullptr;

			void OnDebugLog(
				VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				VkDebugUtilsMessageTypeFlagsEXT messageType,
				const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
			);
		};
		DebugCallback callback;
	#endif // VULKAN_API
	
	#pragma endregion Graphics

		void OnSceneObjectAdd(Scene* pScene, Objects::Object* pObject);
		void OnSceneObjectRemove(Scene* pScene, Objects::Object* pObject);
		void OnSceneObjectComponentAdd(
			Scene* pScene, Objects::Object* pObject,
			const std::string& typeName,
			Objects::Components::Component* pComponent
		);
		void OnSceneObjectComponentRemove(
			Scene* pScene, Objects::Object* pObject,
			const std::string& typeName, 
			Objects::Components::Component* pComponent
		);
	private:
		// Every application has a default scene. This scene contains objects
		// that are always rendered, no matter what the current set scene is.
		Scene defaultScene;
		Scene* activeScene = nullptr;

		std::vector<Objects::Object*> objects;
	};

	template<typename T>
	Ref<T> Application::CreateObject()
	{
		LEGENDENGINE_ASSERT_INITIALIZED_RET(false);

		if (!std::is_base_of<T, Object>())
		{
			Log("In Application::CreateObject: T is not of base class Object!",
				LogType::ERROR);
			return nullptr;
		}

		Ref<T> object = RefTools::Create<T>();
		object->pApplication = this;

		pRenderer->CreateObjectNative(object.get());

		objects.push_back(object.get());

		std::stringstream str;
		str << "Created Object (" << (uint64_t)object.get() << ")";
		str << " (" << (uint64_t)this << ")";

		Log(str.str(), LogType::DEBUG);

		return object;
	}
}

#endif //_LEGENDENGINE_APPLICATION_HPP