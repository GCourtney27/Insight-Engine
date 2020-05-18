#include <ie_pch.h>

#include "Insight/Runtime/Components/Actor_Component.h"
#include "AActor.h"
#include "imgui.h"

namespace Insight {



	AActor::AActor(ActorId id, ActorName actorName)
		: m_id(id)
	{
		Super::SetDisplayName(actorName);
	}
	
	AActor::~AActor()
	{
	}

	// Draw the heirarchy of the actor and its children to ImGui
	void AActor::RenderSceneHeirarchy()
	{
		if (ImGui::TreeNode(Super::GetDisplayName()))
		{
			Super::RenderSceneHeirarchy();
			size_t numComponents = m_Components.size();
			for (size_t i = 0; i < numComponents; ++i)
			{
				m_Components[i]->RenderSceneHeirarchy();
			}
			ImGui::TreePop();
			ImGui::Spacing();
		}
	}

	bool AActor::OnInit()
	{
		Super::OnInit();
		return true;
	}

	bool AActor::OnPostInit()
	{
		Super::OnPostInit();

		return true;
	}

	void AActor::OnUpdate(const float& deltaMs)
	{
		Super::OnUpdate(deltaMs);

		if (m_Parent)
			GetTransformRef().GetWorldMatrixRef() = m_Parent->GetTransform().GetWorldMatrix() * GetTransformRef().GetLocalMatrix();
		else
			GetTransformRef().SetWorldMatrix(GetTransformRef().GetLocalMatrix());

		size_t numComponents = m_Components.size();
		for (size_t i = 0; i < numComponents; ++i)
		{
			m_Components[i]->OnUpdate(deltaMs);
		}
	}

	void AActor::OnPreRender(XMMATRIX& matrix)
	{
		// Render Children
		Super::OnPreRender(GetTransform().GetLocalMatrix());
		// Render Components
		size_t numComponents = m_Components.size();
		for (size_t i = 0; i < numComponents; ++i) {
			m_Components[i]->OnPreRender(GetTransform().GetLocalMatrix());
		}
	}

	void AActor::OnRender()
	{
		// Render Children
		Super::OnRender();
		// Render Components
		size_t numComponents = m_Components.size();
		for (size_t i = 0; i < numComponents; ++i) {
			m_Components[i]->OnRender();
		}
	}

	void AActor::BeginPlay()
	{
		Super::BeginPlay();

	}

	void AActor::Tick(const float& deltaMs)
	{
		Super::Tick(deltaMs);

	}

	void AActor::Exit()
	{
		Super::Exit();

	}

	void AActor::Destroy()
	{
		Super::Destroy();

	}

	void AActor::AddComponent(StrongActorComponentPtr pComponent)
	{
		IE_CORE_ASSERT(pComponent, "Attempting to add component to actor that is NULL");
		IE_CORE_ASSERT(std::find(m_Components.begin(), m_Components.end(), pComponent) == m_Components.end(), "Failed to add component to Actor");
		
		pComponent->OnAttach();
		m_Components.push_back(pComponent);
	}

	void AActor::RemoveComponent(StrongActorComponentPtr component)
	{
		IE_CORE_ASSERT(std::find(m_Components.begin(), m_Components.end(), component) != m_Components.end(), "Could not find Component in Actor list while attempting to delete");

		auto iter = std::find(m_Components.begin(), m_Components.end(), component);
		(*iter)->OnDetach();
		(*iter)->OnDestroy();
		(*iter).reset();
		m_Components.erase(iter);
	}

}

