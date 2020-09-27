#pragma once

#include <Retina/Core.h>

#include "Retina/Layer_Types/ImGui_Layer.h"

namespace Retina {

	class RETINA_API D3D11ImGuiLayer : public ImGuiLayer
	{
	public:
		D3D11ImGuiLayer() = default;
		virtual ~D3D11ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		virtual void Begin() override;
		virtual void End() override;

	private:
		HWND* m_pWindowHandle = nullptr;
	};

}
