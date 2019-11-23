#include "RigidBodyComponent.h"
#include "..\Graphics\ImGui\imgui.h"
#include "..\Objects\Entity.h"
#include <string>
#include <cmath>
#include "..\Editor\Editor.h"

void RigidBody::InitFromJSON(Entity * owner, const rapidjson::Value & componentInformation)
{
	float radius = 0.0f;
	std::string colliderType;


	const rapidjson::Value & aabb = componentInformation[0]["AABB"];
	m_collider.InitFromJSON(aabb);

	json::get_bool(componentInformation[0], "IsStatic", m_static);


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
			writer.Key("IsStatic");
			writer.Bool(m_static);
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
	using namespace std;
	using namespace DirectX;

	m_collider.Update(deltaTime);
	
	if (m_restitution <= 0.0f)
		return;

	// set gravity
	if (m_gravityEnable)
	{
		XMFLOAT3 gravity = {0.0f, -79.8f, 0.0f};

		m_force.x += gravity.x;
		m_force.y += gravity.y;
		m_force.z += gravity.z;
	}

	// set velocity
	m_velocity.x += m_force.x * deltaTime;
	m_velocity.y += m_force.y * deltaTime;
	m_velocity.z += m_force.z * deltaTime;
	
	// velocity clamp
	float max_velocity = 60.0f;

	m_velocity.x = min(max_velocity, max(m_velocity.x, -max_velocity));
	m_velocity.y = min(max_velocity, max(m_velocity.y, -max_velocity));
	m_velocity.z = min(max_velocity, max(m_velocity.z, -max_velocity));

	// velocity drag
	m_velocity.x = m_velocity.x * pow(0.99f, deltaTime);
	m_velocity.y = m_velocity.y * pow(0.99f, deltaTime);
	m_velocity.z = m_velocity.z * pow(0.99f, deltaTime);

	XMVECTOR velocity = XMLoadFloat3(&m_velocity);
	XMVECTOR length_vec = XMVector3Length(velocity);
	XMFLOAT3 rate = {};
	XMStoreFloat3(&rate, length_vec);

	if (m_static)
	{
		m_velocity.x = 0.0f;
		m_velocity.y = 0.0f;
		m_velocity.z = 0.0f;
	}

	// set position
	m_deltaPos = m_owner->GetTransform().GetPosition();
	m_deltaPos.x = m_velocity.x * deltaTime;
	m_deltaPos.y = m_velocity.y * deltaTime;
	m_deltaPos.z = m_velocity.z * deltaTime;


	m_owner->GetTransform().AdjustRotation(m_velocity.z / 10000.0f, 0.0f, -m_velocity.x / 10000.0f);
	m_owner->GetTransform().AdjustPosition(m_deltaPos);

}

void RigidBody::OnImGuiRender()
{
	ImGui::Text(GetName());
	static bool isRBEnabled = this->GetIsComponentEnabled();
	if (ImGui::Checkbox("Is Physics Enabled", &isRBEnabled))
		this->SetComponentEnabled(isRBEnabled);

	ImGui::Checkbox("Is Gravity Enabled", &m_gravityEnable);
	ImGui::Checkbox("Is Static", &m_static);
	ImGui::DragFloat("Mass", &m_mass, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat("Restitution", &m_restitution, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat3("Velocity", &m_velocity.x, 0.01f, -100.0f, 100.0f);

	m_collider.OnImGuiRender();


}

void RigidBody::OnEditorStop()
{
	m_velocity.x = 0.0f;
	m_velocity.y = 0.0f;
	m_velocity.z = 0.0f;

	m_restitution = 1.0f;
}

void RigidBody::Translate(float x, float y, float z)
{
	m_velocity.x += x;
	m_velocity.y += y;
	m_velocity.z += z;

	m_owner->GetTransform().AdjustPosition(m_deltaPos);
}

bool RigidBody::IsCollidingWith(int colliderTag)
{
	
	//if (m_colliders.size() >= 1)
	{
		std::map<int, bool>::iterator collider;
		for (collider = m_colliders.begin(); collider != m_colliders.end(); collider++)
		{
			if ((*collider).first == colliderTag)
			{
				return true;
			}
		}
	}
	

	return false;
}


