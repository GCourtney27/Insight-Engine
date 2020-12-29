#pragma once

#include <Insight/Core.h>

#include "Insight/Core/Layer/ImGui_Layer.h"


namespace Insight {
	
	class Direct3D12Context;
	
	class INSIGHT_API D3D12ImGuiLayer : public ImGuiLayer
	{
	private:
		using Super = ImGuiLayer;
	public:
		D3D12ImGuiLayer() = default;
		~D3D12ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		virtual void Begin() override;
		virtual void End() override;

	private:
		Direct3D12Context* m_pRenderContextRef;
		ID3D12DescriptorHeap* m_pDescriptorHeap = nullptr;
		ID3D12GraphicsCommandList* m_pCommandList = nullptr;
	};

}
