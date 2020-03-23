#pragma once
#include "Entity.h"

class Camera : public Entity
{
public:
	Camera();
	Camera(Scene* scene, const ID& id)
		: Entity(scene, id) 
	{
		this->m_scene = scene;
		this->m_id = id;

		// Set defaults for transform, needed for view matrix construct
		m_transform.SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_transform.SetPosition(XMLoadFloat3(&m_transform.GetPosition()));
		m_transform.SetRotation(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_transform.SetRotation(XMLoadFloat3(&m_transform.GetRotation()));

		this->UpdateViewMatrix();
	}

	bool Initialize(Scene* scene, const ID& id);
	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

	void Draw(const XMMATRIX & viewProjectionMatrix, const XMMATRIX & viewMatrix) override;
	void Update(const float& deltaTime) override;

	const XMMATRIX & GetViewMatrix();
	const XMMATRIX & GetProjectionMatrix();

	float GetNearZ() const { return m_nearZ; }
	float GetFarZ()const { return m_farZ; }

private:
	void UpdateViewMatrix();

	XMMATRIX worldMat;

	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;

	float m_nearZ = 0.0f;
	float m_farZ = 0.0f;

	bool isOrthographic = false;
};