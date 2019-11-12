#pragma once
#include "Component.h"
#include <map>
#include <DirectXMath.h>

class RigidBody : public Component
{
public:
	enum eColliderType
	{
		NONE,
		SPHERE
	};

public:
	RigidBody(Entity* owner)
		: Component(owner) {}

	bool Initialize(Entity* owner);

	virtual void InitFromJSON(Entity* owner, const rapidjson::Value& componentInformation) override;
	virtual void WriteToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;

	virtual void Start() override;
	virtual void Update(const float& deltaTime) override;
	virtual void Destroy() override;
	virtual void OnImGuiRender() override;
	
	float& GetRadius() { return m_sphereRadius; }
	void SetRadius(float radius) { m_sphereRadius = radius; }

	static eColliderType GetColliderTypeFromString(std::string str_collider);
	std::string GetGetColliderTypeAsString();
	eColliderType GetColliderType() { return m_colliderType; }

private:
	float m_sphereRadius = 0.0f;
	eColliderType m_colliderType = eColliderType::SPHERE;
	DirectX::XMFLOAT3 m_position;
};