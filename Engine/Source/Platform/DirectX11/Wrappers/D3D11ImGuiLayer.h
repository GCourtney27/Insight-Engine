#pragma once

#include <Runtime/Core.h>

#include "Runtime/Core/Public/Layer/ImGuiOverlay.h"

namespace Insight {

	class INSIGHT_API D3D11ImGuiLayer : public ImGuiLayer
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

	};

}
