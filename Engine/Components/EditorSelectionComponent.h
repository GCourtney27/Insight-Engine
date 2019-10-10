#pragma once
#include "Component.h"
#include <DirectXMath.h>
#include <d3d11.h >
#include <SimpleMath.h>

class EditorSelection : public Component
{
public:
	EditorSelection(Entity* owner)
		: Component(owner) {}
	
	void Initialize(Entity* owner, float radius, const DirectX::XMFLOAT3 & position);

	void SetRadius(float value) { m_radius = value; }
	void SetPosition(const DirectX::XMFLOAT3 & position) { m_position = position; }

	float GetRadius() const { return m_radius; }
	DirectX::XMFLOAT3 & GetPosition() { return m_position; }

	void Update() override;
	void Destroy() override;
	//void Update(const DirectX::XMFLOAT3 parentPosition);
	void OnImGuiRender() override;


private:
	float m_radius = 0.0f;
	DirectX::SimpleMath::Vector3 m_position;
};