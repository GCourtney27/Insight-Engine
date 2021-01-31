#pragma once

#include <Insight/Core.h>

#include "Insight/Core/Layer/Layer.h"
#include "Insight/Events/KeyEvent.h"
#include "Insight/Events/MouseEvent.h"
#include "Insight/Events/ApplicationEvent.h"


struct ImGuiIO;

namespace Insight {

	class INSIGHT_API ImGuiLayer : public Layer
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

		/*
			Translate a mouse button code from KeyMapCode to ImGuiMouseButton_
			Insight stores mouse codes differently than ImGui.
		*/
		inline void TranslateMouseCode(KeyMapCode MouseKey, uint8_t& ImGuiMouseCode)
		{
			if		(MouseKey & KeyMapCode_Mouse_Button_Left)	ImGuiMouseCode = 0;
			else if (MouseKey & KeyMapCode_Mouse_Button_Right)	ImGuiMouseCode = 1;
			else if (MouseKey & KeyMapCode_Mouse_Button_Middle) ImGuiMouseCode = 2;
		}
	protected:
		ImGuiIO* m_pIO = nullptr;
	};

}
