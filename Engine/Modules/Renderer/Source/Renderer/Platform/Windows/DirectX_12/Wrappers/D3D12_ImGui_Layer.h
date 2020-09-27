#pragma once

#include <Retina/Core.h>

#include "Retina/Layer_Types/ImGui_Layer.h"


namespace Retina {
	
	class RETINA_API D3D12ImGuiLayer : public ImGuiLayer
	{
	public:
		D3D12ImGuiLayer() = default;
		~D3D12ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		virtual void Begin() override;
		virtual void End() override;

	private:
		HWND* m_pWindowHandle = nullptr;
		ID3D12DescriptorHeap* m_pDescriptorHeap = nullptr;
		ID3D12GraphicsCommandList* m_pCommandList = nullptr;
	};

}
