#include "AABB.h"

void AABB::Initialize(float radius, const DirectX::XMFLOAT3 & position)
{
	m_position = position;
	m_radius = radius;
}

void AABB::Update(const DirectX::XMFLOAT3 parentPosition)
{
	this->m_position = parentPosition;
}
