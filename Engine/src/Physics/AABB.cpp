#include "AABB.h"
#include "..\Graphics\ImGui\imgui.h"
#include <map>

bool AABB::Initialize(Entity * owner)
{
	m_owner = owner;

	return false;
}

bool AABB::Initialize(Entity * owner, float radius, eColliderType colliderType)
{
	m_owner = owner;
	m_sphereRadius = radius;
	m_colliderType = colliderType;
	return false;
}

void AABB::Update(const float & deltaTime)
{
	m_position = m_owner->GetTransform().GetPosition();
}

bool AABB::Intersects(AABB * collider)
{
	if (m_colliderType == eColliderType::SPHERE && collider->GetColliderType() == eColliderType::SPHERE)
		return SphereIntersects(*collider);

	if (m_colliderType == eColliderType::BOX && collider->GetColliderType() == eColliderType::BOX)
	{	
		// TODO: Implement BOX -> BOX collision
	}
	if (m_colliderType == eColliderType::BOX && collider->GetColliderType() == eColliderType::SPHERE)
	{
		// TODO: Implement BOX -> SPHERE collision
	}
	if (m_colliderType == eColliderType::SPHERE && collider->GetColliderType() == eColliderType::BOX)
	{
		// TODO: Implement SPHERE -> BOX collision
	}

	return false;
}

bool AABB::SphereIntersects(AABB & collider)
{
	double deltaXSquared = m_owner->GetTransform().GetPosition().x - collider.GetOwner()->GetTransform().GetPosition().x;
	deltaXSquared *= deltaXSquared;

	double deltaYSquared = m_owner->GetTransform().GetPosition().y - collider.GetOwner()->GetTransform().GetPosition().y;
	deltaYSquared *= deltaYSquared;

	double deltaZSquared = m_owner->GetTransform().GetPosition().z - collider.GetOwner()->GetTransform().GetPosition().z;
	deltaZSquared *= deltaZSquared;

	double sumRadiiSquared = m_sphereRadius + collider.GetRadius();
	sumRadiiSquared *= sumRadiiSquared;

	if (deltaXSquared + deltaYSquared + deltaZSquared <= sumRadiiSquared)
		return true;

	return false;
}

AABB::eColliderType AABB::GetColliderTypeFromString(std::string str_collider)
{
	std::map<std::string, eColliderType> stringToColliderType;

	stringToColliderType["SPHERE"] = eColliderType::SPHERE;
	stringToColliderType["BOX"] = eColliderType::BOX;

	std::map<std::string, eColliderType>::iterator iter;
	for (iter = stringToColliderType.begin(); iter != stringToColliderType.end(); iter++)
	{
		if ((*iter).first == str_collider)
			return (*iter).second;
	}
	return eColliderType::NONE;
}

std::string AABB::GetGetColliderTypeAsString()
{
	std::map<std::string, eColliderType> stringToColliderType;
	stringToColliderType["SPHERE"] = eColliderType::SPHERE;
	stringToColliderType["BOX"] = eColliderType::BOX;

	std::map<std::string, eColliderType>::iterator iter;
	for (iter = stringToColliderType.begin(); iter != stringToColliderType.end(); iter++)
	{
		if ((*iter).second == this->m_colliderType)
			return (*iter).first;
	}

	return "ERROR: Could not determine collider type";
}

bool AABB::InitFromJSON(const rapidjson::Value & componentInformation)
{
	float radius;
	float width;
	float height;
	float depth;
	std::string colliderType;

	json::get_string(componentInformation[0], "ColliderType", colliderType);
	if (colliderType == "SPHERE")
	{
		m_colliderType = eColliderType::SPHERE;
		json::get_float(componentInformation[0], "Radius", radius);
		m_sphereRadius = radius;
	}
	if (colliderType == "BOX")
	{
		m_colliderType = eColliderType::BOX;
		json::get_float(componentInformation[0], "Width", width);
		json::get_float(componentInformation[0], "Height", height);
		json::get_float(componentInformation[0], "Depth", depth);

		m_width = width;
		m_height = height;
		m_depth = depth;
	}

	return true;
}

void AABB::WriteToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.Key("AABB");
	writer.StartArray();//Start Array

	writer.StartObject();

	writer.Key("ColliderType");
	writer.String(this->GetGetColliderTypeAsString().c_str());

	if (m_colliderType == eColliderType::SPHERE)
	{
		writer.Key("Radius");
		writer.Double(m_sphereRadius);
	}
	if (m_colliderType == eColliderType::BOX)
	{
		writer.Key("Width");
		writer.Double(m_width);

		writer.Key("Height");
		writer.Double(m_height);

		writer.Key("Depth");
		writer.Double(m_depth);
	}

	writer.EndObject();

	writer.EndArray();//End Array
}

void AABB::OnImGuiRender()
{
	ImGui::Text("Colider Type: ");
	ImGui::SameLine();
	ImGui::Text(GetGetColliderTypeAsString().c_str());

	if (m_colliderType == eColliderType::SPHERE)
	{
		ImGui::DragFloat("Radius", &m_sphereRadius, 0.01f, 0.0f, 100.0f);
	}
	if (m_colliderType == eColliderType::BOX)
	{
		ImGui::DragFloat("Width", &m_width, 0.01f, 0.0f, 100.0f);
		ImGui::DragFloat("Height", &m_height, 0.01f, 0.0f, 100.0f);
		ImGui::DragFloat("Depth", &m_depth, 0.01f, 0.0f, 100.0f);

	}

}


