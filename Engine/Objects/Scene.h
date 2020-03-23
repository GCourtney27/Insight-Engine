#pragma once
#include "ID.h"
#include "..\Systems\RenderManager.h"
#include "..\Systems\PhysicsSystem.h"
#include "..\Systems\LuaManager.h"
#include <list>
#include <map>
#include <assert.h>
#include <DirectXMath.h>

class Entity;
class Engine;

class Scene
{
public:
	Scene() {  }
	~Scene() {  }

	// -- Engine -- //
	bool Initialize(Engine* engine);
	void Update(const float& deltaTime);
	void Draw(const DirectX::XMMATRIX & projectionMatrix, const DirectX::XMMATRIX & viewMatrix);
	void Flush();
	void Shutdown();

	// -- Runtime (Game Logic) -- // 
	bool OnStart();
	void OnUpdate(const float & deltaTime);
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

	void AddInstantiatedEntity(Entity* entity);
	void ClearInstantiatedEntities();

	Entity* GetEntityWithID(const ID& id);
	std::vector<Entity*> GetEntitiesWithTag(const ID& tag);
	std::list<Entity*>* GetAllEntities() { return &m_entities; }

	std::string& GetSceneName() { return m_name; }
	void SetSceneName(std::string name) { m_name = name; }

	RenderManager& GetRenderManager() { return m_renderManager; }
	PhysicsSystem& GetPhysicsSystem() { return m_physicsSystem; }
	LuaManager& GetLuaManager() { return m_luaManager; }

	bool& IsPhysicsEnabled() { return m_physicsEnabled; }
	void SetPhysicsEnabled(bool enable) { m_physicsEnabled = enable; }

	Engine& GetEngineInstance() { return *m_pEngine; }

protected:
	RenderManager m_renderManager;

	bool m_physicsEnabled = false;
	PhysicsSystem m_physicsSystem;

	LuaManager m_luaManager;

	std::list<Entity*> m_entities;
	std::map<Entity*, float> m_instantiatedEntities;
	std::string m_name;
	std::string m_sceneDiretory;

	Engine* m_pEngine = nullptr;
};