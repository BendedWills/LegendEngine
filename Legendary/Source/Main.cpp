#include <iostream>
#include <chrono>
#include <thread>

#include <LegendEngine/LegendEngine.hpp>

using namespace LegendEngine;
using namespace LegendEngine::Objects;
using namespace std::literals::chrono_literals;

class TestScript : public Scripts::Script
{
public:
	TestScript() {}

	void OnInit()
	{
		// Add the mesh component
		{
			float r = (float)rand() / RAND_MAX;
			float g = (float)rand() / RAND_MAX;
			float b = (float)rand() / RAND_MAX;
			
			VertexTypes::Vertex3c testVertices[] =
			{
				{ -0.5f, -0.5f, -0.5f, r, g, b },
				{  0.5f, -0.5f, -0.5f, r, g, b },
				{  0.5f,  0.5f, -0.5f, r, g, b },
				{  0.5f,  0.5f, -0.5f, r, g, b },
				{ -0.5f,  0.5f, -0.5f, r, g, b },
				{ -0.5f, -0.5f, -0.5f, r, g, b },

				{ -0.5f, -0.5f,  0.5f, r, g, b },
				{  0.5f, -0.5f,  0.5f, r, g, b },
				{  0.5f,  0.5f,  0.5f, r, g, b },
				{  0.5f,  0.5f,  0.5f, r, g, b },
				{ -0.5f,  0.5f,  0.5f, r, g, b },
				{ -0.5f, -0.5f,  0.5f, r, g, b },

				{ -0.5f,  0.5f,  0.5f, r, g, b },
				{ -0.5f,  0.5f, -0.5f, r, g, b },
				{ -0.5f, -0.5f, -0.5f, r, g, b },
				{ -0.5f, -0.5f, -0.5f, r, g, b },
				{ -0.5f, -0.5f,  0.5f, r, g, b },
				{ -0.5f,  0.5f,  0.5f, r, g, b },

				{  0.5f,  0.5f,  0.5f, r, g, b },
				{  0.5f,  0.5f, -0.5f, r, g, b },
				{  0.5f, -0.5f, -0.5f, r, g, b },
				{  0.5f, -0.5f, -0.5f, r, g, b },
				{  0.5f, -0.5f,  0.5f, r, g, b },
				{  0.5f,  0.5f,  0.5f, r, g, b },

				{ -0.5f, -0.5f, -0.5f, r, g, b },
				{  0.5f, -0.5f, -0.5f, r, g, b },
				{  0.5f, -0.5f,  0.5f, r, g, b },
				{  0.5f, -0.5f,  0.5f, r, g, b },
				{ -0.5f, -0.5f,  0.5f, r, g, b },
				{ -0.5f, -0.5f, -0.5f, r, g, b },

				{ -0.5f,  0.5f, -0.5f, r, g, b },
				{  0.5f,  0.5f, -0.5f, r, g, b },
				{  0.5f,  0.5f,  0.5f, r, g, b },
				{  0.5f,  0.5f,  0.5f, r, g, b },
				{ -0.5f,  0.5f,  0.5f, r, g, b },
				{ -0.5f,  0.5f, -0.5f, r, g, b }
			};

			pObject->AddComponent<Components::MeshComponent>()->Init(
				testVertices, 36
			);
		}
	}
};

class CameraScript : public Scripts::Script, Input::InputListener
{
public:
	CameraScript() {}

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

		Vector2f normMoveDir = Vector2f::Normalize(moveDir);
		if (keys.ctrl)
			normMoveDir *= 3.0f;

		pObject->AddPosition(pCamera->GetForwardVector() * normMoveDir.x * delta);
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

	const float sense = 0.06f;
	void OnRawMouseMove(Input::RawMouseMoveInfo& info)
	{
		Vector3f rotation = pObject->GetRotation();

		rotation += Vector3f(
			 info.GetRawX() * sense,
			-info.GetRawY() * sense,
			0
		);

		if (rotation.y > 89.9f)
			rotation.y = 89.9f;
		if (rotation.y < -89.9f)
			rotation.y = -89.9f;

		pObject->SetRotation(rotation);
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

	Camera* pCamera;
};

class Triangle : public Application
{
public:
	bool OnInit()
	{
		pWindow->SetCursorMode(Utils::CursorMode::DISABLED);
		pWindow->SetRawInputEnabled(true);

		InitScene(testScene);

		camera = CreateObject<Camera>();

		CameraScript* script = camera->AddScript<CameraScript>();
		script->pCamera = camera.get();

		SetActiveCamera(camera.get());

		cube1 = CreateObject<Object>();
		cube1->AddScript<TestScript>();

		cube2 = CreateObject<Object>();
		cube2->AddScript<TestScript>();
		cube2->SetPosition(Vector3f(3, 0, 0));

		testScene.AddObject(cube1.get());
		testScene.AddObject(cube2.get());

		SetActiveScene(testScene);
		
		return true;
	}

	void OnStop()
	{
		testScene.ClearObjects();
	}
private:
	Scene testScene;

	Ref<Camera> camera;
	Ref<Object> cube1;
	Ref<Object> cube2;
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

		Stopwatch deltaTimer;
		while (!triangle.IsCloseRequested())
		{
			float delta = deltaTimer.GetElapsedMillis() / 1000.0f;
			deltaTimer.Set();

			triangle.RenderFrame(delta);
		}

		triangle.Dispose();
	}
	Context::Dispose();

	std::cout << "\nPress any key to continue..." << std::endl;
	std::cin.get();

	return EXIT_SUCCESS;
}
