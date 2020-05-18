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
		virtual void OnPreRender(XMMATRIX& parentMatrix) override;
		virtual void OnRender() override;
		virtual void OnUpdate(const float& deltaTime);
		virtual void OnChanged() {}
		virtual void OnImGuiRender() override;
		void RenderSceneHeirarchy() override;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

	private:

	private:
		StrongModelPtr m_pModel;

		ID3D12Resource* m_ConstantBufferUploadHeaps = nullptr;
		ID3D12GraphicsCommandList* m_pCommandList = nullptr;
		int ConstantBufferPerObjectAlignedSize = (sizeof(CB_VS_PerObject) + 255) & ~255;
	};

}
