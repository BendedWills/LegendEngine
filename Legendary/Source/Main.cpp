#include <iostream>
#include <chrono>
#include <thread>

#include <LegendEngine/LegendEngine.hpp>

using namespace LegendEngine;
using namespace LegendEngine::Objects;
using namespace LegendEngine::Resources;
using namespace LegendEngine::Objects::Components;

using namespace std::literals::chrono_literals;

class TestScript : public Scripts::Script
{
public:
	TestScript(Material* pMaterial) 
		:
		pMaterial(pMaterial)
	{}

	void OnInit()
	{
		// Add the mesh component
		{
			float r = (float)rand() / RAND_MAX;
			float g = (float)rand() / RAND_MAX;
			float b = (float)rand() / RAND_MAX;
			
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

			MeshComponent* mesh = pObject->AddComponent<Components::MeshComponent>();
			mesh->Init(testVertices, 4, indices, 6);
			mesh->SetMaterial(pMaterial);
		}

		pObject->SetRotation(Math::AngleAxis(Math::Radians(90.0f), Vector3f(0, 1, 0)));
	}
private:
	Material* pMaterial = nullptr;
};

class CameraScript : public Scripts::Script, Input::InputListener
{
public:
	CameraScript(Camera* pCamera) 
		:
		pCamera(pCamera)
	{}

	void OnInit()
	{
		SetRecieveUpdates(true);
		pApplication->pWindow->AddInputListener(this, Input::InputType::KEY);
		pApplication->pWindow->AddInputListener(this, Input::InputType::RAW_MOUSE_MOVE);
	}

	void OnUpdate(float delta)
	{
		float x = (float)rand() / RAND_MAX * 2 - 1;
		float y = (float)rand() / RAND_MAX * 2 - 1;
		float z = (float)rand() / RAND_MAX * 2 - 1;

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

		Vector3f forward = pCamera->GetForwardVector();
		forward.y = 0;
		forward = Math::Normalize(forward);

		pObject->AddPosition(forward * normMoveDir.x * delta);
		pObject->AddPosition(pCamera->GetRightVector() * normMoveDir.y * delta);

		// Vertical movement
		if (keys.space)
			pObject->AddPosition(Vector3f(0, 1, 0) * delta);
		if (keys.shift)
			pObject->AddPosition(Vector3f(0, -1, 0) * delta);
	}

	void OnKey(Input::KeyInfo& info)
	{
		bool pressed = info.IsPressed();
		switch (info.GetKey())
		{
			case Utils::Keycodes::KEY_W: keys.w = pressed; break;
			case Utils::Keycodes::KEY_A: keys.a = pressed; break;
			case Utils::Keycodes::KEY_S: keys.s = pressed; break;
			case Utils::Keycodes::KEY_D: keys.d = pressed; break;

			case Utils::Keycodes::KEY_SPACE: keys.space = pressed; break;
			case Utils::Keycodes::KEY_LEFT_SHIFT: keys.shift = pressed; break;
			case Utils::Keycodes::KEY_LEFT_CONTROL: keys.ctrl = pressed; break;
		}
	}

	const float sense = 0.04f;
	void OnRawMouseMove(Input::RawMouseMoveInfo& info)
	{
		horz += info.GetRawX() * sense;
		vert += info.GetRawY() * sense;

		if (vert > 89.9f)
			vert = 89.9f;
		if (vert < -89.9f)
			vert = -89.9f;

		Quaternion q = Math::AngleAxis(Math::Radians(vert), Vector3f(1, 0, 0));
		q *= Math::AngleAxis(Math::Radians(horz), Vector3f(0, 1, 0));
		
		pObject->SetRotation(q);
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
	Camera* pCamera;
};

class Triangle : public Application
{
public:
	bool OnPreInit()
	{
		pWindow->SetCursorMode(Utils::CursorMode::DISABLED);
		pWindow->SetRawInputEnabled(true);

		InitScene(testScene);

		// Create the camera
		camera = CreateObject<Camera>();
		camera->AddScript<CameraScript>(camera.get());
		camera->SetNearZ(0.01f);

		SetActiveCamera(camera.get());

		// Create the materials
		{
			material = CreateResource<Material>();
			material->Init();
			material2 = CreateResource<Material>();
			material2->Init();

			texture = CreateResource<Texture2D>();
			texture->Init("Assets/planks.png");
			texture2 = CreateResource<Texture2D>();
			texture2->Init("Assets/tiles.png");
			
			material->SetTexture(texture.get());
			material2->SetTexture(texture2.get());
		}

		// Create the objects
		{
			cube1 = CreateObject<Object>();
			cube1->AddScript<TestScript>(material.get());
			cube1->SetPosition(Vector3f(0, 0.5f, 0));

			cube2 = CreateObject<Object>();
			cube2->AddScript<TestScript>(material.get());
			cube2->SetPosition(Vector3f(3, 0.5f, 0));

			floor = CreateObject<Object>();
			floor->AddScript<TestScript>(material2.get());
			floor->SetScale(Vector3f(10));
			floor->SetRotation(Math::AngleAxis(Math::Radians(90.0f), Vector3f(1, 0, 0)));
			
			// Add the objects to the scene
			testScene.AddObject(cube1.get());
			testScene.AddObject(cube2.get());
			GetDefaultScene()->AddObject(floor.get());
		}

		// Lastly, set the active scene.
		// Setting the active scene once everything is initialized gains some
		// performance since objects notify the scene when they are added for misc
		// init tasks.
		SetActiveScene(testScene);
		
		return true;
	}

	void OnUpdate(float delta)
	{
		if (fpsTimer.GetElapsedMillis() >= 5000.0f)
		{
			Log("FPS: " + std::to_string(1.0f / delta), LogType::INFO);
			fpsTimer.Set();
		}
	}

	void OnStop()
	{
		testScene.ClearObjects();

		camera->Dispose();
		cube1->Dispose();
		cube2->Dispose();
		floor->Dispose();
		
		material->Dispose();
		material2->Dispose();
		texture->Dispose();
		texture2->Dispose();
	}
private:
	Stopwatch fpsTimer;
	Stopwatch timer;

	Scene testScene;

	Ref<Camera> camera;
	Ref<Object> cube1;
	Ref<Object> cube2;
	Ref<Object> floor;
	
	Ref<Material> material;
	Ref<Material> material2;
	Ref<Texture2D> texture;
	Ref<Texture2D> texture2;
};

#if defined(_WIN32) && !defined(_DEBUG)
#include <Windows.h>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine,
	int nCmdShow)
#else
int main()
#endif
{
	Context::InitAPI(RenderingAPI::VULKAN, true);
	{
		Triangle triangle;
#if defined(_DEBUG)
		if (!triangle.Init("Legendary", true, true))
			return EXIT_FAILURE;
#else
		if (!triangle.Init("Legendary", false, false))
			return EXIT_FAILURE;
#endif

		if (!triangle.Run())
			return EXIT_FAILURE;

		triangle.Dispose();
	}
	Context::Dispose();

#ifdef _WIN32
	std::cout << "\nPress any key to continue..." << std::endl;
	std::cin.get();
#endif

	return EXIT_SUCCESS;
}
