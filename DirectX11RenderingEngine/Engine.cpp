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

		/*Raycasting*/
		if (mouse.IsLeftDown())
		{
			SimpleMath::Vector3 cameraPosition = gfx.camera3D.GetPositionFloat3();
			SimpleMath::Vector3 mouseVector = GetMouseDirectionVector();
			
			Ray* raycast = new Ray(cameraPosition, mouseVector);

			/*int gos = (int)gfx.m_gameObjects.size();
			for (int i = 0; i < gos; i++)
			{
				if (hit_sphere(gfx.m_gameObjects[i]->sphere_position, gfx.m_gameObjects[i]->sphere_radius, *raycast))
				{
					gfx.selectedGameObject = gfx.m_gameObjects[i];
					break;
				}
			}*/
			int gos = (int)gfx.m_gameObjects.size();
			for (int i = 0; i < gos; i++)
			{
				if (hit_sphere(gfx.m_gameObjects[i]->aabb.GetPosition(), gfx.m_gameObjects[i]->aabb.GetRadius(), *raycast))
				{
					gfx.selectedGameObject = gfx.m_gameObjects[i];
					break;
				}
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
		//this->gfx.light.SetRotation(this->gfx.camera3D.GetRotationFloat3());
	}

	if (keyboard.KeyIsPressed(VK_CONTROL) && keyboard.KeyIsPressed('S'))
	{
		if(!SaveScene())
			ErrorLogger::Log("Failed to save scene");
	}

	for (int i = 0; i < gfx.m_gameObjects.size();i++)
	{
		gfx.m_gameObjects[i]->Update();
	}
}

DirectX::XMFLOAT4X4 boxWorld;

void Engine::RenderFrame()
{
	gfx.RenderFrame();
}

bool Engine::SaveScene()
{
	if (!FileLoader::WriteSceneToFile(gfx.m_gameObjects))
		return false;
	return true;
}

bool Engine::hit_sphere(const SimpleMath::Vector3& center, float radius, const Ray& r)
{
	SimpleMath::Vector3 oc = r.orgin() - center;

	float a = r.direction().Dot(r.direction());
	float b = 2.0f * oc.Dot(r.direction());
	float c = oc.Dot(oc) - radius * radius;
	float discriminant = b * b - 4 * a*c;
	return (discriminant > 0.0f);
}

float Engine::intersection_distance(const SimpleMath::Vector3& center, float radius, const Ray& r)
{
	SimpleMath::Vector3 oc = r.orgin() - center;

	float a = r.direction().Dot(r.direction());
	float b = 2.0f * oc.Dot(r.direction());
	float c = oc.Dot(oc) - radius * radius;
	float discriminant = b * b - 4 * a*c;
	if (discriminant < 0) {
		return -1.0f;
	}
	else {
		return (-b - sqrt(discriminant)) / (2.0f*a);
	}
}

DirectX::XMFLOAT3 Engine::GetMouseDirectionVector()
{
	int posX = mouse.GetPosX();
	int posY = mouse.GetPosY();

	DirectX::XMVECTOR mouseNear = DirectX::XMVectorSet((float)posX, (float)posY, 0.0f, 0.0f);

	DirectX::XMVECTOR mouseFar = DirectX::XMVectorSet((float)posX, (float)posY, 1.0f, 0.0f);

	DirectX::XMVECTOR unprojectedNear = DirectX::XMVector3Unproject(mouseNear, 0, 0, (float)windowWidth, (float)windowHeight, gfx.camera3D.GetNearZ(), gfx.camera3D.GetFarZ(),
		gfx.camera3D.GetProjectionMatrix(), gfx.camera3D.GetViewMatrix(), DirectX::XMMatrixIdentity());

	DirectX::XMVECTOR unprojectedFar = DirectX::XMVector3Unproject(mouseFar, 0, 0, (float)windowWidth, (float)windowHeight, gfx.camera3D.GetNearZ(), gfx.camera3D.GetFarZ(),
		gfx.camera3D.GetProjectionMatrix(), gfx.camera3D.GetViewMatrix(), DirectX::XMMatrixIdentity());

	DirectX::XMVECTOR result = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(unprojectedFar, unprojectedNear));

	DirectX::XMFLOAT3 direction;

	DirectX::XMStoreFloat3(&direction, result);

	return direction; // b = this stuff a = camera world position

	////Get the distance to the ground.
	//DirectX::DirectX::XMVECTOR cameracamPosition = gfx.camera3D.GetPositionFloat3();

	////Get the point on the ground.
	//cameracamPosition.x += direction.x * (cameracamPosition.y / -direction.y);
	//cameracamPosition.z += direction.z * (cameracamPosition.y / -direction.y);

}
