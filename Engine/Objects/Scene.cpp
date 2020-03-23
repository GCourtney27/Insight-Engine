#include "Scene.h"
#include "Entity.h"
#include "..\Components\MeshRenderComponent.h"
#include "..\Components\RigidBodyComponent.h"

#include <assert.h>
#include <algorithm>

bool Scene::Initialize(Engine* engine)
{
	std::list<Entity*>::iterator iter;
	for (iter = m_entities.begin(); iter != m_entities.end(); iter++)
	{
		(*iter)->Initialize();
	}

	m_pEngine = engine;

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

void Scene::Flush()
{
	for (Entity* entity : m_entities)
	{
		entity->Destroy();
		delete entity;
	}
	m_entities.clear();

	m_renderManager.Flush();
	m_physicsSystem.Flush();
	m_luaManager.Flush();

}

void Scene::Shutdown()
{
	for (Entity* entity : m_entities)
	{
		entity->Destroy();
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

	std::map<Entity*, float>::iterator iter;
	for (iter = m_instantiatedEntities.begin(); iter != m_instantiatedEntities.end(); )
	{
		(*iter).first->OnUpdate(deltaTime);

		(*iter).second -= deltaTime;
		if ((*iter).second <= 0.0f)
		{
			m_renderManager.RemoveOpaqueInstance( (*iter).first->GetComponent<MeshRenderer>() );
			m_physicsSystem.RemoveEntity( (*iter).first->GetComponent<RigidBody>() );

			m_instantiatedEntities.erase(iter++);
		}
		else
			++iter;
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

void Scene::AddInstantiatedEntity(Entity * entity)
{
	m_instantiatedEntities.emplace(entity, 1.0f);
}

void Scene::ClearInstantiatedEntities()
{
	m_instantiatedEntities.clear();
	m_renderManager.ClearInstanciatedOpaqueObjects();
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
