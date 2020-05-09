#pragma once

#include <Insight/Core.h>

#include "Insight/Runtime/Components/Actor_Component.h"


namespace Insight {

	class INSIGHT_API StaticMeshComponent : public Component
	{
	public:
		StaticMeshComponent(AActor* owner, ID& id);
		~StaticMeshComponent();

		// TODO: Should just hold a index inside mesh manager to the desired mesh to render

	private:

	};

}