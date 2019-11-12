#include "Scene.h"
#include "Entity.h"

#include <assert.h>
#include <algorithm>

bool Scene::Initialize()
{
	std::list<Entity*>::iterator iter;
	for (iter = m_entities.begin(); iter != m_entities.end(); iter++)
	{
		(*iter)->Initialize();
	}
	return true;
}

void Scene::Update(const float& deltaTime)
{
	if(m_physicsEnabled)
		m_physicsSystem.Simulate(deltaTime);

	for (Entity* entity : m_entities)
	{
		entity->Update(deltaTime);
	}
}

void Scene::Draw(const DirectX::XMMATRIX & projectionMatrix, const DirectX::XMMATRIX & viewMatrix)
{
	m_renderManager.Draw(projectionMatrix, viewMatrix);

	/*for (Entity* entity : m_entities)
	{
		entity->Draw(projectionMatrix, viewMatrix);
	}*/
}

void Scene::Shutdown()
{
	for (Entity* entity : m_entities)
	{
		delete entity;
	}
	m_entities.clear();
}

// -- Runtime -- //
bool Scene::OnStart()
{
	for (Entity* entity : m_entities)
	{
		entity->OnStart();
	}
	return true;
}

void Scene::OnUpdate(const float & deltaTime)
{
	m_physicsSystem.Simulate(deltaTime);

	for (Entity* entity : m_entities)
	{
		entity->OnUpdate(deltaTime);
	}
}

void Scene::OnDestroy()
{
}


void Scene::AddEntity(Entity* entity)
{
	/*assert(std::find(m_entities.begin(), m_entities.end(), entity) != m_entities.end());
	assert(entity);*/

	m_entities.push_back(entity);
}

std::list<Entity*>::iterator Scene::RemoveEntity(Entity * entity, bool destroy)
{
	assert(std::find(m_entities.begin(), m_entities.end(), entity) != m_entities.end());
	assert(entity);

	auto iter = std::find(m_entities.begin(), m_entities.end(), entity);
	if (iter != m_entities.end())
	{
		if (destroy)
		{
			(*iter)->Destroy(); // shutdown all objects and components associated with the Entity
			delete *iter; // Delete the Enitity
		}
		iter = m_entities.erase(iter);
	}

	return iter;
}

Entity * Scene::GetEntityWithID(const ID & id)
{
	Entity* entity = nullptr;

	for (Entity* _entity : m_entities)
	{
		if(*_entity == id) // !!! comparing an Entity type to a ID type? Why?
		{
			entity = _entity;
			break;
		}
	}
	return entity;
}

std::vector<Entity*> Scene::GetEntitiesWithTag(const ID & tag)
{
	std::vector<Entity*> entities;

	for (Entity* entity : m_entities)
	{
		if (entity->GetID() == tag)
		{
			entities.push_back(entity);
		}
	}
	return entities;
}
