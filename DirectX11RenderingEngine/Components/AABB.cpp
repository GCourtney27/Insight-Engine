#include "AABB.h"

void AABB::Update(const DirectX::XMFLOAT3 parentPosition)
{
	this->m_position = parentPosition;
}
