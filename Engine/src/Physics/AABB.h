#pragma once
#include "..\Objects\Entity.h"

#include <string>
#include <DirectXMath.h>

class AABB
{
public:
	enum eColliderType
	{
		NONE,
		SPHERE,
		BOX
	};

public:
	AABB() {}

	bool Initialize(Entity* owner);
	bool Initialize(Entity* owner, float radius, eColliderType colliderType);
	void Update(const float& deltaTime);

	bool Intersects(AABB* collider);

	eColliderType GetColliderTypeFromString(std::string str_collider);
	std::string GetGetColliderTypeAsString();
	eColliderType GetColliderType() { return m_colliderType; }

	float& GetRadius() { return m_sphereRadius; }
	void SetRadius(float radius) { m_sphereRadius = radius; }

	Entity* GetOwner() { return m_owner; }

	bool InitFromJSON(const rapidjson::Value& componentInformation);
	void WriteToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	void OnImGuiRender();

private:
	bool SphereIntersects(AABB& collider);

private:
	eColliderType m_colliderType = eColliderType::SPHERE;
	DirectX::XMFLOAT3 m_position;

	float m_sphereRadius = 0.0f;

	float m_width = 0.0f;
	float m_height = 0.0f;
	float m_depth = 0.0f;

	Entity* m_owner = nullptr;
};