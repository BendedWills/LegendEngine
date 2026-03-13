#include <iostream>
#include <chrono>
#include <thread>

#include <LegendEngine/LegendEngine.hpp>
#include <LegendEngine/IO/Logger.hpp>
#include <LegendEngine/Events/UpdateEvent.hpp>

using namespace le;

using namespace std::literals::chrono_literals;

class TestScript final : public Script
{
public:
	explicit TestScript(Object* pObject, Material* material)
		:
		Script(*pObject),
		m_Material(*material)
	{
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

			auto& mesh = m_Object.AddComponent<MeshComponent>(
			    std::span<VertexTypes::Vertex3>(testVertices),
			    std::span<uint32_t>(indices));
			mesh.SetMaterial(&m_Material);
		}

		m_Object.SetRotation(Math::AngleAxis(Math::Radians(90.0f), Vector3f(0, 1, 0)));
	}
private:
	Material& m_Material;
};

class CameraScript final : public Script, public Input::InputListener
{
public:
	explicit CameraScript(Camera* camera)
		:
		Script(*camera),
		m_Window(Application::Get().GetWindow()),
		m_Camera(*camera)
	{
		m_Window.AddInputListener(*this, Input::InputType::KEY);
		m_Window.AddInputListener(*this, Input::InputType::RAW_MOUSE_MOVE);
		m_Window.AddInputListener(*this, Input::InputType::MOUSE_CLICK);

		m_Camera.SetPosition(Vector3f(-1.0f, 2.0f, 4.0f));

		ListenForUpdates();
	}

	void OnUpdate(const float delta) override
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

		Vector3f forward = m_Camera.GetForwardVector();
		forward.y = 0;
		forward = Math::Normalize(forward);

		m_Object.AddPosition(forward * normMoveDir.x * delta);
		m_Object.AddPosition(m_Camera.GetRightVector() * normMoveDir.y * delta);

		// Vertical movement
		if (keys.space)
			m_Object.AddPosition(Vector3f(0, 1, 0) * delta);
		if (keys.shift)
			m_Object.AddPosition(Vector3f(0, -1, 0) * delta);
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
		
		m_Object.SetRotation(q);
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

	Camera& m_Camera;
	Tether::Window& m_Window;
};

class Legendary final
{
public:
	explicit Legendary(Application& app)
		:
		m_App(app),
		m_Sub(m_App.GetEventBus())
	{
		testScene = Scene::Create();

		// Create the camera
		camera = testScene->CreateObject<Camera>();
		camera->AddScript<CameraScript>(camera);
		camera->SetNearZ(0.01f);

		m_App.SetActiveCamera(camera);

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
		cube1 = testScene->CreateObject<Object>();
		cube1->AddScript<TestScript>(cube1, material.get());
		cube1->SetPosition(Vector3f(0, 0.5f, 0));

		cube2 = testScene->CreateObject<Object>();
		cube2->AddScript<TestScript>(cube2, material.get());
		cube2->SetPosition(Vector3f(3, 0.5f, 0));

		floor = m_App.GetGlobalScene().CreateObject<Object>();
		floor->AddScript<TestScript>(floor, material2.get());
		floor->SetScale(Vector3f(10));
		floor->SetRotation(Math::AngleAxis(Math::Radians(90.0f), Vector3f(1, 0, 0)));

		light = m_App.GetGlobalScene().CreateObject<Light>();
	    LightComponent& lightComponent = light->GetLightComponent();
	    lightComponent.SetColor(Color(1, 0, 0, 1));
	}

	Application& m_App;
	EventBusSubscriber m_Sub;

	Stopwatch fpsTimer;

	Scope<Scene> testScene;

	Camera* camera = nullptr;
	Object* cube1 = nullptr;
	Object* cube2 = nullptr;
	Object* floor = nullptr;
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
