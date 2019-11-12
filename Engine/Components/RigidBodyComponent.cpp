#include "RigidBodyComponent.h"
#include "..\Graphics\ImGui\imgui.h"
#include "..\Objects\Entity.h"
#include <string>

void RigidBody::InitFromJSON(Entity * owner, const rapidjson::Value & componentInformation)
{
	float radius = 0.0f;
	std::string colliderType;

	json::get_string(componentInformation[0], "ColiderType", colliderType);
	json::get_float(componentInformation[0], "Radius", radius);

	m_sphereRadius = radius;
	m_colliderType = GetColliderTypeFromString(colliderType);

	this->Initialize(owner);
}

bool RigidBody::Initialize(Entity* owner)
{
	SetName("Rigid Body");

	return true;
}

void RigidBody::WriteToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.Key("RigidBody");
	writer.StartArray();// Start Component

	writer.StartObject();

	writer.Key("ColliderType");
	writer.String(this->GetGetColliderTypeAsString().c_str());

	writer.Key("Radius");
	writer.Double(m_sphereRadius);

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
	
	m_position = m_owner->GetTransform().GetPosition();
}

void RigidBody::OnImGuiRender()
{
	ImGui::Text(GetName());
	static bool isRBEnabled = this->GetIsComponentEnabled();
	if (ImGui::Checkbox("Is Physics Enabled", &isRBEnabled))
	{
		this->SetComponentEnabled(isRBEnabled);
	}
	ImGui::Text("Colider Type: ");
	ImGui::SameLine();
	ImGui::Text(GetGetColliderTypeAsString().c_str());

	ImGui::DragFloat("Radius", &m_sphereRadius, 0.01f, 0.0f, 100.0f);

}

RigidBody::eColliderType RigidBody::GetColliderTypeFromString(std::string str_collider)
{
	std::map<std::string, eColliderType> stringToColliderType;

	stringToColliderType["SPHERE"] = eColliderType::SPHERE;

	std::map<std::string, eColliderType>::iterator iter;
	for (iter = stringToColliderType.begin(); iter != stringToColliderType.end(); iter++)
	{
		if ((*iter).first == str_collider)
			return (*iter).second;
	}
	return eColliderType::SPHERE;
}

std::string RigidBody::GetGetColliderTypeAsString()
{
	std::map<std::string, eColliderType> stringToColliderType;
	stringToColliderType["SPHERE"] = eColliderType::SPHERE;

	std::map<std::string, eColliderType>::iterator iter;
	for (iter = stringToColliderType.begin(); iter != stringToColliderType.end(); iter++)
	{
		if ((*iter).second == this->m_colliderType)
			return (*iter).first;
	}

	return "ERROR: Could not determine collider type";
}
