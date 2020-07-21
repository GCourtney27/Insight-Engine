#pragma once

#include <Insight/Core.h>

#include "Insight/Core/Layer/Layer.h"
#include "Insight/Events/Application_Event.h"
#include "Insight/Events/Key_Event.h"
#include "Insight/Events/Mouse_Event.h"

struct ImGuiIO;

namespace Insight {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
		void OnEvent(Event& event);

	private:
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseScrollEvent(MouseScrolledEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnMouseRawMoveEvent(MouseRawMoveEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyTypedEvent(KeyTypedEvent& e);
		bool OnWindowResizedEvent(WindowResizeEvent& e);
	private:
		HWND* m_pWindowHandle = nullptr;
		ID3D12DescriptorHeap* m_pDescriptorHeap = nullptr;
		ID3D12GraphicsCommandList* m_pCommandList = nullptr;
		ImGuiIO* m_pIO = nullptr;
	};

}
