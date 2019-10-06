#pragma once
#include "Object.h"
#include "../Components/Component.h"
#include "Scene.h"
#include "../Math/Transform.h"
#include "../Event/Event.h"
#include "../Event/EventReciever.h"
#include "..\Scriptor\CompilerHelper.h"
#include <vector>
#include <assert.h>

class Entity : public Object, public EventReciever
{
public:
	enum eState
	{
		INACTIVE,
		ACTIVE,
		DESTROY,
		RESPAWN
	};

public:
	Entity(Scene* scene, const ID& id) : m_scene(scene), Object(id) {}
	virtual ~Entity() {}

	// -- Editor Specific methods -- //
	bool Start();
	void Update(float deltaTime);
	void Draw(const XMMATRIX & viewProjectionMatrix);
	void Destroy();

	// -- Runtime Methods -- //
	virtual bool OnStart();
	virtual void OnUpdate(float deltaTime);
	virtual void OnDestroy();

	virtual void OnEvent(const Event& event);

	template<typename T>
	T* AddComponent()
	{
		T* component = new T(this);
		assert(dynamic_cast<Component*>(component));

		m_components.push_back(component);
		return component;
	}

	void AddComponent(Component* component);
	void RemoveComponent(Component* component);

	template<typename T>
	T* GetComponent()
	{
		T* component = nullptr;
		for (Component* _component : m_components)
		{
			component = dynamic_cast<T*>(_component);
			if (component != nullptr) break;
		}
		return component;
	}

	eState GetState() { return m_state; }
	void SetState(eState state) { m_state = state; }

	Scene* GetScene() { return m_scene; }
	Transform& GetTransform() { return m_transform; }
	
	ID& GetID() { return m_id; }

	void SetTagUID(std::string id) { m_id.SetUniqueID(id); }

protected:
	eState m_state = eState::ACTIVE;
	ID m_id;
	Transform m_transform;
	Scene* m_scene;
	std::vector<Component*> m_components;
};