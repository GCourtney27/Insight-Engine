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

	if (!FileSystem::Instance()->Initialize(this))
	{
		ErrorLogger::Log("Failed to initialize File System.");
		return false;
	}

	if (!this->render_window.Initialize(this, hInstance, window_title, window_class, width, height))
	{
		ErrorLogger::Log("Filed to initialize WindowContainer.");
		return false;
	}

	if (!Graphics::Instance()->Initialize(this->render_window.GetHWND(), width, height, this))
	{
		ErrorLogger::Log("Failed to initialize Graphics Instance");
		return false;
	}

	if (!FileSystem::Instance()->LoadSceneFromJSON("Data\\Scenes\\PBR_TexturedShowcase.json", &scene, Graphics::Instance()->GetDevice(), Graphics::Instance()->GetDeviceContext(), Graphics::Instance()->GetDefaultVertexShader()))
	{
		ErrorLogger::Log("Failed to initialize scene.");
		return false;
	}

	if (!LuaStateManager::GetStateManager()->Create())
	{
		ErrorLogger::Log("Failed to initialize Lua State Manager");
		return false;
	}

	
	// Enable this to draw the lights mesh (Commented does not effect the lights emission behavior)
	//scene.AddEntity(Graphics::Instance()->pointLight);


	// ENABLE THIS FOR PLAY MODE. Its just disabled so we can see the materials better
	/*textures.push_back("Data\\Textures\\Iron\\IronOld_Albedo.png");
	textures.push_back("Data\\Textures\\Iron\\IronOld_Normal.png");
	textures.push_back("Data\\Textures\\Iron\\IronOld_Metallic.png");
	textures.push_back("Data\\Textures\\Iron\\IronOld_Roughness.png");
	m_pMaterial = new Material(Graphics::Instance()->GetDevice(), Graphics::Instance()->GetDeviceContext(), "PBR_MAPPED", textures);

	player = new Player(&scene, *(new ID("Player")));

	player->GetTransform().SetPosition(0.0f, 0.0f, 0.0f);
	player->GetTransform().SetRotation(0.0f, 0.0f, 0.0f);
	player->GetTransform().SetScale(1.0f, 1.0f, 1.0f);

	MeshRenderer* mr = player->AddComponent<MeshRenderer>();
	mr->Initialize(player, "Data\\Objects\\GraniteRock\\Rock_LOD0.fbx", Graphics::Instance()->GetDevice(), Graphics::Instance()->GetDeviceContext(), Graphics::Instance()->GetDefaultVertexShader(), m_pMaterial);

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


	if (!Debug::Editor::Instance()->Initialize(this, this->render_window.GetHWND()))
	{
		ErrorLogger::Log("Failed to initialize Editor Instance.");
		return false;
	}
	


	return true;
}

bool Engine::ProccessMessages()
{
	return this->render_window.ProccessMessages();
}

void Engine::Update()
{
	float dt = (float)Timer::Instance()->GetTicks();
	float gamedt = (float)Timer::Instance()->GetTicks();
	Timer::Instance()->Restart();

	
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
				Graphics::Instance()->editorCamera.AdjustRotation((float)me.GetPosY() * 0.005f, (float)me.GetPosX() * 0.005f, 0);
			}
		}

	}

	scene.Update(dt);

	if (Debug::Editor::Instance()->PlayingGame())
		scene.OnUpdate(gamedt);

	Graphics::Instance()->Update(dt);
	Debug::Editor::Instance()->Update(dt);


	
	if (InputManager::Instance()->keyboard.KeyIsPressed(27))
	{
		exit(0); // Performs no cleanup
		PostMessage(this->render_window.GetHWND(), WM_QUIT, 0, 0);
	}

	if (InputManager::Instance()->keyboard.KeyIsPressed('C'))
	{
		Graphics::Instance()->pointLight->GetTransform().SetPosition(Graphics::Instance()->editorCamera.GetPositionFloat3());
		Graphics::Instance()->pointLight->GetTransform().SetRotation(Graphics::Instance()->editorCamera.GetRotationFloat3());
	}
	
	/*m_saveDelay -= dt;
	DEBUGLOG(std::to_string(dt));
	if (m_saveDelay <= 0.0f)
	{
		m_saveDelay = 10.0f;
		m_canSave = true;
	}*/

	if (InputManager::Instance()->keyboard.KeyIsPressed(VK_CONTROL) && InputManager::Instance()->keyboard.KeyIsPressed('S'))
	{
		if (!FileSystem::Instance()->WriteSceneToJSON(&scene))
			ErrorLogger::Log("Failed to save scene");
		else
			DEBUGLOG("Scene saved");
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



