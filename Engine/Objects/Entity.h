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
	Entity(Scene* scene, const ID& id) 
		: m_scene(scene), Object(id) {}

	virtual ~Entity() {}

	// -- Editor Specific methods (Engine) -- //
	virtual bool Initialize(); // Called once when Engine starts
	virtual void Update(const float& deltaTime); // Update things like editor click sphere
	virtual void Draw(const XMMATRIX & viewProjectionMatrix, const XMMATRIX & viewMatrix); // Draw mesh renderer for componet, needs to be seen in editor and play mode
	virtual void Destroy(); // Unload resources

	// -- Runtime Methods (Components / Game Logic) -- //
	virtual void OnStart(); // Run Start() components
	virtual void OnUpdate(const float& deltaTime); // Update() components 
	virtual void OnExit(); // When entity exits play

	void OnEditorStop(); //Reset positions and transforms

	virtual void WriteToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	bool CanBeJSONSaved() { return m_canBeJSONSaved; }
	void SetCanBeJSONSaved(bool canBeSaved) { m_canBeJSONSaved = canBeSaved; }

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
	std::vector<Component*> GetAllComponents() const { return m_components; }

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

	void lua_AdjustPosition(float x, float y, float z) { GetTransform().AdjustPosition(x, y, z); }
	void lua_AdjustRotation(float x, float y, float z) { GetTransform().AdjustRotation(x, y, z); }
	void lua_AdjustScale(float x, float y, float z) { GetTransform().AdjustScale(x, y, z); }

	ID& GetID() { return m_id; }
	void SetTagUID(std::string id) { m_id.SetUniqueID(id); }

	void UpdateTransformCopyWithTransform() { m_origionalTransform = m_transform; }
	void UpdateTransformWithCopy() { m_transform = m_origionalTransform; }
	

protected:
	eState m_state = eState::ACTIVE;
	ID m_id;
	bool m_canBeJSONSaved = true;
	std::vector<Transform*> m_children;

	Transform m_transform;
	Transform m_origionalTransform;
	Scene* m_scene;
	std::vector<Component*> m_components;
};