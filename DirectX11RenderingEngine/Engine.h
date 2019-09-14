#pragma once
#include "WindowContainer.h"
#include "Timer.h"
#include "FileLoader.h"
#include "Graphics/Ray.h"
#include "Editor.h"

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

	bool hit_sphere(const SimpleMath::Vector3& center, float radius, const Ray& r);
	float intersection_distance(const SimpleMath::Vector3& center, float radius, const Ray& r);

	DirectX::XMFLOAT3 GetMouseDirectionVector();

private:
	Timer timer;
	Editor editor;
	int windowWidth = 0;
	int windowHeight = 0;

	// DEFERRED RENDER
	//DebugModels models;
};