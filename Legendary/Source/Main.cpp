#include <iostream>
#include <chrono>
#include <thread>

#include <LegendEngine/LegendEngine.hpp>
#include <LegendEngine/IO/Logger.hpp>
#include <LegendEngine/Events/UpdateEvent.hpp>

using namespace le;

using namespace std::literals::chrono_literals;

class ObjectManager
{
public:
	explicit ObjectManager(Scene* pScene)
	{
		m_Object = pScene->CreateObject<Object>();

		// Add the mesh component
		{
			VertexTypes::Vertex3 testVertices[] =
			{
				{  0.5f,  0.5f, 0.0f,  1.0f, 1.0f },
				{  0.5f, -0.5f, 0.0f,  1.0f, 0.0f },
				{ -0.5f, -0.5f, 0.0f,  0.0f, 0.0f },
				{ -0.5f,  0.5f, 0.0f,  0.0f, 1.0f }
			};

			uint32_t indices[] =
			{
				0, 1, 3,
				1, 2, 3
			};

			m_Mesh = &m_Object->AddComponent<MeshComponent>(
				std::span<VertexTypes::Vertex3>(testVertices),
				std::span<uint32_t>(indices), MeshComponent::UpdateFrequency::UPDATES_ONCE);
		}

		m_Object->SetRotation(Math::AngleAxis(Math::Radians(90.0f), Vector3f(0, 1, 0)));
	}

	Object* m_Object = nullptr;
	MeshComponent* m_Mesh = nullptr;
};

class CameraManager final : public Input::InputListener
{
public:
	explicit CameraManager(Scene* pScene)
		:
		m_Window(Application::Get().GetWindow()),
		m_Sub(Application::Get().GetEventBus())
	{
		// Create the camera
		m_Camera = pScene->CreateObject<Camera>();
		m_Camera->SetNearZ(0.01f);

		Application::Get().SetActiveCamera(m_Camera);

		m_Window.AddInputListener(*this, Input::InputType::KEY);
		m_Window.AddInputListener(*this, Input::InputType::RAW_MOUSE_MOVE);
		m_Window.AddInputListener(*this, Input::InputType::MOUSE_CLICK);

		m_Camera->SetPosition(Vector3f(-1.0f, 2.0f, 4.0f));

		m_Sub.AddEventHandler<UpdateEvent>([this](const UpdateEvent& e)
		{
			OnUpdate(e.GetDeltaTime());
		});
	}

	void OnUpdate(const float delta) const
	{
		Vector2f moveDir;
		if (keys.w)
			moveDir += Vector2f(1, 0);
		if (keys.a)
			moveDir += Vector2f(0, -1);
		if (keys.s)
			moveDir += Vector2f(-1, 0);
		if (keys.d)
			moveDir += Vector2f(0, 1);

		Vector2f normMoveDir = Math::Normalize(moveDir);
		if (keys.ctrl)
			normMoveDir *= 3.0f;

		Vector3f forward = m_Camera->GetForwardVector();
		forward.y = 0;
		forward = Math::Normalize(forward);

		m_Camera->AddPosition(forward * normMoveDir.x * delta);
		m_Camera->AddPosition(m_Camera->GetRightVector() * normMoveDir.y * delta);

		// Vertical movement
		if (keys.space)
			m_Camera->AddPosition(Vector3f(0, 1, 0) * delta);
		if (keys.shift)
			m_Camera->AddPosition(Vector3f(0, -1, 0) * delta);
	}

	void OnKey(Input::KeyInfo& info) override
	{
		const bool pressed = info.IsPressed();
		switch (info.GetKey())
		{
			case Utils::Keycodes::KEY_W: keys.w = pressed; break;
			case Utils::Keycodes::KEY_A: keys.a = pressed; break;
			case Utils::Keycodes::KEY_S: keys.s = pressed; break;
			case Utils::Keycodes::KEY_D: keys.d = pressed; break;

			case Utils::Keycodes::KEY_SPACE: keys.space = pressed; break;
			case Utils::Keycodes::KEY_LEFT_SHIFT: keys.shift = pressed; break;
			case Utils::Keycodes::KEY_LEFT_CONTROL: keys.ctrl = pressed; break;

			case Utils::Keycodes::KEY_ESCAPE:
			{
				m_Window.SetCursorMode(Tether::Window::CursorMode::NORMAL);
				m_CaptureMouse = false;
			}
			break;

			default: break;
		}
	}

