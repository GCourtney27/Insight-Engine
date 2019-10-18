#include "EditorSelectionComponent.h"
#include "..\Graphics\ImGui\imgui.h"

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

void EditorSelection::JSONLoad()
{
}


//void EditorSelectionComponent::Update(const DirectX::XMFLOAT3 parentPosition)
//{
//	this->m_position = parentPosition;
//}
