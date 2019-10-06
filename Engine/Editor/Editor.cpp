#include "Editor.h"
#include "..\\Graphics\\Graphics.h"
#include "..\\Systems\\FileSystem.h"
#include "..\\Input\\InputManager.h"

namespace Debug
{

	bool Editor::Initialize(Engine* engine)
	{
		m_engine = engine;

		std::list<Entity*>* entities = m_engine->GetScene().GetAllEntities();
		std::list<Entity*>::iterator iter;
		for (iter = entities->begin(); iter != entities->end(); iter++)
		{
			m_selectedEntity = (*iter);
			break;
		}

		return true;
	}

	void Editor::Update()
	{
		/*Raycasting*/
		//if (InputManager::Instance()->mouse.IsLeftDown())
		//{
		//	SimpleMath::Vector3 cameraPosition = Graphics::Instance()->camera3D.GetPositionFloat3();
		//	SimpleMath::Vector3 mouseVector = GetMouseDirectionVector();

		//	Ray* raycast = new Ray(cameraPosition, mouseVector);

		//	// -- Using bounding sphere -- //
		//	std::list<Entity*>* entities = m_engine->GetScene().GetAllEntities();
		//	std::list<Entity*>::iterator iter;
		//	for (iter = entities->begin(); iter != entities->end(); iter++)
		//	{
		//		if (hit_sphere((*iter)->GetComponent<EditorSelection>()->GetPosition(), (*iter)->GetComponent<EditorSelection>()->GetRadius(), *raycast))
		//		{
		//			m_selectedEntity = (*iter);
		//			break;
		//		}
		//	}
		//}

		//if (InputManager::Instance()->keyboard.KeyIsPressed(VK_CONTROL) && InputManager::Instance()->keyboard.KeyIsPressed('S'))
		//{
		//	if (!SaveScene())
		//	{
		//		ErrorLogger::Log("Failed to Save Scene");
		//	}
		//}

	}

	void Editor::Shutdown()
	{
	}

	DirectX::XMFLOAT3 Editor::GetMouseDirectionVector()
	{
		int posX = InputManager::Instance()->mouse.GetPosX();
		int posY = InputManager::Instance()->mouse.GetPosY();

		DirectX::XMVECTOR mouseNear = DirectX::XMVectorSet((float)posX, (float)posY, 0.0f, 0.0f);

		DirectX::XMVECTOR mouseFar = DirectX::XMVectorSet((float)posX, (float)posY, 1.0f, 0.0f);

		DirectX::XMVECTOR unprojectedNear = DirectX::XMVector3Unproject(mouseNear, 0, 0, (float)m_engine->GetWindowWidth(), (float)m_engine->GetWindowHeight(), Graphics::Instance()->camera3D.GetNearZ(), Graphics::Instance()->camera3D.GetFarZ(),
			Graphics::Instance()->camera3D.GetProjectionMatrix(), Graphics::Instance()->camera3D.GetViewMatrix(), DirectX::XMMatrixIdentity());

		DirectX::XMVECTOR unprojectedFar = DirectX::XMVector3Unproject(mouseFar, 0, 0, (float)m_engine->GetWindowWidth(), (float)m_engine->GetWindowHeight(), Graphics::Instance()->camera3D.GetNearZ(), Graphics::Instance()->camera3D.GetFarZ(),
			Graphics::Instance()->camera3D.GetProjectionMatrix(), Graphics::Instance()->camera3D.GetViewMatrix(), DirectX::XMMatrixIdentity());

		DirectX::XMVECTOR result = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(unprojectedFar, unprojectedNear));

		DirectX::XMFLOAT3 direction;

		DirectX::XMStoreFloat3(&direction, result);

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

	bool Editor::SaveScene()
	{
		if (!FileSystem::WriteSceneToFile(m_engine->GetScene().GetAllEntities()))
			return false;
		return true;
	}
}