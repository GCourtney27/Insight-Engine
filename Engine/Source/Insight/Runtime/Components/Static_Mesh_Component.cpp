#include <ie_pch.h>

#include "Static_Mesh_Component.h"

namespace Insight {



	StaticMeshComponent::StaticMeshComponent(StrongActorPtr owner)
		: ActorComponent("Static Mesh Component", owner)
	{
		m_pModel = make_unique<Model>();
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

	void StaticMeshComponent::OnImGuiRender()
	{
	}

	void StaticMeshComponent::OnAttach()
	{
	}

	void StaticMeshComponent::OnDetach()
	{
	}

}
