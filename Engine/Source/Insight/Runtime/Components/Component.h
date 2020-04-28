#pragma once
#include "Insight/Runtime/Identification/Object.h"

class Actor;

class Component : public Object
{
public:
	virtual void Destroy() = 0;
	virtual void BeginPlay() = 0;
	virtual void Tick(const float& deltaTime) = 0;
	virtual void OnImGuiRender() = 0;

	virtual void OnAttach() = 0;
	virtual void OnDetach() = 0;

	bool& GetIsComponentEnabled() { return m_Enabled; }
	void SetComponentEnabled(bool enable) { m_Enabled = enable; }

	Actor* GetOwner() { return m_Owner; }

	void SetName(const char* name) { m_Name = name; }
	const char* GetName() { return m_Name; }
protected:
	Component(Actor* owner, const ID& id = ID())
		: m_Owner(owner), Object(id) {}
protected:
	Actor* m_Owner = nullptr;
	const char* m_Name = nullptr;
	bool m_Enabled = true;


};