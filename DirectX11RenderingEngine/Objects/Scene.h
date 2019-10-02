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

	bool Initialize();
	void Update(float deltaTime); // Call this in engine.cpp and pass in delta time
	void Draw();
	void Shutdown();

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

protected:
	std::list<Entity*> m_entities;
};