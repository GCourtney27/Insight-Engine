#include "Engine.h"
#include "Editor\\Editor.h"
#include "Graphics\\Graphics.h"
#include "..\\Systems\\FileSystem.h"
#include "Components\\MeshRenderComponent.h"
#include "Components\\EditorSelectionComponent.h"

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
	//PBR_TexturedShowcase
	if (!FileSystem::Instance()->LoadSceneFromJSON("..\\Assets\\Scenes\\TEST.json", &scene, Graphics::Instance()->GetDevice(), Graphics::Instance()->GetDeviceContext()))
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
	scene.AddEntity(Graphics::Instance()->pointLight);


	// ENABLE THIS FOR PLAY MODE. Its just disabled so we can see the materials better
	player = new Player(&scene, *(new ID("Player")));

	MeshRenderer* mr = player->AddComponent<MeshRenderer>();
	mr->Initialize(player, "..\\Assets\\Objects\\GraniteRock\\Rock_LOD0.fbx", Graphics::Instance()->GetDevice(), Graphics::Instance()->GetDeviceContext(), Graphics::Instance()->GetDefaultVertexShader(), m_pMaterial);

	EditorSelection* es = player->AddComponent<EditorSelection>();
	es->Initialize(player, 10.0f, player->GetTransform().GetPosition());

	scene.AddEntity(player);

	

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
	timer.tick();
	float dt = timer.dt();
	float gamedt = timer.dt();
	
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
	
	if (m_canSave == false)
	{
		m_saveDelay -= dt;
		if (m_saveDelay <= 0.0f)
		{
			m_saveDelay = 3.0f;
			m_canSave = true;
		}
	}
	if (InputManager::Instance()->keyboard.KeyIsPressed(VK_CONTROL) && InputManager::Instance()->keyboard.KeyIsPressed('S') && m_canSave)
	{
		m_canSave = false;
		//if (!FileSystem::Instance()->WriteSceneToJSON(&scene))
		//	ErrorLogger::Log("Failed to save scene");
		//else
			//DEBUGLOG("SCENE SAVED");
			DEBUGLOG("SCENE SAVING DISABLED!");
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



