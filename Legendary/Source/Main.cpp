#include <iostream>
#include <chrono>
#include <thread>

#include <LegendEngine/LegendEngine.hpp>
#include <LegendEngine/Common/Logger.hpp>

using namespace LegendEngine;
using namespace Objects;
using namespace Resources;
using namespace Components;

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

class CameraScript final : public Script, Input::InputListener
{
public:
	explicit CameraScript(Camera* camera)
		:
		Script(*camera),
		m_Camera(*camera)
	{
		Application& app = Application::Get();
		Utils::Window& window = app.GetWindow();
		window.AddInputListener(*this, Input::InputType::KEY);
		window.AddInputListener(*this, Input::InputType::RAW_MOUSE_MOVE);

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

			default: break;
		}
	}

	const float sense = 0.04f;
	void OnRawMouseMove(Input::RawMouseMoveInfo& info) override
	{
		horz += static_cast<float>(info.GetRawX()) * sense;
		vert += static_cast<float>(info.GetRawY()) * sense;

		if (vert > 89.9f)
			vert = 89.9f;
		if (vert < -89.9f)
			vert = -89.9f;

		Quaternion q = Math::AngleAxis(Math::Radians(vert), Vector3f(1, 0, 0));
		q *= Math::AngleAxis(Math::Radians(horz), Vector3f(0, 1, 0));
		
		m_Object.SetRotation(q);
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

	float horz = 0.0f;
	float vert = 0.0f;
private:
	Camera& m_Camera;
};

class Legendary final : public Application
{
public:
	Legendary()
		:
#if !defined(NDEBUG)
		Application("Legendary", true, true, GraphicsAPI::VULKAN),
#else
		Application("Legendary", false, false, RenderingAPI::VULKAN),
#endif
		m_Logger(GetLogger())
	{}

	void OnUpdate(const float delta) override
	{
		if (fpsTimer.GetElapsedMillis() >= 5000.0f)
		{
			m_Logger.Log(Logger::Level::INFO,
				"FPS: " + std::to_string(1.0f / delta));
			fpsTimer.Set();
		}
	}
private:
	void OnSetup() override
	{
		GetWindow().SetCursorMode(Tether::Window::CursorMode::DISABLED);
		GetWindow().SetRawInputEnabled(true);

	    testScene = Scene::Create();

		// Create the camera
		camera = Objects::Create<Camera>();
		camera->AddScript<CameraScript>(camera.get());
		camera->SetNearZ(0.01f);
	    testScene->AddObject(*camera);

		SetActiveCamera(camera.get());

		CreateMaterials();
		CreateObjects();

		SetActiveScene(*testScene);
	}

	void CreateMaterials()
	{
		material = Material::Create();
		material2 = Material::Create();

		IO::TextureLoader planksLoader;
		IO::TextureLoader tilesLoader;
		planksLoader.FromFile("Assets/planks.png");
		tilesLoader.FromFile("Assets/tiles.png");

		texture = Texture2D::Create(planksLoader);
		texture2 = Texture2D::Create(tilesLoader);

		material->SetTexture(texture.get());
		material2->SetTexture(texture2.get());
	}

	void CreateObjects()
	{
		cube1 = Objects::Create<Object>();
		cube1->AddScript<TestScript>(cube1.get(), material.get());
		cube1->SetPosition(Vector3f(0, 0.5f, 0));

		cube2 = Objects::Create<Object>();
		cube2->AddScript<TestScript>(cube2.get(), material.get());
		cube2->SetPosition(Vector3f(3, 0.5f, 0));

		floor = Objects::Create<Object>();
		floor->AddScript<TestScript>(floor.get(), material2.get());
		floor->SetScale(Vector3f(10));
		floor->SetRotation(Math::AngleAxis(Math::Radians(90.0f), Vector3f(1, 0, 0)));

	    light = Objects::Create<Light>();
	    LightComponent& lightComponent = light->GetLightComponent();
	    lightComponent.SetColor(Color(1, 0, 0, 1));

		testScene->AddObject(*cube1.get());
		testScene->AddObject(*cube2.get());
	    GetGlobalScene().AddObject(*light);
		GetGlobalScene().AddObject(*floor.get());
	}

	Stopwatch fpsTimer;
	Stopwatch timer;

	Scope<Scene> testScene;

	std::unique_ptr<Camera> camera;
	std::unique_ptr<Object> cube1;
	std::unique_ptr<Object> cube2;
	std::unique_ptr<Object> floor;
    std::unique_ptr<Light> light;

	std::unique_ptr<Material> material;
	std::unique_ptr<Material> material2;
	std::unique_ptr<Texture2D> texture;
	std::unique_ptr<Texture2D> texture2;

	Logger& m_Logger;
};

#include <LegendEngine/Common/Platform.hpp>
LEGENDENGINE_MAIN
{
	return Application::RunApplication<Legendary>();
}
