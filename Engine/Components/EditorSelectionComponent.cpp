#include "EditorSelectionComponent.h"
#include "..\Graphics\ImGui\imgui.h"
#include "..\Objects\Entity.h"

void EditorSelection::InitFromJSON(Entity* owner, const rapidjson::Value& componentInformation)
{
	this->Initialize(owner, 10.0f, owner->GetTransform().GetPosition());

	/*EditorSelection* finalComp = nullptr;
	std::string mode;
	for (rapidjson::SizeType e = 0; e < editorSelection.Size(); e++)
	{
		json::get_string(editorSelection[e], "Mode", mode);
		if (mode != "OFF")
			finalComp = entity->AddComponent<EditorSelection>();
		else
			break;
	}
	if (finalComp != nullptr)
		finalComp->Initialize(entity, 10.0f, entity->GetTransform().GetPosition());*/

}

void EditorSelection::Initialize(Entity* owner, float radius, const DirectX::XMFLOAT3 & position)
{
	this->m_owner = owner;
	m_position = position;
	m_radius = radius;
	SetName("Editor Selection (ENGINE DEBUG)");

}

void EditorSelection::Update(float deltaTime)
{
}

void EditorSelection::Destroy()
{
}

void EditorSelection::OnImGuiRender()
{

	ImGui::Text(GetName());

}




//void EditorSelectionComponent::Update(const DirectX::XMFLOAT3 parentPosition)
//{
//	this->m_position = parentPosition;
//}
