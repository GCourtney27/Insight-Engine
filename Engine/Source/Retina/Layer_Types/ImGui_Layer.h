#pragma once

#include <Retina/Core.h>

#include "Retina/Core/Layer/Layer.h"
#include "Retina/Events/Application_Event.h"
#include "Retina/Events/Key_Event.h"
#include "Retina/Events/Mouse_Event.h"

struct ImGuiIO;

namespace Retina {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		virtual ~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		virtual void Begin() = 0;
		virtual void End() = 0;
		void OnEvent(Event& event);

		bool IsMouseOverUI();

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

	protected:
		ImGuiIO* m_pIO = nullptr;
	};

}
