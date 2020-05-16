#pragma once

#include <Insight/Core.h>

#include "Actor_Component.h"
#include "Insight/Rendering/Geometry/Static_Mesh.h"

namespace Insight {

	class INSIGHT_API StaticMeshComponent : public ActorComponent
	{
	public:
		StaticMeshComponent(StrongActorPtr owner);
		virtual ~StaticMeshComponent();
		
		virtual void OnInit() override;
		virtual void OnPostInit() {}
		virtual void OnDestroy() override;
		virtual void OnRender() override;
		virtual void OnUpdate(const float& deltaTime);
		virtual void OnChanged() {}
		virtual void OnImGuiRender() override;
		void RenderSceneHeirarchy() override;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

	private:

	private:
		unique_ptr<StaticMesh> m_pModel = nullptr;
	};

}
