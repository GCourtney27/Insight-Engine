#include "Engine.h"
#include "Editor\\Editor.h"
#include "Graphics\\Graphics.h"
#include "..\\Systems\\FileSystem.h"
#include "..\Systems\BenchmarkingTimer.h"

bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	windowHeight = height;
	windowWidth = width;
	
	Debug::ScopedTimer timerEngineStart;
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

	if (!LuaStateManager::GetStateManager()->Create())
	{
		ErrorLogger::Log("Failed to initialize Lua State Manager");
		return false;
	}

	// ============= Choose from these scenes
	// PBR_TexturedShowcase
	// PBR_UnTexturedShowcase
	// Norway - Disable physics for this level!
	// Test
	// PhysicsTest
	// Physics_Showcase
	// Water
	// AdvancedModels
	// Game
	scene.SetPhysicsEnabled(false);
	if (!FileSystem::Instance()->LoadSceneFromJSON("..\\Assets\\Scenes\\MySceneName\\World_Backup.json", &scene, Graphics::Instance()->GetDevice(), Graphics::Instance()->GetDeviceContext()))
	{
		ErrorLogger::Log("Failed to initialize scene.");
		return false;
	}

	
	// Enable this to draw the lights mesh (Commented does not effect the lights emission behavior)
	//scene.AddEntity(Graphics::Instance()->pointLight);
	//scene.AddEntity(Graphics::Instance()->directionalLight);

	if (!scene.Initialize(this))
	{ 
		ErrorLogger::Log("Failed to initialize scene");
		return false;
	}

	if (!Debug::Editor::Instance()->Initialize(this, this->render_window.GetHWND()))
	{
		ErrorLogger::Log("Failed to initialize Editor Instance.");
		return false;
	}
	timerEngineStart.Stop();
	Debug::Editor::Instance()->DebugLog("Engine initialized in " + std::to_string(timerEngineStart.GetTimeInSeconds()) + " seconds");

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

	/*while (!InputManager::Instance()->keyboard.CharBufferIsEmpty())
	{
		unsigned char ch = InputManager::Instance()->keyboard.ReadChar();
	}*/

	while (!InputManager::Instance()->keyboard.KeyBufferIsEmpty())
	{
		KeyboardEvent kbe = InputManager::Instance()->keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();

		if (kbe.IsRealesed())
		{
			// Let lua know that the button with this keycode has been released
		}
	}

	while (!InputManager::Instance()->mouse.EventBufferIsEmpty())
	{
		MouseEvent me = InputManager::Instance()->mouse.ReadEvent();

		if (InputManager::Instance()->mouse.IsRightDown() && !Debug::Editor::Instance()->PlayingGame())
		{
			if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
			{
				Graphics::Instance()->editorCamera.GetTransform().AdjustRotation((float)me.GetPosY() * 0.005f, (float)me.GetPosX() * 0.005f, 0.0f);
			}

		}

		if (me.GetType() == MouseEvent::EventType::LRelease)
			Debug::Editor::Instance()->rayCastEnabled = true;

		if (Debug::Editor::Instance()->PlayingGame())
		{
			if (InputManager::Instance()->mouse.IsRightDown())
			{
				if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
				{
					InputManager::Instance()->MouseMoved(true);
					InputManager::Instance()->SetMouseX((float)me.GetPosX());
					InputManager::Instance()->SetMouseY((float)me.GetPosY());
				}
				else
				{
					InputManager::Instance()->MouseMoved(false);
				}
			}
			else
			{
				InputManager::Instance()->SetMouseX(0.0f);
				InputManager::Instance()->SetMouseY(0.0f);
			}
			
		}

	}
	
	if (InputManager::Instance()->keyboard.KeyIsPressed('V'))
	{
		float px = Graphics::Instance()->editorCamera.GetTransform().GetPosition().x;
		float py = Graphics::Instance()->editorCamera.GetTransform().GetPosition().y;
		float pz = Graphics::Instance()->editorCamera.GetTransform().GetPosition().z;

		float rx = Graphics::Instance()->editorCamera.GetTransform().GetRotation().x;
		float ry = Graphics::Instance()->editorCamera.GetTransform().GetRotation().y;
		float rz = Graphics::Instance()->editorCamera.GetTransform().GetRotation().z;

		m_pPlayer->GetPlayerCamera()->GetTransform().SetPosition(px, py, pz);
		m_pPlayer->GetPlayerCamera()->GetTransform().SetRotation(rx, ry, rz);
	}


	if (InputManager::Instance()->keyboard.KeyIsPressed(VK_F5))
		Debug::Editor::Instance()->PlayGame();

	scene.Update(dt);

	if (Debug::Editor::Instance()->PlayingGame())
		scene.OnUpdate(gamedt);

	Graphics::Instance()->Update(dt);
	Debug::Editor::Instance()->Update(dt);


	// FORCE ENGINE CLOSE (Space + Esc)
	if (InputManager::Instance()->keyboard.KeyIsPressed(27) && InputManager::Instance()->keyboard.KeyIsPressed(' '))
	{
		exit(0); // Performs no cleanup
		PostMessage(this->render_window.GetHWND(), WM_QUIT, 0, 0);
	}

	if (InputManager::Instance()->keyboard.KeyIsPressed('C'))
	{
		Graphics::Instance()->pointLight->GetTransform().SetPosition(Graphics::Instance()->editorCamera.GetTransform().GetPositionFloat3());
		Graphics::Instance()->pointLight->GetTransform().SetRotation(Graphics::Instance()->editorCamera.GetTransform().GetRotationFloat3());
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
		if (!FileSystem::Instance()->WriteSceneToJSON(&scene))
			ErrorLogger::Log("Failed to save scene");
		else
			DEBUGLOG("SCENE SAVED");
			//DEBUGLOG("SCENE SAVING DISABLED!");
	}

	if (m_canChangeScene == false)
	{
		m_delay -= dt;
		if (m_delay <= 0.0f)
		{
			m_delay = 3.0f;
			m_canChangeScene = true;
		}
	}
	if (InputManager::Instance()->keyboard.KeyIsPressed('O') && m_canChangeScene)
	{
		m_canChangeScene = false;
		scene.Flush();

		scene.SetPhysicsEnabled(false);
		if (!FileSystem::Instance()->LoadSceneFromJSON("..\\Assets\\Scenes\\NewScene\\World.json", &scene, Graphics::Instance()->GetDevice(), Graphics::Instance()->GetDeviceContext()))
			ErrorLogger::Log("Failed to initialize scene.");

		std::list<Entity*>* entities = scene.GetAllEntities();
		std::list<Entity*>::iterator iter;
		int i = 0;
		for (iter = entities->begin(); iter != entities->end(); iter++)
		{
			Debug::Editor::Instance()->SetSelectedEntity(*iter);
			i++;
			if (i == 3)
				break;
		}
	}
}

void Engine::RenderFrame()
{
	Graphics::Instance()->RenderFrame();
}

void Engine::Shutdown()
{
	scene.Shutdown();
	//Graphics::Instance()->Shutdown();
}

void Engine::OnGameStart()
{
	scene.OnStart();
}



