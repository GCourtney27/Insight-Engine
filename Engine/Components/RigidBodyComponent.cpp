#include "RigidBodyComponent.h"
#include "..\Graphics\ImGui\imgui.h"
#include "..\Objects\Entity.h"
#include <string>

void RigidBody::InitFromJSON(Entity * owner, const rapidjson::Value & componentInformation)
{
	float radius = 0.0f;
	std::string colliderType;

	const rapidjson::Value & aabb = componentInformation[0]["AABB"];
	m_collider.InitFromJSON(aabb);


	this->Initialize(owner);
}

bool RigidBody::Initialize(Entity* owner)
{
	SetName("Rigid Body");

	m_collider.Initialize(owner);

	return true;
}

bool RigidBody::Initialize(Entity * owner, float radius, AABB::eColliderType colliderType)
{
	SetName("Rigid Body");

	m_collider.Initialize(owner, 10.0f, AABB::eColliderType::SPHERE);

	return true;
}

void RigidBody::WriteToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.Key("RigidBody");
	writer.StartArray();// Start Component

		writer.StartObject();
			m_collider.WriteToJSON(writer);
		writer.EndObject();

	writer.EndArray();//End Component
}

void RigidBody::Destroy()
{
}

void RigidBody::Start()
{

}

void RigidBody::Update(const float & deltaTime)
{
	m_collider.Update(deltaTime);

	//DirectX::XMFLOAT3 newPos = m_owner->GetTransform().GetPosition();
	//newPos += m_velocity;
	//m_owner->GetTransform().GetPosition() = m_owner->GetTransform().GetPosition() + m_velocity;

}

void RigidBody::OnImGuiRender()
{
	ImGui::Text(GetName());
	static bool isRBEnabled = this->GetIsComponentEnabled();
	if (ImGui::Checkbox("Is Physics Enabled", &isRBEnabled))
		this->SetComponentEnabled(isRBEnabled);

	m_collider.OnImGuiRender();

}

void RigidBody::Translate(float x, float y, float z)
{
}


