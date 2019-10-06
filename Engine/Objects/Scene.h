#pragma once
#include "ID.h"
//#include "engine.h"
#include <vector>
#include <list>
#include <assert.h>

class Entity;

class Scene
{
public:
	Scene() {}
	~Scene() {}

	// -- Engine -- //
	bool Initialize();
	void Update(const float& deltaTime);
	void Draw();
	void Shutdown();

	// -- Runtime (Game Logic) -- // 
	bool OnStart();
	void OnUpdate(float deltaTime);
	void OnDestroy();

	template<typename T>
	T* AddEntity(const ID& id = ID())
	{
		T* entity = new T(this, id);
		m_entities.push_back(entity);
		return entity;
	}

	void AddEntity(Entity* entity);
	std::list<Entity*>::iterator RemoveEntity(Entity* entity, bool destroy = true);

	Entity* GetEntityWithID(const ID& id);
	std::vector<Entity*> GetEntitiesWithTag(const ID& tag);
	std::list<Entity*>* GetAllEntities() { return &m_entities; }



protected:
	std::list<Entity*> m_entities;
	char* m_pName;
};