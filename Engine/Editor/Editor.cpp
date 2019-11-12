#include "Editor.h"
#include "..\\Graphics\\Graphics.h"
#include "..\\Systems\\FileSystem.h"
#include "..\\Input\\InputManager.h"

namespace Debug
{

	bool Editor::Initialize(Engine* engine, HWND hwnd)
	{
		m_pEngine = engine;

		std::list<Entity*>* entities = m_pEngine->GetScene().GetAllEntities();
		std::list<Entity*>::iterator iter;
		for (iter = entities->begin(); iter != entities->end(); iter++)
		{
			m_pSelectedEntity = (*iter);
			break;
		}

		m_debugLog = "";

		return true;
	}

	void Editor::Update(float deltaTime)
	{
		if (!m_playingGame)
		{
			float camera3DSpeed = 30.0f;
			if (InputManager::Instance()->keyboard.KeyIsPressed(VK_SHIFT))
			{
				camera3DSpeed = 70.0f;
			}

			if (InputManager::Instance()->keyboard.KeyIsPressed('W'))
			{
				Graphics::Instance()->editorCamera.GetTransform().AdjustPosition(Graphics::Instance()->editorCamera.GetTransform().GetForwardVector() * camera3DSpeed * deltaTime);
			}
			if (InputManager::Instance()->keyboard.KeyIsPressed('S'))
			{
				Graphics::Instance()->editorCamera.GetTransform().AdjustPosition(Graphics::Instance()->editorCamera.GetTransform().GetBackwardVector() * camera3DSpeed * deltaTime);
			}
			if (InputManager::Instance()->keyboard.KeyIsPressed('A'))
			{
				Graphics::Instance()->editorCamera.GetTransform().AdjustPosition(Graphics::Instance()->editorCamera.GetTransform().GetLeftVector() * camera3DSpeed * deltaTime);
			}
			if (InputManager::Instance()->keyboard.KeyIsPressed('D'))
			{
				Graphics::Instance()->editorCamera.GetTransform().AdjustPosition(Graphics::Instance()->editorCamera.GetTransform().GetRightVector() * camera3DSpeed * deltaTime);
			}
			if (InputManager::Instance()->keyboard.KeyIsPressed('E'))
			{
				Graphics::Instance()->editorCamera.GetTransform().AdjustPosition(0.0f, camera3DSpeed * deltaTime, 0.0f);
			}
			if (InputManager::Instance()->keyboard.KeyIsPressed('Q'))
			{
				Graphics::Instance()->editorCamera.GetTransform().AdjustPosition(0.0f, -camera3DSpeed * deltaTime, 0.0f);
			}
		}
		

		/*Raycasting*/
		if (InputManager::Instance()->mouse.IsLeftDown())
		{
			SimpleMath::Vector3 cameraPosition = Graphics::Instance()->editorCamera.GetTransform().GetPosition();
			SimpleMath::Vector3 mouseVector = GetMouseDirectionVector();
			Ray* raycast = new Ray(cameraPosition, mouseVector);
			
			// -- Using bounding sphere -- //
			std::list<Entity*>* entities = m_pEngine->GetScene().GetAllEntities();
			std::list<Entity*>::iterator iter;
			for (iter = entities->begin(); iter != entities->end(); iter++)
			{
				if (hit_sphere((*iter)->GetComponent<EditorSelection>()->GetPosition(), (*iter)->GetComponent<EditorSelection>()->GetRadius(), *raycast))
				{
					m_pSelectedEntity = (*iter);
					break;
				}
			}
		}



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

		DirectX::XMVECTOR unprojectedNear = DirectX::XMVector3Unproject(mouseNear, 0, 0, (float)m_pEngine->GetWindowWidth(), (float)m_pEngine->GetWindowHeight(), Graphics::Instance()->editorCamera.GetNearZ(), Graphics::Instance()->editorCamera.GetFarZ(),
			Graphics::Instance()->editorCamera.GetProjectionMatrix(), Graphics::Instance()->editorCamera.GetViewMatrix(), DirectX::XMMatrixIdentity());

		DirectX::XMVECTOR unprojectedFar = DirectX::XMVector3Unproject(mouseFar, 0, 0, (float)m_pEngine->GetWindowWidth(), (float)m_pEngine->GetWindowHeight(), Graphics::Instance()->editorCamera.GetNearZ(), Graphics::Instance()->editorCamera.GetFarZ(),
			Graphics::Instance()->editorCamera.GetProjectionMatrix(), Graphics::Instance()->editorCamera.GetViewMatrix(), DirectX::XMMatrixIdentity());

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

}