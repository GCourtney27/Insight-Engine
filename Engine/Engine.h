#pragma once
#include "WindowContainer.h"
#include "..\\Objects\\Scene.h"
#include "Scriptor\\Compiler.h"
#include "Objects\\Entity.h"
#include "Objects\\Player.h"
#include "LUAScripting\\LuaStateManager.h"
#include "Objects/PointLight.h"

#include "Systems/TimeSystem.h"

class Engine : WindowContainer
{
public:
	Engine() {}
	~Engine() {}

	// -- Engine -- //
	bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	bool ProccessMessages();
	void Update();
	void RenderFrame();
	void Shutdown();
	
	// -- Game -- //
	void OnGameStart();

	Scene& GetScene() { return scene; }

	int GetWindowWidth() { return windowWidth; }
	int GetWindowHeight() { return windowHeight; }

	Player* GetPlayer() { return player; }
private:

	frame_timer timer;

	Player* player = nullptr;
	std::vector<std::string> textures;
	Material* m_pMaterial = nullptr;

	Scene scene;

	int windowWidth = 0;
	int windowHeight = 0;

	float m_saveDelay = 3.0f;
	bool m_canSave = true;
};