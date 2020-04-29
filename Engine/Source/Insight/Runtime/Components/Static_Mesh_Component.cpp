#include "ie_pch.h"

#include "Static_Mesh_Component.h"

namespace Insight {



	StaticMeshComponent::StaticMeshComponent(AActor* owner, ID& id)
		: Component(owner, id)
	{
	}

	StaticMeshComponent::~StaticMeshComponent()
	{
	}

}