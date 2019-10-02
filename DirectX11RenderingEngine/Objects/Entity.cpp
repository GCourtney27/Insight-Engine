#include "Entity.h"

bool Entity::Start()
{
	bool success = true;

	return success;
}

void Entity::Update(float deltaTime)
{
	for (Component* component : m_components)
	{
		component->Update();
	}
}

void Entity::Draw()
{
	// update the Mesh Renderer component
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
