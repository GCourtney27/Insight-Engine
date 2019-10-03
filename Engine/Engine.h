#pragma once
#include "WindowContainer.h"
#include "Systems\\Timer.h"
#include "Systems\\FileLoader.h"
#include "Physics\\Ray.h"
#include "Editor\\Editor.h"
#include "..\\Objects\\Scene.h"

class Engine : WindowContainer
{
public:
	Engine() {}
	~Engine() {}
	bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	bool ProccessMessages();
	void Update();
	void RenderFrame();
	bool SaveScene();
	
private:
	
	Timer timer;
	Editor editor; // Move ray casting into editor

	Scene scene;

	int windowWidth = 0;
	int windowHeight = 0;

};