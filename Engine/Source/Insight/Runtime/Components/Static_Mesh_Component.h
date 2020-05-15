#pragma once

#include <Insight/Core.h>

#include "Actor_Component.h"
#include "Insight/Rendering/Geometry/Model.h"

namespace Insight {

	class INSIGHT_API StaticMeshComponent : public ActorComponent
	{
	public:
		StaticMeshComponent(StrongActorPtr owner);
		virtual ~StaticMeshComponent();
		
		virtual void OnInit() override;
		virtual void OnPostInit() {}
		virtual void OnDestroy() override;
		virtual void OnUpdate(const float& deltaTime) {}
		virtual void OnChanged() {}
		virtual void OnImGuiRender() override;


		virtual void OnAttach() override;
		virtual void OnDetach() override;

	private:

	private:
		unique_ptr<Model> m_pModel = nullptr;
	};

}
