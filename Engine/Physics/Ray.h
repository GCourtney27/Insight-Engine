#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

using namespace DirectX;

class Ray {
public:
	Ray() {}
	Ray(const SimpleMath::Vector3& position, const SimpleMath::Vector3& direction) { m_position = position; m_direction = direction; }
	SimpleMath::Vector3 orgin() const { return m_position; }
	SimpleMath::Vector3 direction() const { return m_direction; }
	SimpleMath::Vector3 point_at_parameter(float t) const { return m_position + t * m_direction; }

	SimpleMath::Vector3 m_position;
	SimpleMath::Vector3 m_direction;
};