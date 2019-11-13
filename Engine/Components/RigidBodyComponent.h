#pragma once
#include "Component.h"
#include "..\Physics\AABB.h"
#include <map>
#include <DirectXMath.h>

class RigidBody : public Component
{


public:
	RigidBody(Entity* owner)
		: Component(owner) {}

	bool Initialize(Entity* owner);
	bool Initialize(Entity* owner, float radius, AABB::eColliderType colliderType);

	virtual void InitFromJSON(Entity* owner, const rapidjson::Value& componentInformation) override;
	virtual void WriteToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;

	virtual void Start() override;
	virtual void Update(const float& deltaTime) override;
	virtual void Destroy() override;
	virtual void OnImGuiRender() override;
	
	// Translates the object and updates velocity
	void Translate(float x, float y, float z);

	AABB& GetCollider() { return m_collider; }

private:
	AABB m_collider;

	float m_velocity = 0.0f;
	float m_drag = 0.0f;

};