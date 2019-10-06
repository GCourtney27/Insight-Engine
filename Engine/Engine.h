#pragma once
#include "WindowContainer.h"
#include "..\\Objects\\Scene.h"
#include "Scriptor\\Compiler.h"
#include "Objects\\Entity.h"

class Engine : WindowContainer
{
public:
	Engine() {}
	~Engine() {}
	bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	bool ProccessMessages();
	void Update();
	void Shutdown();
	void RenderFrame();
	
	Scene& GetScene() { return scene; }

	int GetWindowWidth() { return windowWidth; }
	int GetWindowHeight() { return windowHeight; }

	Compiler* GetCompiler() { return &compiler; }

private:
	
	// DEBUG
	Entity* scriptedEntity = nullptr;

	Scene scene;
	Compiler compiler;

	int windowWidth = 0;
	int windowHeight = 0;

};