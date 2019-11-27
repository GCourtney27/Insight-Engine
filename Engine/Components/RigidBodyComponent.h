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
	void OnEditorStop() override;

	// Translates the object and updates velocity
	void Translate(float x, float y, float z);

	AABB& GetCollider() { return m_collider; }

	void SetIsColliding(bool value)
	{
		m_colliding = value; 
	}
	
	bool& IsColliding() 
	{
		return m_colliding; 
	}

	void ResetForce()
	{
		m_force.x = m_force.y = m_force.z = 0.0f;
	}

	void SetForce(DirectX::XMFLOAT3 force)
	{
		m_force = force;
	}

	void SetVelocity(XMFLOAT3 vel) { m_velocity = vel; }
	XMFLOAT3& GetVelocity() { return m_velocity; }
	float& GetRestitution() { return m_restitution; }
	float& GetInverseMass() { return m_inverseMass; }
	float& GetMass() { return m_mass; }

	void SetColliderTag(int tag) { m_colliderTag = tag; }
	int& GetColliderTag() { return m_colliderTag; }

	bool IsCollidingWith(int colliderTag);
	void RecordCollisionByTag(int colliderTag) { m_colliders.emplace(colliderTag, true); }

	void ClearColliders() { m_colliders.clear(); }

	bool GetIsTrigger() const { return m_isTrigger; }
	void SetIsTrigger(bool value) { m_isTrigger = value; }

	void RecordCollidingObject(RigidBody* collider) { m_collidingObjects.push_back(collider); }
	void ClearCollidingObjects() { m_collidingObjects.clear(); }

private:
	AABB m_collider;

	int m_colliderTag = 0;

	float m_mass = 5.0f;
	float m_inverseMass = 1.0f / m_mass;
	float m_restitution = 1.0f;
	DirectX::XMFLOAT3 m_force = {};
	DirectX::XMFLOAT3 m_velocity = {};
	DirectX::XMFLOAT3 m_deltaPos = {};
	float m_drag = 0.0f;

	bool m_static = 0;
	bool m_gravityEnable = true;
	bool m_colliding = false;

	bool m_isTrigger = false;

	std::map<int, bool> m_colliders;

	std::vector<RigidBody*> m_collidingObjects;
};