#include "Engine.h"
#include <iostream>


bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	windowHeight = height;
	windowWidth = width;

	timer.Start();
	if (!this->render_window.Initialize(this, hInstance, window_title, window_class, width, height))
		return false;

	// Forward Renderer
	if (!gfx.Initialize(this->render_window.GetHWND(), width, height))
		return false;

	if (!FileLoader::LoadSceneFromFile("Data\\Scenes\\Scene01.txt", &scene, gfx.GetDevice(), gfx.GetDeviceContext(), gfx.GetDefaultVertexShader()))
	{
		ErrorLogger::Log("Failed to initialize scene.");
		return false;
	}

	gfx.SetScene(&scene);

	return true;
}

bool Engine::ProccessMessages()
{
	return this->render_window.ProccessMessages();
}

void Engine::Update()
{
	float dt = (float)timer.GetMilisecondsElapsed();
	timer.Restart();

	gfx.Update();
	//scene.Update(dt);

	while (!keyboard.CharBufferIsEmpty())
	{
		unsigned char ch = keyboard.ReadChar();
	}

	while (!keyboard.KeyBufferIsEmpty())
	{
		KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
	}

	while (!mouse.EventBufferIsEmpty())
	{
		MouseEvent me = mouse.ReadEvent();
		if (mouse.IsRightDown())
		{
			if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
			{
				this->gfx.camera3D.AdjustRotation((float)me.GetPosY() * 0.01f, (float)me.GetPosX() * 0.01f, 0);
			}
		}

	}

	float camera3DSpeed = 0.01f;
	if (keyboard.KeyIsPressed(VK_SHIFT))
	{
		camera3DSpeed = 0.1f;
	}


	if (keyboard.KeyIsPressed('W'))
	{
		this->gfx.camera3D.AdjustPosition(this->gfx.camera3D.GetForwardVector() * camera3DSpeed * dt);
	}
	if (keyboard.KeyIsPressed('S'))
	{
		this->gfx.camera3D.AdjustPosition(this->gfx.camera3D.GetBackwardVector() * camera3DSpeed * dt);
	}
	if (keyboard.KeyIsPressed('A'))
	{
		this->gfx.camera3D.AdjustPosition(this->gfx.camera3D.GetLeftVector() * camera3DSpeed * dt);
	}
	if (keyboard.KeyIsPressed('D'))
	{
		this->gfx.camera3D.AdjustPosition(this->gfx.camera3D.GetRightVector() * camera3DSpeed * dt);
	}
	if (keyboard.KeyIsPressed('E'))
	{
		this->gfx.camera3D.AdjustPosition(0.0f, camera3DSpeed * dt, 0.0f);
	}
	if (keyboard.KeyIsPressed('Q'))
	{
		this->gfx.camera3D.AdjustPosition(0.0f, -camera3DSpeed * dt, 0.0f);
	}
	if (keyboard.KeyIsPressed(27))
	{
		exit(0); // Performs no cleanup
		//PostMessage(this->render_window.GetHWND(), WM_QUIT, 0, 0);
	}

	if (keyboard.KeyIsPressed('C'))
	{
		//lightPosition += this->gfx.camera3D.GetForwardVector();
		this->gfx.light.SetPosition(this->gfx.camera3D.GetPositionFloat3());
		this->gfx.light.SetRotation(this->gfx.camera3D.GetRotationFloat3());
	}

	if (keyboard.KeyIsPressed(VK_CONTROL) && keyboard.KeyIsPressed('S'))
	{
		if(!SaveScene())
			ErrorLogger::Log("Failed to save scene");
	}

	/*for (int i = 0; i < gfx.m_gameObjects.size();i++)
	{
		gfx.m_gameObjects[i]->Update();
	}*/

}

DirectX::XMFLOAT4X4 boxWorld;

void Engine::RenderFrame()
{
	gfx.RenderFrame();
}

bool Engine::SaveScene()
{
	/*if (!FileLoader::WriteSceneToFile(gfx.m_gameObjects))
		return false;*/
	return true;
}



