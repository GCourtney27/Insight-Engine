#pragma once

#include "Insight/Core.h"

#include "Insight/Runtime/Components/Component.h"
#include "Insight/Rendering/Geometry/Model.h"


namespace Insight {

	class INSIGHT_API StaticMeshComponent : public Component
	{
	public:
		StaticMeshComponent(Actor* owner, ID& id);
		~StaticMeshComponent();



	private:
		Model m_Model;

	};

}