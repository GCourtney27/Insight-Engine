#include "Engine.h"
#include "Editor\\Editor.h"
#include "Systems\\Timer.h"
#include "Graphics\\Graphics.h"
#include "..\\Systems\\FileSystem.h"
#include "Components\\MeshRenderComponent.h"
#include "Components\\EditorSelectionComponent.h"
#include <iostream>

bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	windowHeight = height;
	windowWidth = width;

	Timer::Instance()->Start();
	FileSystem::Instance()->Initialize(this);

	if (!this->render_window.Initialize(this, hInstance, window_title, window_class, width, height))
		return false;

	if (!Graphics::Instance()->Initialize(this->render_window.GetHWND(), width, height, this))
		return false;

	if (!FileSystem::Instance()->LoadSceneFromJSON("Data\\Scenes\\scene_json.txt", &scene, Graphics::Instance()->GetDevice(), Graphics::Instance()->GetDeviceContext(), Graphics::Instance()->GetDefaultVertexShader()))
	{
		ErrorLogger::Log("Failed to initialize scene.");
		return false;
	}

	if (!LuaStateManager::GetStateManager()->Create())
	{
		ErrorLogger::Log("Failed to initialize Lua state manager");
		return false;
	}

	/*player = new Player(&scene, *(new ID("Player")));

	player->GetTransform().SetPosition(0.0f, 0.0f, 0.0f);
	player->GetTransform().SetRotation(0.0f, 0.0f, 0.0f);
	player->GetTransform().SetScale(1.0f, 1.0f, 1.0f);

	MeshRenderer* mr = player->AddComponent<MeshRenderer>();
	mr->Initialize(player, "Data\\Objects\\Dandelion\\Var1\\Textured_Flower.obj", Graphics::Instance()->GetDevice(), Graphics::Instance()->GetDeviceContext(), Graphics::Instance()->GetDefaultVertexShader());

	EditorSelection* es = player->AddComponent<EditorSelection>();
	es->Initialize(player, 20.0f, player->GetTransform().GetPosition());

	scene.AddEntity(player);*/

	// Dandelion\\Var1\\Textured_Flower.obj
	/*entity = new Entity(&scene, *(new ID("PBR_Entity")));
	MeshRenderer* me = entity->AddComponent<MeshRenderer>();
	me->Initialize("Data\\Objects\\Primatives\\Sphere.fbx", Graphics::Instance()->GetDevice(), Graphics::Instance()->GetDeviceContext(), Graphics::Instance()->GetDefaultVertexShader(), Graphics::Instance()->GetWoodMaterial());
	entity->GetTransform().SetPosition(0.0f, 0.0f, 0.0f);
	entity->GetTransform().SetScale(10.0f, 10.0f, 10.0f);
	entity->GetTransform().SetRotation(0.0f, 0.0f, 0.0f);
	EditorSelection* es = entity->AddComponent<EditorSelection>();
	es->Initialize(20.0f, entity->GetTransform().GetPosition());
	scene.AddEntity(entity);*/

	if (!scene.Initialize())
	{
		ErrorLogger::Log("Failed to initialize scene");
		return false;
	}

	//Graphics::Instance()->InitSkybox();

	if (!Debug::Editor::Instance()->Initialize(this, this->render_window.GetHWND()))
		return false;
	


	return true;
}

bool Engine::ProccessMessages()
{
	return this->render_window.ProccessMessages();
}

void Engine::Update()
{
	float dt = (float)Timer::Instance()->GetDeltaTime();
	float gamedt = (float)Timer::Instance()->GetDeltaTime();
	Timer::Instance()->Restart();

	scene.Update(dt);

	if (Debug::Editor::Instance()->PlayingGame())
		scene.OnUpdate(gamedt);


	while (!InputManager::Instance()->keyboard.CharBufferIsEmpty())
	{
		unsigned char ch = InputManager::Instance()->keyboard.ReadChar();
	}

	while (!InputManager::Instance()->keyboard.KeyBufferIsEmpty())
	{
		KeyboardEvent kbe = InputManager::Instance()->keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
	}

	while (!InputManager::Instance()->mouse.EventBufferIsEmpty())
	{
		MouseEvent me = InputManager::Instance()->mouse.ReadEvent();
		if (InputManager::Instance()->mouse.IsRightDown())
		{
			if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
			{
				Graphics::Instance()->editorCamera.AdjustRotation((float)me.GetPosY() * 0.01f, (float)me.GetPosX() * 0.01f, 0);
			}
		}

	}

	Graphics::Instance()->Update(dt);
	Debug::Editor::Instance()->Update(dt);


	
	if (InputManager::Instance()->keyboard.KeyIsPressed(27))
	{
		exit(0); // Performs no cleanup
		PostMessage(this->render_window.GetHWND(), WM_QUIT, 0, 0);
	}

	if (InputManager::Instance()->keyboard.KeyIsPressed('C'))
	{
		Graphics::Instance()->light.SetPosition(Graphics::Instance()->editorCamera.GetPositionFloat3());
		Graphics::Instance()->light.SetRotation(Graphics::Instance()->editorCamera.GetRotationFloat3());
	}
	

	
	if (InputManager::Instance()->keyboard.KeyIsPressed(VK_CONTROL) && InputManager::Instance()->keyboard.KeyIsPressed('S'))
	{
		if (!FileSystem::Instance()->WriteSceneToJSON(&scene))
			ErrorLogger::Log("Failed to save scene");
		else
			Debug::Editor::Instance()->DebugLog("Scene Saved");
	}

}

void Engine::RenderFrame()
{
	Graphics::Instance()->RenderFrame();
}

void Engine::Shutdown()
{
	scene.Shutdown();
	Graphics::Instance()->Shutdown();
}

void Engine::OnGameStart()
{
	scene.OnStart();
}



