#pragma once
#include "Component.h"
#include <DirectXMath.h>

class AABB
{
public:
	AABB(float radius, const DirectX::XMFLOAT3 & position) : m_position(position), m_radius(radius) {}

	void SetRadius(float value) { m_radius = value; }
	void SetPosition(const DirectX::XMFLOAT3 & position) { m_position = position; }

	float GetRadius() const { return m_radius; }
	DirectX::XMFLOAT3 & GetPosition() { return m_position; }

	void Update(const DirectX::XMFLOAT3 parentPosition);

private:
	DirectX::XMFLOAT3 m_position;
	float m_radius = 0.0f;
};