	const float sense = 0.04f;
	void OnRawMouseMove(Input::RawMouseMoveInfo& info) override
	{
		if (!m_CaptureMouse)
			return;

		horizontal += static_cast<float>(info.GetRawX()) * sense;
		vertical += static_cast<float>(info.GetRawY()) * sense;

		if (vertical > 89.9f)
			vertical = 89.9f;
		if (vertical < -89.9f)
			vertical = -89.9f;

		Quaternion q = Math::AngleAxis(Math::Radians(vertical), Vector3f(1, 0, 0));
		q *= Math::AngleAxis(Math::Radians(horizontal), Vector3f(0, 1, 0));
		
		m_Camera->SetRotation(q);
	}

	void OnMouseClick(Input::MouseClickInfo& info) override
	{
		if (m_CaptureMouse)
			return;

		m_Window.SetCursorMode(Tether::Window::CursorMode::DISABLED);
		m_CaptureMouse = true;
	}

	struct Keys
	{
		bool w = false;
		bool a = false;
		bool s = false;
		bool d = false;
		bool space = false;
		bool shift = false;
		bool ctrl = false;
	};
	Keys keys;

	float horizontal = 0.0f;
	float vertical = 0.0f;
private:
	bool m_CaptureMouse = true;

	Camera* m_Camera = nullptr;
	Tether::Window& m_Window;
	EventBusSubscriber m_Sub;
};

class Legendary final
{
public:
	explicit Legendary(Application& app)
		:
		m_App(app),
		m_Sub(m_App.GetEventBus()),
		testScene(Scene::Create()),
		camera(testScene.get()),
		cube1(testScene.get()),
		cube2(testScene.get()),
		floor(testScene.get())
	{
		CreateMaterials();
		CreateObjects();

		m_App.SetActiveScene(*testScene);

		Utils::Window& window = m_App.GetWindow();
		window.SetCursorMode(Tether::Window::CursorMode::DISABLED);
		window.SetRawInputEnabled(true);

		m_Sub.AddEventHandler<UpdateEvent>(
			[this](const UpdateEvent& e) { OnUpdate(); });
	}

	void OnUpdate()
	{
		if (fpsTimer.GetElapsedMillis() >= 5000.0f)
		{
			LE_INFO("FPS: {}", m_Frames / 5);
			fpsTimer.Set();
			m_Frames = 0;
		}

		m_Frames++;
	}
private:
	void CreateMaterials()
	{
		material = Material::Create();
		material2 = Material::Create();

		std::future<TextureData> planksLoader = TextureData::FromFile("Assets/planks.png");
		std::future<TextureData> tilesLoader = TextureData::FromFile("Assets/tiles.png");

		texture = Texture2D::Create(planksLoader.get());
		texture2 = Texture2D::Create(tilesLoader.get());

		material->SetTexture(texture.get());
		material2->SetTexture(texture2.get());
	}

	void CreateObjects()
	{
		cube1.m_Object->SetPosition(Vector3f(0, 0.5f, 0));
		cube1.m_Mesh->SetMaterial(material.get());
		cube2.m_Object->SetPosition(Vector3f(3, 0.5f, 0));
		cube2.m_Mesh->SetMaterial(material.get());
		floor.m_Object->SetScale(Vector3f(10));
		floor.m_Object->SetRotation(Math::AngleAxis(Math::Radians(90.0f), Vector3f(1, 0, 0)));
		floor.m_Mesh->SetMaterial(material2.get());

		light = m_App.GetGlobalScene().CreateObject<Light>();
	    LightComponent& lightComponent = light->GetLightComponent();
	    lightComponent.SetColor(Color(1, 0, 0, 1));
	}

	Application& m_App;
	EventBusSubscriber m_Sub;

	Stopwatch fpsTimer;

	Scope<Scene> testScene;

	CameraManager camera;
	ObjectManager cube1;
	ObjectManager cube2;
	ObjectManager floor;
    Light* light = nullptr;

	Scope<Material> material;
	Scope<Material> material2;
	Scope<Texture2D> texture;
	Scope<Texture2D> texture2;

	size_t m_Frames = 0;
};

#include <LegendEngine/Common/Entrypoint.hpp>
LEGENDENGINE_MAIN
{
	Application::Create(1280, 720, "Legendary", GraphicsAPI::VULKAN);

	{
		Legendary legend(Application::Get());
		Application::Run();
	}

	Application::Destroy();
}
