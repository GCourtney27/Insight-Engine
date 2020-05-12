#include "ie_pch.h"

#include "AActor.h"
#include "Insight/Runtime/Components/Actor_Component.h"


namespace Insight {



	AActor::AActor(ActorId id)
		: m_id(id)
	{
		
	}
	
	AActor::~AActor()
	{
	}

	bool AActor::OnInit()
	{
		return true;
	}

	void AActor::OnPostInit()
	{
	}

	void AActor::OnUpdate(const float& deltaMs)
	{
		size_t numComponents = m_Components.size();
		for (size_t i = 0; i < numComponents; ++i)
		{
			m_Components[i]->OnUpdate(deltaMs);
		}
	}

	void AActor::OnRender()
	{
	}

	void AActor::BeginPlay()
	{
	}

	void AActor::Tick()
	{
	}

	void AActor::Destroy()
	{
	}

	void AActor::AddComponent(StrongActorComponentPtr pComponent)
	{
		IE_CORE_ASSERT(pComponent, "Attempting to add component to actor that is NULL");
		IE_CORE_ASSERT(std::find(m_Components.begin(), m_Components.end(), pComponent) == m_Components.end(), "Failed to add component to Actor");

		m_Components.push_back(pComponent);
	}

	void AActor::RemoveComponent(StrongActorComponentPtr component)
	{
		IE_CORE_ASSERT(std::find(m_Components.begin(), m_Components.end(), component) != m_Components.end(), "Could not find Component in Actor list while attempting to delete");

		auto iter = std::find(m_Components.begin(), m_Components.end(), component);
		(*iter)->OnDestroy();
		(*iter).reset();
		m_Components.erase(iter);
	}

}

