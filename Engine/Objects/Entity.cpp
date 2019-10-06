#include "Entity.h"
#include "..\\Components\MeshRenderComponent.h"
#include "..\\Components\\EditorSelectionComponent.h"

bool Entity::Start()
{
	bool success = true;
	this->m_transform.SetPosition(0.0f, 0.0f, 0.0f);
	this->m_transform.SetRotation(0.0f, 0.0f, 0.0f);
	this->m_transform.UpdateMatrix();

	

	return success;
}

void Entity::Update(float deltaTime)
{
	m_transform.AdjustPosition(0.0f, 0.0f, 0.0f);
	//m_transform.AdjustRotation(0.0f, 0.001f * deltaTime, 0.0f);

	EditorSelection* es = GetComponent<EditorSelection>();
	if (es != nullptr)
	{
		es->SetPosition(m_transform.GetPosition());
	}

	for (Component* component : m_components)
	{
		component->Update();
	}
}

void Entity::Draw(const XMMATRIX & viewProjectionMatrix)
{
	MeshRenderer* mr = GetComponent<MeshRenderer>();

	if (mr != nullptr)
	{
		mr->SetWorldMat(this->m_transform.GetWorldMatrix());
		mr->Draw(viewProjectionMatrix);
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

bool Entity::OnStart()
{
	return true;
}

void Entity::OnUpdate(float deltaTime)
{
}

void Entity::OnDestroy()
{
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
