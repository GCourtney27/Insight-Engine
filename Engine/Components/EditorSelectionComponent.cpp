#include "EditorSelectionComponent.h"

void EditorSelection::Initialize(float radius, const DirectX::XMFLOAT3 & position)
{
	m_position = position;
	m_radius = radius;
	m_pName = "EditorSelection";
}

void EditorSelection::Update()
{
}

void EditorSelection::Destroy()
{
}

void EditorSelection::OnImGuiRender()
{
}


//void EditorSelectionComponent::Update(const DirectX::XMFLOAT3 parentPosition)
//{
//	this->m_position = parentPosition;
//}
