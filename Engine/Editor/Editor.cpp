#include "Editor.h"

bool Editor::Initialize()
{

	return false;
}

void Editor::Update()
{
	/*Raycasting*/
	if (mouse.IsLeftDown())
	{
		/*SimpleMath::Vector3 cameraPosition = gfx.camera3D.GetPositionFloat3();
		SimpleMath::Vector3 mouseVector = GetMouseDirectionVector();

		Ray* raycast = new Ray(cameraPosition, mouseVector);*/

		// -- Using numerical radius -- //
		/*int gos = (int)gfx.m_gameObjects.size();
		for (int i = 0; i < gos; i++)
		{
			if (hit_sphere(gfx.m_gameObjects[i]->sphere_position, gfx.m_gameObjects[i]->sphere_radius, *raycast))
			{
				gfx.selectedGameObject = gfx.m_gameObjects[i];
				break;
			}
		}*/
		// -- Using bounding sphere -- //
		/*int gos = (int)gfx.m_gameObjects.size();
		for (int i = 0; i < gos; i++)
		{
			if (hit_sphere(gfx.m_gameObjects[i]->aabb.GetPosition(), gfx.m_gameObjects[i]->aabb.GetRadius(), *raycast))
			{
				gfx.selectedGameObject = gfx.m_gameObjects[i];
				break;
			}
		}*/
	}
}

void Editor::Shutdown()
{
}

DirectX::XMFLOAT3 Editor::GetMouseDirectionVector()
{
	/*int posX = mouse.GetPosX();
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

	return direction; */
	DirectX::XMFLOAT3 direction;
	return direction;
}


bool Editor::hit_sphere(const SimpleMath::Vector3& center, float radius, const Ray& r)
{
	SimpleMath::Vector3 oc = r.orgin() - center;

	float a = r.direction().Dot(r.direction());
	float b = 2.0f * oc.Dot(r.direction());
	float c = oc.Dot(oc) - radius * radius;
	float discriminant = b * b - 4 * a*c;
	return (discriminant > 0.0f);
}

float Editor::intersection_distance(const SimpleMath::Vector3& center, float radius, const Ray& r)
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