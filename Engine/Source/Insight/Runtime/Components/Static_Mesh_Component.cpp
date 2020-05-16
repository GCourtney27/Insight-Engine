#include <ie_pch.h>

#include "Insight/Runtime/AActor.h"
#include "Static_Mesh_Component.h"

#include "imgui.h"

namespace Insight {



	StaticMeshComponent::StaticMeshComponent(StrongActorPtr owner)
		: ActorComponent("Static Mesh Component", owner)
	{
		m_pModel = make_unique<StaticMesh>();
		m_pModel->Init("../Assets/Objects/nanosuit/nanosuit.obj");;
	}

	StaticMeshComponent::~StaticMeshComponent()
	{
	}

	void StaticMeshComponent::OnInit()
	{
	}

	void StaticMeshComponent::OnDestroy()
	{
	}

	void StaticMeshComponent::OnRender()
	{
		// TODO: This is where we should request the draw call from the model manager by passing in the world transform
		m_pModel->Draw();//ActorComponent::m_pOwner->GetTransform().GetWorldMatrix()
	}

	void StaticMeshComponent::OnUpdate(const float& deltaTime)
	{
	}

	void StaticMeshComponent::OnImGuiRender()
	{
	}

	void StaticMeshComponent::RenderSceneHeirarchy()
	{
		//if (ImGui::TreeNode(m_ComponentName))
		{
			m_pModel->RenderSceneHeirarchy();

			//ImGui::TreePop();
			//ImGui::Spacing();
		}
	}

	void StaticMeshComponent::OnAttach()
	{
	}

	void StaticMeshComponent::OnDetach()
	{
	}

}
