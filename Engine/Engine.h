#pragma once
#include "WindowContainer.h"
#include "..\\Objects\\Scene.h"
#include "Scriptor\\Compiler.h"
#include "Objects\\Entity.h"
#include "LUAScripting/LuaStateManager.h"

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


private:

	// DEBUG
	Entity* entity = nullptr;

	Scene scene;

	int windowWidth = 0;
	int windowHeight = 0;

};