#include "Entity.h"
#include "..\\Components\MeshRenderComponent.h"
#include "..\\Components\\EditorSelectionComponent.h"
#include "..\\Components\\LuaScriptComponent.h"
#include "..\Editor\Editor.h"

bool Entity::Initialize()
{
	bool success = true;

	UpdateTransformCopyWithTransform();
	
	return success;
}

void Entity::Update(const float& deltaTime)
{
	if (m_pParent != nullptr) // Multiply the parent matrix byt this matrix to ge tthe world matrix
	{
		//XMMATRIX worldMat = m_transform.GetWorldMatrix() * m_pParent->GetWorldMatrix();
		//this->m_transform.SetWorldMatrix(worldMat);
		//this->m_transform.GetWorldMatrix() = this->m_transform.GetWorldMatrix() * m_pParent->GetWorldMatrix();
		m_transform.AdjustPosition(m_pParent->GetPosition().x, m_pParent->GetPosition().y, m_pParent->GetPosition().z);
	}
	else
	{
		m_transform.AdjustPosition(0.0f, 0.0f, 0.0f);
	}

	// If the editor is not playing keep coppying the transforms
	if(!Debug::Editor::Instance()->PlayingGame())
		UpdateTransformCopyWithTransform();

	// If editor is present do this if not just remove this
	EditorSelection* es = GetComponent<EditorSelection>();
	if (es != nullptr)
	{
		es->SetPosition(m_transform.GetPosition());
	}

}

void Entity::Draw(const XMMATRIX & viewProjectionMatrix, const XMMATRIX & viewMatrix)
{
	MeshRenderer* mr = GetComponent<MeshRenderer>();

	if (mr != nullptr)
	{
		mr->SetWorldMat(this->m_transform.GetWorldMatrix());
		mr->Draw(viewProjectionMatrix, viewMatrix);
	}
}

void Entity::Destroy()
{
	for (Component* component : m_components)
	{
		component->Destroy();
		delete component;
	}
	m_components.clear();
}

void Entity::OnStart()
{

}

void Entity::OnUpdate(float deltaTime)
{
	//m_transform.AdjustRotation(0.0f, 0.0f, 0.001f * deltaTime); // This line is not needed, It is a Debug line to test Playing Game feature
	if (m_pParent != nullptr)
		m_transform.AdjustPosition(m_transform.GetPosition().x + m_pParent->GetPosition().x, m_transform.GetPosition().y + m_pParent->GetPosition().y, m_transform.GetPosition().z + m_pParent->GetPosition().z);
	

	for (Component* component : m_components)
	{
		component->Update(deltaTime);
	}
}

void Entity::OnExit()
{
}

void Entity::OnEditorStop()
{
	UpdateTransformWithCopy();
}

void Entity::OnEvent(const Event & event)
{
}

void Entity::AddComponent(Component * component)
{
	assert(component);
	assert(std::find(m_components.begin(), m_components.end(), component) == m_components.end());

	m_components.push_back(component);
}

void Entity::RemoveComponent(Component * component)
{
	assert(std::find(m_components.begin(), m_components.end(), component) != m_components.end());

	auto iter = std::find(m_components.begin(), m_components.end(), component);
	(*iter)->Destroy();
	delete *iter;
	m_components.erase(iter);
}
